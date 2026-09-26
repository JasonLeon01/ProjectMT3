#!/usr/bin/env python3
"""Select the newest installed stable Xcode 26 with an iPhoneOS 26 SDK."""
import os
from pathlib import Path
import re
import subprocess

candidates = []
for application in Path("/Applications").glob("Xcode*.app"):
    if "beta" in application.name.lower():
        continue
    developer = application / "Contents/Developer"
    environment = dict(os.environ, DEVELOPER_DIR=str(developer))
    result = subprocess.run(["xcodebuild", "-version"], env=environment, capture_output=True, text=True)
    match = re.search(r"^Xcode (26(?:\.\d+)*)$", result.stdout, re.MULTILINE)
    if result.returncode or not match:
        continue
    sdk = subprocess.run(["xcrun", "--sdk", "iphoneos", "--show-sdk-version"], env=environment, capture_output=True, text=True)
    if sdk.returncode == 0 and sdk.stdout.strip().split(".")[0] == "26":
        candidates.append((tuple(int(part) for part in match[1].split(".")), developer))
if not candidates:
    raise SystemExit("No stable Xcode 26 with iPhoneOS 26 SDK is installed; update the runner/toolchain selection.")
_, selected = max(candidates)
subprocess.run(["sudo", "xcode-select", "--switch", str(selected)], check=True)
with open(os.environ["GITHUB_ENV"], "a") as destination:
    destination.write(f"DEVELOPER_DIR={selected}\n")
print(f"Selected {selected}")
