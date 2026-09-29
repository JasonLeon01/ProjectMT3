#!/usr/bin/env python3
"""CI-only cleanup, package inspection and provenance for every platform."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import stat
import struct
import subprocess
import tempfile
import zipfile
import zlib

GROUPS = ("Assets", "Data", "Scripts")
HEADER = struct.Struct("<4sHHIIQQII")
ENTRY = struct.Struct("<IIQQQII")
BLOCK = struct.Struct("<III")
BLOCK_SIZE = 1024 * 1024


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


def is_signing_material(name):
    path = Path(name)
    return path.suffix.lower() in {
        ".p12", ".pfx", ".pkcs12", ".jks", ".keystore", ".pem", ".key", ".cer",
        ".p7b", ".p8", ".mobileprovision",
    } or path.name.lower() == "harmony-signing.json"


def inspect_ldpak(path, group, reject_signing=False):
    # Read Ludork's block-compressed LDPK format; never unpack entries into the checkout.
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
            length, entry_flags, offset, length_data, stored_size, data_crc, block_count = ENTRY.unpack_from(index, position)
            position += ENTRY.size
            require(length > 0 and position + length <= len(index), f"Invalid entry name: {path}")
            name = index[position:position + length].decode("utf-8")
            position += length
            require(name not in names and not name.startswith("/") and "\\" not in name
                    and all(part not in ("", ".", "..") for part in name.split("/")), f"Invalid entry path: {name}")
            names.add(name)
            if reject_signing:
                require(not is_signing_material(name), f"Signing material remains in {group}: {name}")
            require(entry_flags in (0, 1), f"Invalid entry flags: {name}")
            if entry_flags == 1:
                require((offset, length_data, stored_size, data_crc, block_count) == (0, 0, 0, 0, 0),
                        f"Invalid directory: {name}")
                continue
            require(offset % 8 == 0 and offset >= HEADER.size + group_size
                    and offset + stored_size <= index_offset, f"Invalid data bounds: {name}")
            require(block_count == (length_data + BLOCK_SIZE - 1) // BLOCK_SIZE
                    and position + block_count * BLOCK.size <= len(index), f"Invalid block count: {name}")
            stream.seek(offset)
            consumed, actual_crc, signature = 0, 0, b""
            for block_index in range(block_count):
                block_size, block_flags, block_crc = BLOCK.unpack_from(index, position)
                position += BLOCK.size
                raw_size = min(BLOCK_SIZE, length_data - block_index * BLOCK_SIZE)
                require(block_flags in (0, 1) and 0 < block_size <= stored_size - consumed
                        and (block_size == raw_size if block_flags == 0 else block_size < raw_size),
                        f"Invalid block bounds: {name}")
                chunk = stream.read(block_size)
                require(len(chunk) == block_size, f"Truncated block: {name}")
                consumed += block_size
                if block_flags == 1:
                    inflater = zlib.decompressobj()
                    chunk = inflater.decompress(chunk, raw_size + 1)
                    require(len(chunk) == raw_size and inflater.eof
                            and not inflater.unused_data and not inflater.unconsumed_tail,
                            f"Invalid compressed block: {name}")
                require(zlib.crc32(chunk) & 0xffffffff == block_crc, f"Invalid block checksum: {name}")
                if not signature:
                    signature = chunk[:4]
                actual_crc = zlib.crc32(chunk, actual_crc)
            require(consumed == stored_size, f"Invalid stored size: {name}")
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


def inspect_zip(archive, label):
    names = archive.namelist()
    require(len(names) == len(set(names)), f"Duplicate {label} entries")
    for info in archive.infolist():
        name = info.filename
        require(name and not name.startswith("/") and "\\" not in name
                and all(part not in ("", ".", "..") for part in name.rstrip("/").split("/")),
                f"Invalid {label} path: {name}")
        require(not stat.S_ISLNK(info.external_attr >> 16), f"Symlink in {label}: {name}")
        require(not is_signing_material(name), f"Signing material in {label}: {name}")
    require(archive.testzip() is None, f"Corrupt {label} entry")
    return names


def validate_hap(hap, project):
    require(hap.is_file() and not hap.is_symlink(), "Signed HAP is missing or is a symlink")
    version = json.loads((project / "Main.proj").read_text(encoding="utf-8"))["packaging"]["version"]
    require(isinstance(version, str), "Invalid project package version")
    match = re.fullmatch(rf"ProjectMT3-{re.escape(version)}\.(20\d{{8}})-harmony-mobile-signed\.hap", hap.name)
    require(match is not None, "Expected one ProjectMT3 Mobile signed dev HAP")
    built_at = datetime.datetime.strptime(match[1], "%Y%m%d%H")
    with zipfile.ZipFile(hap) as archive, tempfile.TemporaryDirectory(dir=os.environ.get("RUNNER_TEMP")) as temporary:
        names = inspect_zip(archive, "HAP")
        require("module.json" in names, "HAP module.json is missing")
        manifest = json.loads(archive.read("module.json"))
        app, module = manifest.get("app", {}), manifest.get("module", {})
        require(app.get("bundleName") == "com.ludork.projectmt3.76adf92ced"
                and app.get("versionName") == version and app.get("versionCode") == int(built_at.strftime("%y%m%d%H")),
                "HAP identity or version does not match ProjectMT3")
        require(app.get("buildMode") == "release" and app.get("debug") is False, "HAP must use the Release build")
        require(module.get("name") == "entry" and module.get("type") == "entry"
                and module.get("mainElement") == "EntryAbility"
                and sorted(module.get("deviceTypes", [])) == ["phone", "tablet"], "Invalid Mobile HAP module")
        native = "libs/arm64-v8a/libentry.so"
        require(native in names, "HAP native entry is missing")
        with archive.open(native) as stream:
            header = stream.read(20)
        require(len(header) == 20 and header[:6] == b"\x7fELF\x02\x01"
                and int.from_bytes(header[18:20], "little") == 183, "HAP native entry is not arm64 ELF")
        runtime_name = "resources/rawfile/ludork-runtime.zip"
        hash_name = "resources/rawfile/ludork-runtime.sha256"
        require(runtime_name in names and hash_name in names, "HAP runtime archive or checksum is missing")
        require(archive.getinfo(runtime_name).compress_type == zipfile.ZIP_STORED, "HAP recompressed its runtime archive")
        digest = archive.read(hash_name).decode("ascii").strip()
        require(re.fullmatch(r"[0-9a-f]{64}", digest) is not None, "Invalid HAP runtime checksum")
        root = Path(temporary)
        runtime_path = root / "runtime.zip"
        with archive.open(runtime_name) as source, runtime_path.open("wb") as destination:
            shutil.copyfileobj(source, destination)
        with runtime_path.open("rb") as stream:
            require(hashlib.file_digest(stream, "sha256").hexdigest() == digest, "HAP runtime checksum mismatch")
        with zipfile.ZipFile(runtime_path) as runtime:
            runtime_names = inspect_zip(runtime, "HAP runtime")
            for group in GROUPS:
                name = f"{group}.ldpak"
                require(name in runtime_names and not any(n.rstrip("/") == group or n.startswith(f"{group}/") for n in runtime_names),
                        f"Invalid HAP runtime resource layout: {group}")
                require(runtime.getinfo(name).compress_type == zipfile.ZIP_STORED, f"HAP runtime recompressed {name}")
                target = root / name
                with runtime.open(name) as source, target.open("wb") as destination:
                    shutil.copyfileobj(source, destination)
                print(f"Validated {group}: {inspect_ldpak(target, group, reject_signing=True)} files")
    print("Validated Mobile HAP identity, runtime checksum and packed encrypted resources")


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
    hap_parser = sub.add_parser("validate-hap")
    hap_parser.add_argument("path", type=Path)
    hap_parser.add_argument("project", type=Path)
    args = parser.parse_args()
    if args.command == "clean":
        clean(args.project, args.output)
    elif args.command == "validate":
        validate(args.path)
    elif args.command == "validate-apk":
        validate_apk(args.path)
    elif args.command == "validate-hap":
        validate_hap(args.path, args.project)
    else:
        metadata(args.project, args.output, args.platform, args.artifact, args.signed, args.notarized)


if __name__ == "__main__":
    main()
