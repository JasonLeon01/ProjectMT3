#!/usr/bin/env bash
# Shared by Android, macOS and iOS; writable paths are CI-local.
set -euo pipefail

require_environment() {
    local variable
    for variable in "$@"; do
        if [[ -z "${!variable:-}" ]]; then
            echo "Missing required environment variable: $variable" >&2
            return 1
        fi
    done
}

cleanup_package_work() {
    local platform=$1 work mountpoint keychain status=0 file marker
    [[ "${GITHUB_ACTIONS:-}" == true && -n "${RUNNER_TEMP:-}" ]]
    case "$platform" in android|macos|ios) ;; *) return 1 ;; esac
    work="$RUNNER_TEMP/projectmt3-$platform-work"
    [[ ! -L "$work" ]]
    mountpoint="$work/mount"
    # Also called by the workflow's always() step after shell interruption.
    if [[ -f "$work/mounted" ]]; then
        hdiutil detach "$mountpoint" || hdiutil detach -force "$mountpoint" || return 1
        rm -f -- "$work/mounted"
    fi
    if [[ -f "$work/keychain-search-list.json" ]]; then
        python3 "$GITHUB_WORKSPACE/.github/scripts/package-apple.py" restore-keychains "$work" || return 1
    fi
    shopt -s nullglob
    for keychain in "$work/ci.keychain-db" "$work"/tmp/ludork-signing-*/ludork-signing.keychain-db; do
        if [[ -f "$keychain" ]]; then
            security delete-keychain "$keychain" || status=1
        fi
    done
    for marker in installed-profile-path transporter-key-path; do
        if [[ -f "$work/$marker" ]]; then
            file=$(cat "$work/$marker")
            case "$file" in
                "$HOME/Library/MobileDevice/Provisioning Profiles/"*.mobileprovision|"$HOME/.appstoreconnect/private_keys/"AuthKey_*.p8)
                    rm -f -- "$file" || status=1 ;;
                *) echo "Unexpected signing cleanup path" >&2; status=1 ;;
            esac
        fi
    done
    rm -rf -- "$work"
    return "$status"
}

package_exit() {
    local status=$?
    trap - EXIT
    cleanup_package_work "$package_platform" || status=1
    exit "$status"
}

init_package() {
    package_platform=$1
    require_environment GITHUB_WORKSPACE RUNNER_TEMP PROJECT_SHA LUDORK_SHA LUDORK_CHECKED_SHA LUDORK_RUN_ID LUDORK_ARTIFACT_ID
    [[ "${GITHUB_ACTIONS:-}" == true && "$(uname -s)" == Darwin && "$(uname -m)" == arm64 ]]
    project=$(cd -- "$2" && pwd)
    artifact=$(cd -- "$3" && pwd)
    output=$4
    ci="$project/.github/scripts/package-ci.py"
    python3 "$ci" clean "$project" "$output"
    cleanup_package_work "$package_platform"
    work="$RUNNER_TEMP/projectmt3-$package_platform-work"
    mkdir -p "$work/mount" "$work/tmp"
    export TMPDIR="$work/tmp/"
    umask 077
    trap package_exit EXIT
    trap 'exit 130' INT
    trap 'exit 143' TERM
    shopt -s nullglob
    local images=("$artifact"/Ludork-*-macos-arm64.dmg)
    [[ ${#images[@]} == 1 && -s "${images[0]}" ]]
    hdiutil attach -readonly -nobrowse -mountpoint "$work/mount" "${images[0]}"
    touch "$work/mounted"
    ditto "$work/mount/Ludork.app/Contents/Resources/tools" "$work/tools"
    hdiutil detach "$work/mount"
    rm "$work/mounted"
    test -x "$work/tools/ScriptTools/ScriptTools"
    test -x "$work/tools/luac"
}

decode_secret() {
    local variable=$1 destination=$2
    require_environment "$variable"
    printf '%s' "${!variable}" | base64 --decode > "$destination"
    test -s "$destination"
}

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
    [[ $# == 2 && $1 == cleanup ]]
    cleanup_package_work "$2"
fi
