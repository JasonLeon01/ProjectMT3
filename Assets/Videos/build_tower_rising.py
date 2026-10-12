"""Run `python build_tower_rising.py` to create tower-rising.mp4 (requires av, numpy, opencv-python).

Source: the AI clip in tower-rising_assets/ (locked camera, anime-style redraw of cg_after.png):
debris falls off the tower, then the tower tears out of the ground and starts to rise.

Post:
  1. Crop  - fixed 4:3 window that also cuts off the clip-corner "AI生成" mark.
  2. Shake - violent camera shake while the debris falls, a jolt when the tower lifts off,
             then a low rumble while it rises.
  3. Timing- 24 -> 30 fps with frame blending, 1x speed.
  4. Fade  - fades to black shortly after the tower starts rising.
"""

from fractions import Fraction
from pathlib import Path

import av
import cv2
import numpy as np

DIRECTORY = Path(__file__).resolve().parent
SOURCE_PREFIX = "5c2087c5"  # dark weathered tower version (old light-tower clip: "d4849f29")
SOURCE = sorted((DIRECTORY / "tower-rising_assets").glob(f"{SOURCE_PREFIX}*.mp4"))[0]
OUTPUT = DIRECTORY / "tower-rising.mp4"
SIZE, FPS = (1440, 1080), 30

CROP_H = 650                 # crop height in clip px (width = CROP_H * 4 / 3)
CROP_CENTRE = (512, 348)     # keeps the bottom-right disclosure mark (y >= 690) out of frame

LIFT_AT = 2.4                # s, tower base leaves the ground in the AI clip
FADE = (2.9, 3.9)            # s, fade-to-black start / end
DURATION = 4.0               # s, output length

# Shake envelope (amplitude in clip px): violent -> jolt at lift-off -> rumble.
def shake_amplitude(t: float) -> float:
    if t < LIFT_AT:
        return 11.0 - 3.0 * (t / LIFT_AT)                     # 11 -> 8 while debris falls
    jolt = 9.0 * np.exp(-(t - LIFT_AT) / 0.18)                 # heavy kick when it tears free
    return 3.0 + jolt


def noise(t: float, seed: int) -> float:
    """Smooth pseudo-random signal in [-1, 1] made of a few incommensurate sines."""
    rng = np.random.default_rng(seed)
    freqs = rng.uniform(7.0, 17.0, 4)
    phases = rng.uniform(0, 2 * np.pi, 4)
    weights = np.array([1.0, 0.7, 0.5, 0.35])
    return float(np.sum(weights * np.sin(2 * np.pi * freqs * t + phases)) / weights.sum())


def load_frames(path: Path) -> tuple[list[np.ndarray], float]:
    with av.open(str(path)) as source:
        stream = source.streams.video[0]
        rate = float(stream.average_rate)
        frames = [f.to_ndarray(format="rgb24") for f in source.decode(stream)]
    return frames, rate


def sample(frames: list[np.ndarray], rate: float, t: float) -> np.ndarray:
    position = min(t * rate, len(frames) - 1)
    i = int(position)
    j = min(i + 1, len(frames) - 1)
    k = position - i
    if k < 1e-3 or i == j:
        return frames[i]
    return cv2.addWeighted(frames[i], 1 - k, frames[j], k, 0)


def render(frame: np.ndarray, t: float) -> np.ndarray:
    amplitude = shake_amplitude(t)
    dx = amplitude * noise(t, 1)
    dy = amplitude * noise(t, 2)
    angle = 0.035 * amplitude * noise(t, 3)  # degrees
    cx, cy = CROP_CENTRE[0] + dx, CROP_CENTRE[1] + dy
    scale = SIZE[1] / CROP_H
    matrix = cv2.getRotationMatrix2D((cx, cy), angle, scale)
    matrix[0, 2] += SIZE[0] / 2 - cx
    matrix[1, 2] += SIZE[1] / 2 - cy
    out = cv2.warpAffine(frame, matrix, SIZE, flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REFLECT)
    if t > FADE[0]:
        k = min((t - FADE[0]) / (FADE[1] - FADE[0]), 1.0)
        out = (out.astype(np.float32) * (1 - k * k * (3 - 2 * k))).astype(np.uint8)
    return out


def main() -> None:
    frames, rate = load_frames(SOURCE)
    with av.open(str(OUTPUT), "w") as container:
        stream = container.add_stream("libx264", rate=Fraction(FPS))
        stream.width, stream.height = SIZE
        stream.pix_fmt = "yuv420p"
        stream.options = {"crf": "16", "preset": "slow"}
        for n in range(round(DURATION * FPS)):
            t = n / FPS
            image = render(sample(frames, rate, t), t)
            for packet in stream.encode(av.VideoFrame.from_ndarray(image, format="rgb24")):
                container.mux(packet)
        for packet in stream.encode():
            container.mux(packet)
    print(f"Wrote {OUTPUT} ({DURATION}s, {SIZE[0]}x{SIZE[1]} @ {FPS}fps)")


if __name__ == "__main__":
    main()
