#!/usr/bin/env bash
set -euo pipefail

if [[ $# != 3 ]]; then
    echo 'Usage: package-android.sh <project-directory> <ludork-artifact-directory> <output-directory>' >&2
    exit 1
fi
if [[ "$(uname -s)" != Darwin || "$(uname -m)" != arm64 ]]; then
    echo 'Ludork Android packaging requires Apple Silicon macOS.' >&2
    exit 1
fi
for variable in RUNNER_TEMP PROJECT_SHA LUDORK_SHA LUDORK_CHECKED_SHA LUDORK_RUN_ID LUDORK_ARTIFACT_ID \
    AOS_ALIAS AOS_KEY_PASSWORD AOS_KEY_STORE_PASSWORD AOS_SIGNING_KEY; do
    if [[ -z "${!variable:-}" ]]; then
        echo "Missing required environment variable: $variable" >&2
        exit 1
    fi
done

project=$(cd -- "$1" && pwd)
artifact=$(cd -- "$2" && pwd)
mkdir -p -- "$3"
output=$(cd -- "$3" && pwd)
shopt -s nullglob
images=("$artifact"/Ludork-*-macos-arm64.dmg)
if [[ ${#images[@]} != 1 || ! -s "${images[0]}" ]]; then
    echo 'Expected exactly one non-empty Ludork macOS ARM64 DMG.' >&2
    exit 1
fi

work=$(mktemp -d "$RUNNER_TEMP/projectmt3-android.XXXXXX")
mountpoint="$work/mount"
keystore="$RUNNER_TEMP/projectmt3-aos.jks"
mounted=false
cleanup() {
    local status=$?
    trap - EXIT
    rm -f -- "$keystore" || status=1
    if [[ "$mounted" == true ]]; then
        if ! hdiutil detach "$mountpoint"; then
            echo "Could not detach Ludork DMG at $mountpoint" >&2
            exit 1
        fi
    fi
    rm -rf -- "$work" || status=1
    exit "$status"
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

mkdir "$mountpoint"
mounted=true
hdiutil attach -readonly -nobrowse -mountpoint "$mountpoint" "${images[0]}"
# Only use the distributed tools; Engine and Application come from this checkout.
ditto "$mountpoint/Ludork.app/Contents/Resources/tools" "$work/tools"
hdiutil detach "$mountpoint"
mounted=false
for file in pack_android.sh common.sh ScriptTools/ScriptTools; do
    if [[ ! -f "$work/tools/$file" ]]; then
        echo "Ludork DMG is missing tools/$file" >&2
        exit 1
    fi
done
test -x "$work/tools/ScriptTools/ScriptTools"

umask 077
printf '%s' "$AOS_SIGNING_KEY" | base64 --decode > "$keystore"
test -s "$keystore"
# The packer reads store password, then key password, from standard input.
printf '%s\n%s\n' "$AOS_KEY_STORE_PASSWORD" "$AOS_KEY_PASSWORD" | \
    sh "$work/tools/pack_android.sh" --dev --sign --keystore "$keystore" --key-alias "$AOS_ALIAS" \
        "$project" "$output"

apks=("$output"/*.apk)
if [[ ${#apks[@]} != 1 || ! -s "${apks[0]}" || "${apks[0]}" != *-android-arm64-v8a-signed.apk ]]; then
    echo 'Expected exactly one non-empty signed Android ARM64 APK.' >&2
    exit 1
fi
python3 - "$project" "${apks[0]}" "$output/build-info.json" <<'PY'
import hashlib
import json
import os
import pathlib
import subprocess
import sys

project, apk, destination = map(pathlib.Path, sys.argv[1:])
with apk.open('rb') as source:
    digest = hashlib.file_digest(source, 'sha256').hexdigest()
metadata = {
    'project_commit': os.environ['PROJECT_SHA'],
    'engine_tree': subprocess.check_output(
        ['git', '-C', str(project), 'rev-parse', f"{os.environ['PROJECT_SHA']}:Engine"], text=True
    ).strip(),
    'ludork_commit': os.environ['LUDORK_SHA'],
    'ludork_checked_commit': os.environ['LUDORK_CHECKED_SHA'],
    'ludork_run_id': os.environ['LUDORK_RUN_ID'],
    'ludork_artifact_id': os.environ['LUDORK_ARTIFACT_ID'],
    'ludork_run_url': f"https://github.com/JasonLeon01/Ludork/actions/runs/{os.environ['LUDORK_RUN_ID']}",
    'configuration': 'Release',
    'platform': 'android-arm64-v8a',
    'signed': True,
    'apk': apk.name,
    'apk_sha256': digest,
}
destination.write_text(json.dumps(metadata, indent=2) + '\n', encoding='utf-8')
PY
