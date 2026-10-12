"""Timeline and audio helpers for build_cg_ai.py (requires av and numpy).

Run build_cg_ai.py to create cg.mp4. Audio files are matched by name without
the extension, including optional BGM; see suno_bgm_prompt.md.
"""

from dataclasses import dataclass
from pathlib import Path
import math

import av
import numpy as np


# Opening black screen: music starts immediately, narration follows this hold.
INTRO_HOLD_SECONDS = 1.0  # Provisional; update the music cue sheet if changed.
BGM_FILENAME = "The Spire and the Unfinished Oath - CG BGM"
# Existing narration is quiet. These gains assume a mastered music export.
BGM_INTRO_GAIN = 0.18
BGM_NARRATION_GAIN = 0.035


# Playback order: (shot ID, duration/audio filename without extension, fade times).
# None is a black screen. Shot IDs select clips in build_cg_ai.py, not still images.
# Fade-in is included in the duration; fade-out is added afterward for a shot.
CFG = [
    (None, INTRO_HOLD_SECONDS, (0, 0)),
    ("op1", "File 01", (0.5, 0.4)),
    ("op2", "File 02", (0.4, 0.4)),
    ("op3", "File 03", (0.4, 0.4)),
    (None, "File 04", (0, 0)),
    ("op4", "File 05", (0, 0)),
    ("op5", "File 06", (0, 0.3)),
    ("op6", "File 07", (0.3, 0.4)),
    ("op7", "File 08", (0.4, 0.4)),
    ("op8", "File 09", (0.3, 0.3)),
    (None, "File 10", (0, 0)),
]

FPS = 30
SAMPLE_RATE = 48_000
SAMPLES_PER_FRAME = SAMPLE_RATE // FPS
SIZE = (1448, 1086)
DIRECTORY = Path(__file__).parent


@dataclass
class Segment:
    shot: str | None
    frames: int  # Configured time, including fade-in for a shot.
    fade_in_frames: int
    fade_out_frames: int
    audio: np.ndarray | None = None  # Planar stereo, 48 kHz, float32.

    @property
    def total_frames(self) -> int:
        return self.frames + (self.fade_out_frames if self.shot is not None else 0)


@dataclass
class VoiceTiming:
    index: int  # One-based narration index; silent segments and BGM do not count.
    start_time: float
    end_time: float


def effect_frames(seconds: float, label: str) -> int:
    if not isinstance(seconds, (int, float)) or not math.isfinite(seconds) or seconds < 0:
        raise ValueError(f"{label} must be a non-negative number of seconds: {seconds!r}")
    return round(seconds * FPS)


def pair(value: tuple, label: str) -> tuple:
    if not isinstance(value, tuple) or len(value) != 2:
        raise ValueError(f"{label} must be a two-item tuple: {value!r}")
    return value


def resolve_audio(filename: str) -> Path:
    requested = DIRECTORY / filename
    matches = sorted(path for path in requested.parent.iterdir()
                     if path.is_file() and path.stem == requested.stem)
    if not matches:
        raise FileNotFoundError(f"Audio not found: {requested.stem} (any extension)")
    if len(matches) > 1:
        raise ValueError(f"Multiple audio files match {requested.stem!r}: "
                         + ", ".join(path.name for path in matches))
    return matches[0]


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
    for shot, duration, fades in CFG:
        fade_in_seconds, fade_out_seconds = pair(fades, "Fade times")
        fade_in_frames = effect_frames(fade_in_seconds, "Fade-in")
        fade_out_frames = effect_frames(fade_out_seconds, "Fade-out")
        audio = None
        if isinstance(duration, str):
            path = resolve_audio(duration)
            audio = decode_audio(path)
            frames = math.ceil(audio.shape[1] / SAMPLES_PER_FRAME)
        elif isinstance(duration, (int, float)) and math.isfinite(duration) and duration > 0:
            frames = round(duration * FPS)
        else:
            raise ValueError(f"Invalid duration or audio filename: {duration!r}")
        if frames < 1:
            raise ValueError(f"Duration must be at least one frame: {duration!r}")
        if shot is not None and frames < fade_in_frames:
            raise ValueError(f"{shot}: fade-in exceeds the configured duration")
        segments.append(Segment(shot, frames, fade_in_frames, fade_out_frames, audio))
    return segments


def soundtrack(segments: list[Segment], total_frames: int
               ) -> tuple[np.ndarray | None, list[VoiceTiming]]:
    try:
        music_path = resolve_audio(BGM_FILENAME)
    except FileNotFoundError:
        music_path = None
    has_music = music_path is not None
    if not has_music:
        print(f"BGM not found: {BGM_FILENAME} (any extension); creating narration-only video")
    if not has_music and not any(segment.audio is not None for segment in segments):
        return None, []
    samples = np.zeros((2, total_frames * SAMPLES_PER_FRAME), dtype=np.float32)
    voice_timings = []
    start = 0
    for segment in segments:
        if segment.audio is not None:
            end = start + segment.audio.shape[1]
            samples[:, start:end] = segment.audio
            voice_timings.append(VoiceTiming(len(voice_timings) + 1,
                                            start / SAMPLE_RATE, end / SAMPLE_RATE))
        start += segment.total_frames * SAMPLES_PER_FRAME
    if has_music:
        music = decode_audio(music_path)
        # Short music ends naturally; long music is trimmed to the video length.
        length = min(samples.shape[1], music.shape[1])
        gain = np.full(length, BGM_NARRATION_GAIN, dtype=np.float32)
        intro_end = min(segments[0].total_frames * SAMPLES_PER_FRAME, length)
        transition = min(round(0.25 * SAMPLE_RATE), intro_end)
        gain[:intro_end] = BGM_INTRO_GAIN
        if transition:
            gain[intro_end - transition:intro_end] = np.linspace(
                BGM_INTRO_GAIN, BGM_NARRATION_GAIN, transition, dtype=np.float32)
        fade_samples = min(round(0.5 * SAMPLE_RATE), length)
        gain[-fade_samples:] *= np.linspace(1, 0, fade_samples, dtype=np.float32)
        samples[:, :length] += music[:, :length] * gain
        peak = float(np.max(np.abs(samples)))
        if peak > 0.99:
            samples *= 0.99 / peak
    return samples, voice_timings
