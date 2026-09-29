"""Run `python build_cg.py` to create cg.mp4 (requires av, numpy, Pillow)."""

from dataclasses import dataclass
from fractions import Fraction
from pathlib import Path
import math

import av
import numpy as np
from PIL import Image, ImageOps


# List order is playback order. Each entry contains an image name and settings:
# duration/audio filename, (fade-in, fade-out seconds), (first, last scale),
# (first, last x offset), and (first, last y offset).
# Offsets are output pixels: +x right, +y down.
# Fade-in is included in the duration; fade-out is added afterward. Scale and
# offsets change linearly until fade-out, then stay at their last values.
# Most shots pan horizontally; op4 stays still and op5 zooms in place.
# "---" is pure black for exactly its configured duration.
CFG = [
    ("op1.png", (10, (0.5, 0.4), (1.08, 1.08), (45, -45), (0, 0))),
    ("op2.png", (10, (0.4, 0.4), (1.08, 1.08), (-35, 35), (0, 0))),
    ("op3.png", (10, (0.4, 0.4), (1.05, 1.05), (30, -30), (0, 0))),
    ("---", (5, (0, 0), (1.0, 1.0), (0, 0), (0, 0))),
    ("op4.png", (5, (0, 0), (1.0, 1.0), (0, 0), (0, 0))),
    ("op5.png", (5, (0, 0.3), (1.2, 1.4), (0, 0), (0, 0))),
    ("op6.png", (5, (0.3, 0.4), (1.08, 1.08), (40, -40), (0, 0))),
    ("op7.png", (10, (0.4, 0.4), (1.08, 1.08), (-40, 40), (0, 0))),
    ("op8.png", (5, (0.3, 0.3), (1.14, 1.14), (75, -75), (0, 0))),
    ("---", (5, (0, 0), (1.0, 1.0), (0, 0), (0, 0))),
]

FPS = 30
SAMPLE_RATE = 48_000
SAMPLES_PER_FRAME = SAMPLE_RATE // FPS
SIZE = (1448, 1086)
DIRECTORY = Path(__file__).parent
OUTPUT = DIRECTORY / "cg.mp4"


@dataclass
class Segment:
    image: Path | None
    frames: int  # Configured time, including fade-in for an image.
    fade_in_frames: int
    fade_out_frames: int
    first_scale: float
    last_scale: float
    x_offset_in: float
    x_offset_out: float
    y_offset_in: float
    y_offset_out: float
    audio: np.ndarray | None = None  # Planar stereo, 48 kHz, float32.

    @property
    def total_frames(self) -> int:
        return self.frames + (self.fade_out_frames if self.image else 0)


def effect_frames(seconds: float, label: str) -> int:
    if not isinstance(seconds, (int, float)) or not math.isfinite(seconds) or seconds < 0:
        raise ValueError(f"{label} must be a non-negative number of seconds: {seconds!r}")
    return round(seconds * FPS)


def image_scale(value: float, label: str) -> float:
    if not isinstance(value, (int, float)) or not math.isfinite(value) or value <= 0:
        raise ValueError(f"{label} must be a positive scale: {value!r}")
    return float(value)


def pair(value: tuple, label: str) -> tuple:
    if not isinstance(value, tuple) or len(value) != 2:
        raise ValueError(f"{label} must be a two-item tuple: {value!r}")
    return value


def image_offset(value: float, label: str) -> float:
    if not isinstance(value, (int, float)) or not math.isfinite(value):
        raise ValueError(f"{label} must be a finite pixel offset: {value!r}")
    return float(value)


def lerp(first: float, last: float, progress: float) -> float:
    return first + (last - first) * progress


def decode_audio(path: Path) -> np.ndarray:
    chunks = []
    resampler = av.AudioResampler(format="fltp", layout="stereo", rate=SAMPLE_RATE)
    with av.open(str(path)) as source:
        stream = next((item for item in source.streams if item.type == "audio"), None)
        if stream is None:
            raise ValueError(f"No audio stream: {path}")
        for frame in source.decode(stream):
            chunks.extend(part.to_ndarray() for part in resampler.resample(frame))
        chunks.extend(part.to_ndarray() for part in resampler.resample(None))
    if not chunks:
        raise ValueError(f"Empty audio: {path}")
    return np.concatenate(chunks, axis=1)


def make_segments() -> list[Segment]:
    if not CFG:
        raise ValueError("CFG must contain at least one segment")
    segments = []
    for filename, value in CFG:
        if not isinstance(value, tuple) or len(value) != 5:
            raise ValueError(f"{filename}: expected (duration, fades, scales, x offsets, y offsets)")
        duration, fades, scales, x_offsets, y_offsets = value
        fade_in_seconds, fade_out_seconds = pair(fades, "Fade times")
        first_scale, last_scale = pair(scales, "Scales")
        x_offset_in, x_offset_out = pair(x_offsets, "X offsets")
        y_offset_in, y_offset_out = pair(y_offsets, "Y offsets")
        fade_in_frames = effect_frames(fade_in_seconds, "Fade-in")
        fade_out_frames = effect_frames(fade_out_seconds, "Fade-out")
        first_scale = image_scale(first_scale, "First-frame scale")
        last_scale = image_scale(last_scale, "Pre-fade-out scale")
        x_offset_in = image_offset(x_offset_in, "First-frame x offset")
        x_offset_out = image_offset(x_offset_out, "Pre-fade-out x offset")
        y_offset_in = image_offset(y_offset_in, "First-frame y offset")
        y_offset_out = image_offset(y_offset_out, "Pre-fade-out y offset")
        audio = None
        if isinstance(duration, str):
            path = DIRECTORY / duration
            if not path.is_file():
                raise FileNotFoundError(path)
            audio = decode_audio(path)
            frames = math.ceil(audio.shape[1] / SAMPLES_PER_FRAME)
        elif isinstance(duration, (int, float)) and math.isfinite(duration) and duration > 0:
            frames = round(duration * FPS)
        else:
            raise ValueError(f"Invalid duration or audio filename: {duration!r}")
        if frames < 1:
            raise ValueError(f"Duration must be at least one frame: {duration!r}")
        image = None if filename == "---" else DIRECTORY / filename
        if image is not None:
            if not image.is_file():
                raise FileNotFoundError(image)
            if frames < fade_in_frames:
                raise ValueError(f"{filename}: fade-in exceeds the configured duration")
        segments.append(Segment(image, frames, fade_in_frames, fade_out_frames,
                                first_scale, last_scale, x_offset_in, x_offset_out,
                                y_offset_in, y_offset_out, audio))
    return segments


def transformed_pixels(image: Image.Image, scale: float, x_offset: float,
                       y_offset: float) -> np.ndarray:
    if scale == 1 and x_offset == 0 and y_offset == 0:
        return np.asarray(image)
    inverse_scale = 1 / scale
    source_x = SIZE[0] / 2 - (SIZE[0] / 2 + x_offset) * inverse_scale
    source_y = SIZE[1] / 2 - (SIZE[1] / 2 + y_offset) * inverse_scale
    return np.asarray(image.transform(
        SIZE, Image.Transform.AFFINE,
        (inverse_scale, 0, source_x, 0, inverse_scale, source_y),
        resample=Image.Resampling.BICUBIC, fillcolor=(0, 0, 0),
    ))


def video_frames(segments: list[Segment]):
    black = np.zeros((SIZE[1], SIZE[0], 3), dtype=np.uint8)
    for segment in segments:
        if segment.image is None:
            for _ in range(segment.frames):
                yield black
            continue
        with Image.open(segment.image) as source:
            image = Image.new("RGB", SIZE)
            fitted = ImageOps.contain(source.convert("RGB"), SIZE)
            image.paste(fitted, ((SIZE[0] - fitted.width) // 2,
                                 (SIZE[1] - fitted.height) // 2))
        previous_transform = None
        for index in range(segment.frames):
            progress = index / (segment.frames - 1) if segment.frames > 1 else 0
            transform = (
                lerp(segment.first_scale, segment.last_scale, progress),
                lerp(segment.x_offset_in, segment.x_offset_out, progress),
                lerp(segment.y_offset_in, segment.y_offset_out, progress),
            )
            if transform != previous_transform:
                pixels = transformed_pixels(image, *transform)
                previous_transform = transform
            opacity = (index / segment.fade_in_frames
                       if index < segment.fade_in_frames else 1)
            yield pixels if opacity == 1 else np.rint(pixels * opacity).astype(np.uint8)
        for index in range(segment.fade_out_frames):
            opacity = 1 - (index + 1) / segment.fade_out_frames
            yield pixels if opacity == 1 else np.rint(pixels * opacity).astype(np.uint8)


def soundtrack(segments: list[Segment], total_frames: int) -> np.ndarray | None:
    if not any(segment.audio is not None for segment in segments):
        return None
    samples = np.zeros((2, total_frames * SAMPLES_PER_FRAME), dtype=np.float32)
    start = 0
    for segment in segments:
        if segment.audio is not None:
            end = start + segment.audio.shape[1]
            samples[:, start:end] = segment.audio
        start += segment.total_frames * SAMPLES_PER_FRAME
    return samples


def main() -> None:
    segments = make_segments()
    total_frames = sum(segment.total_frames for segment in segments)
    samples = soundtrack(segments, total_frames)
    temporary = OUTPUT.with_name(".cg.tmp.mp4")
    try:
        with av.open(str(temporary), "w") as output:
            video = output.add_stream("libx264", rate=FPS,
                                      options={"crf": "18", "preset": "medium"})
            video.width, video.height = SIZE
            video.pix_fmt = "yuv420p"
            if samples is not None:
                audio = output.add_stream("aac", rate=SAMPLE_RATE)
                audio.layout = "stereo"
                audio.bit_rate = 192_000
            for index, pixels in enumerate(video_frames(segments)):
                frame = av.VideoFrame.from_ndarray(pixels, format="rgb24")
                frame.pts = index
                frame.time_base = Fraction(1, FPS)
                for packet in video.encode(frame):
                    output.mux(packet)
                if samples is not None:
                    start = index * SAMPLES_PER_FRAME
                    chunk = np.ascontiguousarray(samples[:, start:start + SAMPLES_PER_FRAME])
                    audio_frame = av.AudioFrame.from_ndarray(chunk, format="fltp", layout="stereo")
                    audio_frame.sample_rate = SAMPLE_RATE
                    audio_frame.pts = start
                    audio_frame.time_base = Fraction(1, SAMPLE_RATE)
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
