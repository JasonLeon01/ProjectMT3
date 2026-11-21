"""Expose the Emscripten target and sysroot to clang-tidy's Clang driver."""

import json
from pathlib import Path
import shlex
import subprocess
import sys


def main():
    build = Path(sys.argv[1])
    compiler = Path(sys.argv[2])
    output = build / "tidy-web"
    output.mkdir(exist_ok=True)
    flags = shlex.split(subprocess.check_output([str(compiler), "--cflags"], text=True))
    sysroot = next(flag for flag in flags if flag.startswith("--sysroot="))
    root = Path(sysroot.split("=", 1)[1])
    commands = json.loads((build / "compile_commands.json").read_text())
    result = []
    for entry in commands:
        if "_deps" in Path(entry["file"]).parts:
            continue
        # CMake escapes Windows paths in JSON; normalize separators before shell parsing.
        arguments = shlex.split(entry.pop("command").replace("\\", "/"))
        arguments[0] = "clang++" if entry["file"].endswith((".cpp", ".cxx")) else "clang"
        arguments[1:1] = ["--target=wasm32-unknown-emscripten", sysroot,
                          "-isystem", str(root / "include/c++/v1"),
                          "-isystem", str(root / "include/compat")]
        entry["arguments"] = arguments
        result.append(entry)
    (output / "compile_commands.json").write_text(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
