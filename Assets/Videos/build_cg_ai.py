"""Run `python build_cg_ai.py` to create cg.mp4 (requires av, numpy, Pillow, opencv-python).

Uses the timeline, narration, BGM mix and fades from build_cg.py, with
AI-animated clips of the redrawn (anime-style) CG.

Per clip:
  1. LOCKS  - static regions (hero statue / statue rubble) are frozen to the clip's
              first frame. If the generated camera moved, the frozen patch follows
              the camera (similarity transform estimated from background features),
              so the statue never deforms - it only moves rigidly with the view.
  2. Camera - the AI clips use a locked camera; the slow pan / push-in is done here
              with a crop window (CAMERA).
  3. Timing - 1x speed and trimmed if the clip is longer than the segment; slowed
              down (with frame blending) if it is shorter.
  4. Fall   - op4 plays an empty storm-sky clip; for the last FALL_SECONDS the tower
              (cut out of the redrawn op4 CG) plunges in from above with motion blur
              and camera shake, then hard-cuts to op5 (impact shake via SHAKE).
  5. Label  - the generated clips carry an "AI生成" disclosure mark in a corner, which
              the pan would cut in and out. When AI_LABEL is set, the output is
              LABEL_BAR px taller than the picture, with the label in that bottom
              strip. An empty AI_LABEL adds neither text nor a strip.
Does NOT touch the subtitle / voice-timing files (timings are identical to cg.mp4).
"""

from fractions import Fraction
from pathlib import Path

import av
import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont

import build_cg as base

CLIP_DIR = base.DIRECTORY / "Opening_CG_AI_assets"
OUTPUT = base.DIRECTORY / "cg.mp4"
SIZE, FPS = base.SIZE, base.FPS
CLIP_W, CLIP_H = 1024, 768
MARK_BOX = (850, 690, 1024, 768)  # clip-corner disclosure mark; excluded from camera tracking
AI_LABEL = ""
AI_LABEL_FONT = Path("C:/Windows/Fonts/msyhbd.ttc")
AI_LABEL_SIZE = 28
LABEL_BAR = 40 if AI_LABEL else 0  # extra output rows only when a label is configured
OUT_SIZE = (SIZE[0], SIZE[1] + LABEL_BAR)

# op4: tower fall composited over the storm-sky clip.
TOWER_SOURCE = "409b37b0"   # redrawn op4 CG (tower hanging in the sky)
FALL_SECONDS = 0.35
FALL_OFFSETS = (-440, -235)  # tower y offset vs. its CG position: start (above frame) -> cut;
                             # -235 leaves only the top ~1/4 of the picture showing the tower
FALL_EASE = 1.5             # >1 accelerates
FALL_SCALE = (0.99, 1.02)   # slight growth while it approaches
TOWER_GAIN = 0.78           # match the darker storm sky
TOWER_PERIOD = 55           # window-row period, used to extend the tower upward

# Camera shake: name -> (start, seconds, amplitude in clip px); start "end" = last seconds.
SHAKE = {
    "op4": ("end", 0.35, 4.0),
    "op5": ("start", 0.45, 9.0),
}

# Shot ID -> hash prefix of its animated clip in CLIP_DIR.
CLIPS = {
    "op1": "aff7e939",
    "op2": "8a837359",
    "op3": "3dbbe3be",   # market lane (life returning) - distinct from op2 square
    "op4": "14ab6f01",   # empty storm sky; tower is composited (FALL)
    "op5": "58553dc9",
    "op6": "41482e39",
    "op7": "e87381c3",
    "op8": "ad7ac10a",
}

# Frozen rectangles (x0, y0, x1, y1) in clip pixels, taken from each clip's first frame.
LOCKS = {
    "op2": [(478, 236, 640, 515)],   # hero statue (figure + cape + pedestal top)
    "op3": [(492, 140, 580, 236)],   # distant statue silhouette above the rooftops
    "op6": [(440, 470, 790, 665)],   # broken pedestal + fallen statue / stone cape
}

# Crop window per shot: (first height, last height, first centre, last centre),
# centres are (x, y) in clip pixels; width = height * 4 / 3. Interpolated with ease-in-out.
CAMERA = {
    "op1": (680, 680, (468, 358), (556, 358)),   # slow pan right
    "op2": (680, 680, (556, 358), (468, 358)),   # slow pan left
    "op3": (712, 652, (512, 360), (512, 372)),   # slow push-in down the lane
    "op4": (700, 690, (512, 360), (512, 356)),   # held, barely creeping in
    "op5": (688, 630, (512, 360), (512, 392)),   # slow push-in toward the base
    "op6": (680, 680, (468, 358), (556, 358)),   # slow pan right
    "op7": (680, 680, (556, 358), (468, 358)),   # slow pan left
    "op8": (690, 672, (500, 362), (530, 370)),   # gentle drift with the walkers
}


def clip_path(shot: str) -> Path:
    prefix = CLIPS[shot]
    matches = sorted(CLIP_DIR.glob(f"{prefix}*.mp4"))
    if len(matches) != 1:
        raise FileNotFoundError(f"Clip for {shot}: expected one {prefix}*.mp4 in {CLIP_DIR}")
    return matches[0]


def load_clip(path: Path) -> tuple[list[np.ndarray], float]:
    with av.open(str(path)) as source:
        stream = source.streams.video[0]
        rate = float(stream.average_rate)
        frames = [frame.to_ndarray(format="rgb24") for frame in source.decode(stream)]
    if frames[0].shape[:2] != (CLIP_H, CLIP_W):
        raise ValueError(f"{path.name}: expected {CLIP_W}x{CLIP_H}, got {frames[0].shape[1]}x{frames[0].shape[0]}")
    return frames, rate


def lock_regions(frames: list[np.ndarray], rects: list[tuple], label: str) -> list[np.ndarray]:
    reference = frames[0]
    mask = np.zeros((CLIP_H, CLIP_W), np.float32)
    for x0, y0, x1, y1 in rects:
        mask[y0:y1, x0:x1] = 1
    mask = cv2.GaussianBlur(mask, (0, 0), 7)
    feature_mask = ((mask < 0.01) * 255).astype(np.uint8)
    mx0, my0, mx1, my1 = MARK_BOX
    feature_mask[my0:my1, mx0:mx1] = 0  # the mark never moves; it would bias tracking
    orb = cv2.ORB_create(5000)
    matcher = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)
    gray0 = cv2.cvtColor(reference, cv2.COLOR_RGB2GRAY)
    kp0, des0 = orb.detectAndCompute(gray0, feature_mask)
    result, scales = [reference], []
    previous = np.float32([[1, 0, 0], [0, 1, 0]])
    for frame in frames[1:]:
        kp, des = orb.detectAndCompute(cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY), None)
        transform = None
        if des is not None and des0 is not None:
            matches = matcher.match(des0, des)
            if len(matches) >= 20:
                src = np.float32([kp0[m.queryIdx].pt for m in matches])
                dst = np.float32([kp[m.trainIdx].pt for m in matches])
                transform, inliers = cv2.estimateAffinePartial2D(
                    src, dst, method=cv2.RANSAC, ransacReprojThreshold=2.0)
                if transform is not None and inliers.sum() < 20:
                    transform = None
        if transform is None:
            transform = previous
        # Light temporal smoothing so the frozen patch never jitters.
        transform = (0.6 * transform + 0.4 * previous).astype(np.float32)
        previous = transform
        scales.append(float(np.hypot(transform[0, 0], transform[1, 0])))
        warped = cv2.warpAffine(reference, transform, (CLIP_W, CLIP_H),
                                flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)
        alpha = cv2.warpAffine(mask, transform, (CLIP_W, CLIP_H))[..., None]
        result.append(np.rint(frame * (1 - alpha) + warped * alpha).astype(np.uint8))
    print(f"  {label}: statue locked, camera scale {min(scales):.3f}-{max(scales):.3f}")
    return result


def ease(t: float) -> float:
    return 0.5 - 0.5 * np.cos(np.pi * min(max(t, 0.0), 1.0))


def shake_offset(name: str, index: int, total: int) -> tuple[float, float]:
    if name not in SHAKE:
        return 0.0, 0.0
    where, seconds, amplitude = SHAKE[name]
    length = max(round(seconds * FPS), 1)
    local = index if where == "start" else index - (total - length)
    if not 0 <= local < length:
        return 0.0, 0.0
    t = local / length
    envelope = (1 - t) ** 2 if where == "start" else t ** 1.5  # impact decays; fall builds
    phase = local * 2.7
    return (amplitude * envelope * 0.6 * np.sin(phase * 1.3 + 1.1),
            amplitude * envelope * np.sin(phase))


def crop_window(name: str, index: int, total: int) -> tuple[float, float, float, float]:
    h0, h1, (x0, y0), (x1, y1) = CAMERA[name]
    t = ease(index / max(total - 1, 1))
    dx, dy = shake_offset(name, index, total)
    h = min(h0 + (h1 - h0) * t, CLIP_H)
    w = h * 4 / 3
    cx = min(max(x0 + (x1 - x0) * t + dx, w / 2), CLIP_W - w / 2)
    cy = min(max(y0 + (y1 - y0) * t + dy, h / 2), CLIP_H - h / 2)
    return cx - w / 2, cy - h / 2, w, h


def tower_sprite() -> tuple[np.ndarray, np.ndarray, int]:
    """Cut the tower (+ falling fragments) out of the redrawn op4 CG.

    Returns (rgb float32, alpha float32, rows_added_on_top) in clip pixels. The CG
    crops the tower at the top edge, so window rows are repeated upward to make it
    long enough to fall fully into view.
    """
    path = sorted(CLIP_DIR.glob(f"{TOWER_SOURCE}*.png"))[0]
    image = cv2.cvtColor(cv2.imread(str(path)), cv2.COLOR_BGR2RGB)
    image = cv2.resize(image, (CLIP_W, CLIP_H), interpolation=cv2.INTER_AREA)
    r, _, b = [cv2.GaussianBlur(image[..., i].astype(np.float32), (0, 0), 1.2) for i in range(3)]
    mask = ((r - b) > -6).astype(np.uint8)  # stone is warm/neutral, storm sky is blue-grey
    mask[480:], mask[:, :330], mask[:, 700:] = 0, 0, 0
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
    count, labels, stats, _ = cv2.connectedComponentsWithStats(mask, 8)
    order = np.argsort(-stats[1:, 4]) + 1
    tower = (labels == order[0]).astype(np.uint8)
    tower = cv2.morphologyEx(tower, cv2.MORPH_CLOSE, np.ones((7, 7), np.uint8))
    outside = (1 - tower).astype(np.uint8)
    cv2.floodFill(outside, np.zeros((CLIP_H + 2, CLIP_W + 2), np.uint8), (5, CLIP_H - 5), 0)
    tower = np.maximum(tower, outside)  # fill windows
    bx, by, bw, bh, _ = stats[order[0]]
    for i in order[1:]:
        x, y, _, _, area = stats[i]
        if 6 < area < 700 and y > by + bh - 90 and bx - 30 < x < bx + bw + 30:
            tower[labels == i] = 1  # crumbling fragments under the base
    alpha = cv2.GaussianBlur(tower.astype(np.float32), (0, 0), 0.8)
    rgb = image.astype(np.float32) * TOWER_GAIN
    band = slice(2 * TOWER_PERIOD, 4 * TOWER_PERIOD)  # two window rows, phase-aligned to row 0
    repeats = 6
    added = repeats * 2 * TOWER_PERIOD
    rgb = np.concatenate([np.tile(rgb[band], (repeats, 1, 1)), rgb], axis=0)
    alpha = np.concatenate([np.tile(alpha[band], (repeats, 1)), alpha], axis=0)
    return rgb, alpha, added


def composite_tower(frame: np.ndarray, sprite, offset: float, scale: float,
                    blur: float) -> np.ndarray:
    rgb, alpha, added = sprite
    centre_x = CLIP_W / 2 + 4
    matrix = np.float32([[scale, 0, centre_x - centre_x * scale],
                         [0, scale, offset - added * scale + (1 - scale) * 300]])
    layer = cv2.warpAffine(rgb * alpha[..., None], matrix, (CLIP_W, CLIP_H), flags=cv2.INTER_LINEAR)
    cover = cv2.warpAffine(alpha, matrix, (CLIP_W, CLIP_H), flags=cv2.INTER_LINEAR)
    length = int(round(blur))
    if length > 1:  # vertical motion blur along the fall
        kernel = np.full((length, 1), 1 / length, np.float32)
        layer = cv2.filter2D(layer, -1, kernel, anchor=(0, length - 1))
        cover = cv2.filter2D(cover, -1, kernel, anchor=(0, length - 1))
    # Lightning in the storm clip lights the tower too.
    gain = float(np.clip((frame[:400].mean() / 44.0) ** 0.5, 0.85, 1.6))
    out = frame.astype(np.float32) * (1 - cover[..., None]) + layer * gain
    return np.clip(out, 0, 255).astype(np.uint8)


def render(frame: np.ndarray, window: tuple) -> np.ndarray:
    left, top, w, h = window
    sx, sy = SIZE[0] / w, SIZE[1] / h
    matrix = np.float32([[sx, 0, -left * sx], [0, sy, -top * sy]])
    return cv2.warpAffine(frame, matrix, SIZE, flags=cv2.INTER_LANCZOS4, borderMode=cv2.BORDER_REPLICATE)


def sample(frames: list[np.ndarray], position: float) -> np.ndarray:
    position = min(max(position, 0.0), len(frames) - 1)
    low = int(position)
    high = min(low + 1, len(frames) - 1)
    weight = position - low
    if weight < 1e-3 or low == high:
        return frames[low]
    return np.rint(frames[low] * (1 - weight) + frames[high] * weight).astype(np.uint8)


def label_bar() -> np.ndarray:
    """Black strip (LABEL_BAR x width) with the AI disclosure mark at the right."""
    bar = Image.new("RGB", (OUT_SIZE[0], LABEL_BAR), (0, 0, 0))
    draw = ImageDraw.Draw(bar)
    font = ImageFont.truetype(str(AI_LABEL_FONT), AI_LABEL_SIZE)
    box = draw.textbbox((0, 0), AI_LABEL, font=font)
    x = OUT_SIZE[0] - (box[2] - box[0]) - 24 - box[0]
    y = (LABEL_BAR - (box[3] - box[1])) // 2 - box[1]
    draw.text((x, y), AI_LABEL, font=font, fill=(200, 200, 200))
    return np.asarray(bar)


def video_frames(segments):
    black = np.zeros((OUT_SIZE[1], OUT_SIZE[0], 3), dtype=np.uint8)
    bar = label_bar()
    sprite = tower_sprite()
    for segment in segments:
        if segment.shot is None:
            for _ in range(segment.frames):
                yield black
            continue
        name = segment.shot
        frames, rate = load_clip(clip_path(name))
        if name in LOCKS:
            frames = lock_regions(frames, LOCKS[name], name)
        total = segment.total_frames
        clip_seconds = len(frames) / rate
        speed = min(1.0, clip_seconds / (total / FPS))  # never speed up, only slow down
        fall_frames = round(FALL_SECONDS * FPS) if name == "op4" else 0
        previous_offset = None
        for index in range(total):
            source = sample(frames, index / FPS * speed * rate)
            fall_index = index - (total - fall_frames)
            if fall_frames and fall_index >= 0:
                t = (fall_index + 1) / fall_frames
                start, end = FALL_OFFSETS
                offset = start + (end - start) * t ** FALL_EASE
                scale = FALL_SCALE[0] + (FALL_SCALE[1] - FALL_SCALE[0]) * t
                step = offset - (previous_offset if previous_offset is not None
                                 else start + (end - start) * (fall_index / fall_frames) ** FALL_EASE)
                previous_offset = offset
                source = composite_tower(source, sprite, offset, scale, step * 0.8)
            picture = render(source, crop_window(name, index, total))
            pixels = np.concatenate([picture, bar], axis=0)
            if index < segment.fade_in_frames:
                opacity = index / segment.fade_in_frames
            elif index >= segment.frames:
                opacity = 1 - (index - segment.frames + 1) / segment.fade_out_frames
            else:
                opacity = 1.0
            yield pixels if opacity == 1 else np.rint(pixels * opacity).astype(np.uint8)
        print(f"{name}: {total / FPS:.2f}s, clip {clip_seconds:.2f}s, speed {speed:.2f}x")


def main() -> None:
    segments = base.make_segments()
    total_frames = sum(segment.total_frames for segment in segments)
    samples, voice_timings = base.soundtrack(segments, total_frames)
    for timing in voice_timings:
        print(f"Voice {timing.index}: {timing.start_time:.6f} - {timing.end_time:.6f} s")
    temporary = OUTPUT.with_name(".cg_ai.tmp.mp4")
    spf = base.SAMPLES_PER_FRAME
    try:
        with av.open(str(temporary), "w") as output:
            video = output.add_stream("libx264", rate=FPS, options={"crf": "18", "preset": "medium"})
            video.width, video.height = OUT_SIZE
            video.pix_fmt = "yuv420p"
            if samples is not None:
                audio = output.add_stream("aac", rate=base.SAMPLE_RATE)
                audio.layout = "stereo"
                audio.bit_rate = 192_000
            for index, pixels in enumerate(video_frames(segments)):
                frame = av.VideoFrame.from_ndarray(np.ascontiguousarray(pixels), format="rgb24")
                frame.pts = index
                frame.time_base = Fraction(1, FPS)
                for packet in video.encode(frame):
                    output.mux(packet)
                if samples is not None:
                    start = index * spf
                    chunk = np.ascontiguousarray(samples[:, start:start + spf])
                    audio_frame = av.AudioFrame.from_ndarray(chunk, format="fltp", layout="stereo")
                    audio_frame.sample_rate = base.SAMPLE_RATE
                    audio_frame.pts = start
                    audio_frame.time_base = Fraction(1, base.SAMPLE_RATE)
                    for packet in audio.encode(audio_frame):
                        output.mux(packet)
            for packet in video.encode(None):
                output.mux(packet)
            if samples is not None:
                for packet in audio.encode(None):
                    output.mux(packet)
        temporary.replace(OUTPUT)
    finally:
        temporary.unlink(missing_ok=True)
    print(f"Created {OUTPUT} ({total_frames / FPS:.3f} s)")


if __name__ == "__main__":
    main()
