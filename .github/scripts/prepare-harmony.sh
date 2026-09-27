#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "$0")/package-macos-common.sh"
require_environment RUNNER_TEMP
[[ "${GITHUB_ACTIONS:-}" == true && "$(uname -s)" == Darwin && "$(uname -m)" == arm64 ]]
deveco_work="$RUNNER_TEMP/projectmt3-deveco-work"
studio="$deveco_work/DevEco-Studio.app"
[[ ! -L "$deveco_work" ]]

detach_deveco() {
    if [[ -f "$deveco_work/mounted" ]]; then
        hdiutil detach "$deveco_work/mount" || hdiutil detach -force "$deveco_work/mount" || return 1
        rm -f -- "$deveco_work/mounted"
    fi
}

validate_deveco() {
    local executable resource
    [[ -d "$studio" && ! -L "$studio" ]] || return 1
    for executable in jbr/Contents/Home/bin/java tools/node/bin/node tools/hvigor/bin/hvigorw \
        sdk/default/openharmony/native/llvm/bin/llvm-readobj; do
        test -x "$studio/Contents/$executable" || return 1
    done
    for resource in sdk/default/openharmony/native/oh-uni-package.json \
        sdk/default/openharmony/toolchains/lib/hap-sign-tool.jar \
        tools/hvigor/hvigor-ohos-plugin/node_modules/json5/package.json; do
        test -f "$studio/Contents/$resource" || return 1
    done
    "$studio/Contents/tools/node/bin/node" -e \
        'if (process.arch !== "arm64") { throw new Error("DevEco Mac ARM installer required"); }'
}

if [[ "${1:-}" == cleanup && $# == 1 ]]; then
    detach_deveco
    rm -rf -- "$deveco_work"
    exit 0
fi
[[ $# == 0 ]]
require_environment HOS_DEVECO_PACKAGE HOS_DEVECO_SHA256 GITHUB_ENV
[[ "$HOS_DEVECO_PACKAGE" =~ ^ghcr[.]io/[a-z0-9][a-z0-9-]*/[a-z0-9][a-z0-9._-]*$ &&
    "$HOS_DEVECO_SHA256" =~ ^[[:xdigit:]]{64}$ ]] || {
    echo 'Configure ghcr.io/owner/package (lowercase, no tag) and the complete installer SHA-256 (64 characters).' >&2
    exit 1
}
if [[ "${HOS_DEVECO_CACHE_HIT:-}" == true ]]; then
    if ! validate_deveco; then
        echo 'Restored DevEco Studio cache is invalid. Delete this DevEco Actions cache and retry.' >&2
        exit 1
    fi
    echo 'Using cached DevEco Studio and bundled SDK.'
    cmake --version
    echo "LUDORK_DEVECO_STUDIO=$studio" >> "$GITHUB_ENV"
    exit 0
fi
detach_deveco
rm -rf -- "$deveco_work"
umask 077
mkdir -p "$deveco_work/mount" "$deveco_work/unpacked"

prepare_exit() {
    local status=$?
    trap - EXIT
    detach_deveco || exit 1
    rm -rf -- "$deveco_work/installer" "$deveco_work/unpacked" "$deveco_work/registry.json"
    if (( status != 0 )); then rm -rf -- "$deveco_work"; fi
    exit "$status"
}
trap prepare_exit EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

require_environment GH_TOKEN GITHUB_ACTOR
command -v oras > /dev/null
printf '{}\n' > "$deveco_work/registry.json"
deveco_digest=$(printf '%s' "$HOS_DEVECO_SHA256" | tr '[:upper:]' '[:lower:]')
printf '%s' "$GH_TOKEN" | oras blob fetch \
    --username "$GITHUB_ACTOR" --password-stdin \
    --registry-config "$deveco_work/registry.json" \
    --output "$deveco_work/installer" "$HOS_DEVECO_PACKAGE@sha256:$deveco_digest"
rm "$deveco_work/registry.json"
python3 - "$deveco_work/installer" "$deveco_digest" <<'PY'
import hashlib
from pathlib import Path
import sys
with Path(sys.argv[1]).open('rb') as stream:
    actual = hashlib.file_digest(stream, 'sha256').hexdigest()
if actual != sys.argv[2]:
    raise SystemExit('DevEco installer SHA-256 mismatch; refusing to use it.')
print('DevEco installer SHA-256 verified.')
PY

find_payload() {
    python3 - "$deveco_work/unpacked" "$1" <<'PY'
from pathlib import Path
import sys
matches = list(Path(sys.argv[1]).rglob(sys.argv[2]))
if len(matches) > 1:
    raise SystemExit('DevEco installer contains multiple matching payloads.')
print(matches[0] if matches else '')
PY
}

install_image() {
    hdiutil attach -readonly -nobrowse -mountpoint "$deveco_work/mount" "$1"
    touch "$deveco_work/mounted"
    test -d "$deveco_work/mount/DevEco-Studio.app"
    ditto "$deveco_work/mount/DevEco-Studio.app" "$deveco_work/DevEco-Studio.app"
    detach_deveco
}

if python3 - "$deveco_work/installer" <<'PY'
import sys, zipfile
sys.exit(0 if zipfile.is_zipfile(sys.argv[1]) else 1)
PY
then
    ditto -x -k "$deveco_work/installer" "$deveco_work/unpacked"
    rm "$deveco_work/installer"
    app=$(find_payload DevEco-Studio.app)
    if [[ -n "$app" ]]; then
        mv "$app" "$deveco_work/DevEco-Studio.app"
    else
        image=$(find_payload '*.dmg')
        [[ -n "$image" ]] || { echo 'DevEco ZIP contains no app or DMG.' >&2; exit 1; }
        install_image "$image"
    fi
else
    install_image "$deveco_work/installer"
fi

validate_deveco
cmake --version
echo "LUDORK_DEVECO_STUDIO=$studio" >> "$GITHUB_ENV"
