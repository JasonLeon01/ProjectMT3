#!/usr/bin/env bash
set -euo pipefail
[[ $# == 3 ]] || { echo 'Usage: package-harmony.sh <project> <ludork-artifact> <output>' >&2; exit 1; }
source "$(dirname "$0")/package-macos-common.sh"
require_environment HOS_ALIAS HOS_KEY_PASSWORD HOS_KEY_STORE_PASSWORD HOS_SIGNING_KEY HOS_SIGNING_CERTIFICATE HOS_PROVISIONING_PROFILE
init_package harmony "$@"

if [[ ! -f "$work/tools/pack_harmony.sh" ]] || \
    ! harmony_help=$("$work/tools/ScriptTools/ScriptTools" harmony-pack --help) || \
    ! grep -q -- '--certificate' <<< "$harmony_help" || \
    ! grep -q -- '--profile' <<< "$harmony_help"; then
    echo 'This Ludork artifact lacks non-interactive HOS signing. Publish a new Ludork tools artifact with harmony-pack --certificate and --profile support.' >&2
    exit 1
fi

keystore="$work/signing.p12"
certificate="$work/signing.cer"
profile="$work/signing.p7b"
decode_secret HOS_SIGNING_KEY "$keystore"
decode_secret HOS_SIGNING_CERTIFICATE "$certificate"
decode_secret HOS_PROVISIONING_PROFILE "$profile"
harmony_arguments=(--dev --encrypt-data --compile-lua --use-ldpak
    --sign --keystore "$keystore" --certificate "$certificate" --profile "$profile"
    --key-alias "$HOS_ALIAS" --device-form mobile --graphics-api opengl-es)
run_harmony_pack() {
    printf '%s\n%s\n' "$HOS_KEY_STORE_PASSWORD" "$HOS_KEY_PASSWORD" | \
        sh "$work/tools/pack_harmony.sh" "$@" "${harmony_arguments[@]}" "$project" "$output"
}
run_harmony_pack --check
run_harmony_pack

haps=("$output"/*.hap)
[[ ${#haps[@]} == 1 && -s "${haps[0]}" && "${haps[0]}" == *-harmony-mobile-signed.hap ]]
python3 "$ci" validate-hap "${haps[0]}" "$project"
python3 "$ci" metadata "$project" "$output" harmony-mobile-arm64-v8a --artifact "${haps[0]}" --signed
