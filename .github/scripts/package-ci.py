#!/usr/bin/env python3
"""CI-only cleanup, package inspection and provenance for every platform."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import zipfile
import zlib

GROUPS = ("Assets", "Data", "Scripts")
HEADER = struct.Struct("<4sHHIIQQII")
ENTRY = struct.Struct("<IIQQII")


def require(condition, message):
    if not condition:
        raise ValueError(message)


def clean(project, output):
    require(os.environ.get("GITHUB_ACTIONS") == "true", "Cleanup is only allowed on CI")
    project = project.resolve()
    require(not output.is_symlink(), "Refusing to clean an output symlink")
    output = output.resolve()
    require(project == Path(os.environ["GITHUB_WORKSPACE"]).resolve(), "Unexpected checkout")
    temporary = Path(os.environ["RUNNER_TEMP"]).resolve()
    require(output.is_relative_to(temporary) and output != temporary, "Output must be inside RUNNER_TEMP")
    for target in [*(project / name for name in ("build", "bin", "Intermediate", "Cache", "EditorCache")), output]:
        require(not target.is_symlink(), f"Refusing to clean a symlink: {target}")
        if target.exists():
            shutil.rmtree(target)
    output.mkdir(parents=True)


def inspect_ldpak(path, group):
    # Read the published LDPK v1 format; never unpack entries into the checkout.
    size = path.stat().st_size
    files = set()
    with path.open("rb") as stream:
        raw = stream.read(HEADER.size)
        require(len(raw) == HEADER.size, f"Truncated archive: {path}")
        magic, version, flags, group_size, count, index_offset, index_size, crc, reserved = HEADER.unpack(raw)
        require((magic, version, flags, reserved) == (b"LDPK", 1, 0, 0), f"Invalid LDPK header: {path}")
        require(group_size == len(group) and stream.read(group_size) == group.encode(), f"Wrong resource group: {path}")
        require(index_offset >= HEADER.size + group_size and index_offset % 8 == 0
                and index_offset + index_size == size, f"Invalid index bounds: {path}")
        stream.seek(index_offset)
        index = stream.read(index_size)
        require(zlib.crc32(index) & 0xffffffff == crc, f"Invalid index checksum: {path}")
        position = 0
        names = set()
        for _ in range(count):
            require(position + ENTRY.size <= len(index), f"Truncated index: {path}")
            length, entry_flags, offset, length_data, data_crc, entry_reserved = ENTRY.unpack_from(index, position)
            position += ENTRY.size
            require(length > 0 and position + length <= len(index), f"Invalid entry name: {path}")
            name = index[position:position + length].decode("utf-8")
            position += length
            require(name not in names and not name.startswith("/") and "\\" not in name
                    and all(part not in ("", ".", "..") for part in name.split("/")), f"Invalid entry path: {name}")
            names.add(name)
            require(entry_flags in (0, 1) and entry_reserved == 0, f"Invalid entry flags: {name}")
            if entry_flags == 1:
                require((offset, length_data, data_crc) == (0, 0, 0), f"Invalid directory: {name}")
                continue
            require(offset % 8 == 0 and offset >= HEADER.size + group_size
                    and offset + length_data <= index_offset, f"Invalid data bounds: {name}")
            stream.seek(offset)
            remaining, actual_crc, signature = length_data, 0, b""
            while remaining:
                chunk = stream.read(min(1024 * 1024, remaining))
                require(bool(chunk), f"Truncated entry: {name}")
                if not signature:
                    signature = chunk[:4]
                actual_crc = zlib.crc32(chunk, actual_crc)
                remaining -= len(chunk)
            require(actual_crc & 0xffffffff == data_crc, f"Invalid data checksum: {name}")
            lower = name.lower()
            require(not lower.endswith(".lua"), f"Lua source remains in {group}: {name}")
            if group == "Data":
                require(not lower.endswith(".json"), f"Unencrypted data remains: {name}")
                if lower.endswith(".ldc"):
                    require(signature == b"LDDC", f"Invalid encrypted data: {name}")
            if group == "Scripts" and lower.endswith(".luac"):
                require(signature == b"\x1bLua", f"Invalid compiled Lua: {name}")
            files.add(name)
        require(position == len(index), f"Trailing index bytes: {path}")
    if group == "Scripts":
        require("Entry.luac" in files, "Compiled Lua entry is missing")
    if group == "Data":
        require("BuildInfo.ldc" in files and "Configs/System.ldc" in files, "Encrypted game data is missing")
    return len(files)


def validate(root):
    for group in GROUPS:
        require(not (root / group).exists(), f"Loose resource group remains: {group}")
        path = root / f"{group}.ldpak"
        require(path.is_file() and not path.is_symlink(), f"Resource archive missing: {path}")
        print(f"Validated {group}: {inspect_ldpak(path, group)} files")


def validate_apk(apk):
    with zipfile.ZipFile(apk) as archive, tempfile.TemporaryDirectory(dir=os.environ.get("RUNNER_TEMP")) as temporary:
        names = archive.namelist()
        for group in GROUPS:
            name = f"assets/{group}.ldpak"
            require(names.count(name) == 1 and not any(n.startswith(f"assets/{group}/") for n in names),
                    f"Invalid APK resource layout: {group}")
            require(archive.getinfo(name).compress_type == zipfile.ZIP_STORED, f"APK recompressed {name}")
            target = Path(temporary) / f"{group}.ldpak"
            with archive.open(name) as source, target.open("wb") as destination:
                shutil.copyfileobj(source, destination)
        validate(Path(temporary))


def metadata(project, output, platform, artifact=None, signed=False, notarized=False):
    required = ("PROJECT_SHA", "LUDORK_SHA", "LUDORK_CHECKED_SHA", "LUDORK_RUN_ID", "LUDORK_ARTIFACT_ID")
    for name in required:
        require(bool(os.environ.get(name)), f"Missing metadata: {name}")
    data = {
        "project_commit": os.environ["PROJECT_SHA"],
        "engine_tree": subprocess.check_output(["git", "-C", str(project), "rev-parse", f"{os.environ['PROJECT_SHA']}:Engine"], text=True).strip(),
        "ludork_commit": os.environ["LUDORK_SHA"],
        "ludork_checked_commit": os.environ["LUDORK_CHECKED_SHA"],
        "ludork_run_id": os.environ["LUDORK_RUN_ID"],
        "ludork_artifact_id": os.environ["LUDORK_ARTIFACT_ID"],
        "ludork_run_url": f"https://github.com/JasonLeon01/Ludork/actions/runs/{os.environ['LUDORK_RUN_ID']}",
        "configuration": "Release", "channel": "dev", "platform": platform,
        "clean_build": True, "tools_cache_hit": False, "build_cache_hit": False,
        "encrypt_data": True, "compile_lua": True, "use_ldpak": True,
        "encrypt_shaders": False, "encrypt_saves": False,
        "signed": signed, "notarized": notarized,
        "workflow_run_id": os.environ.get("GITHUB_RUN_ID"),
        "workflow_run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT"),
    }
    if artifact:
        with artifact.open("rb") as stream:
            data.update(artifact=artifact.name, artifact_sha256=hashlib.file_digest(stream, "sha256").hexdigest())
    (output / "build-info.json").write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    for command in ("clean", "metadata"):
        item = sub.add_parser(command)
        item.add_argument("project", type=Path)
        item.add_argument("output", type=Path)
        if command == "metadata":
            item.add_argument("platform")
            item.add_argument("--artifact", type=Path)
            item.add_argument("--signed", action="store_true")
            item.add_argument("--notarized", action="store_true")
    for command in ("validate", "validate-apk"):
        sub.add_parser(command).add_argument("path", type=Path)
    args = parser.parse_args()
    if args.command == "clean":
        clean(args.project, args.output)
    elif args.command == "validate":
        validate(args.path)
    elif args.command == "validate-apk":
        validate_apk(args.path)
    else:
        metadata(args.project, args.output, args.platform, args.artifact, args.signed, args.notarized)


if __name__ == "__main__":
    main()
