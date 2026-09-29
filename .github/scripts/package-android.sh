#!/usr/bin/env bash
set -euo pipefail
[[ $# == 3 ]] || { echo 'Usage: package-android.sh <project> <ludork-artifact> <output>' >&2; exit 1; }
source "$(dirname "$0")/package-macos-common.sh"
require_environment AOS_ALIAS AOS_KEY_PASSWORD AOS_KEY_STORE_PASSWORD AOS_SIGNING_KEY
init_package android "$@"

export GRADLE_USER_HOME="$work/gradle-home"
mkdir "$GRADLE_USER_HOME"
printf 'org.gradle.caching=false\norg.gradle.configuration-cache=false\n' > "$GRADLE_USER_HOME/gradle.properties"
keystore="$work/signing.jks"
decode_secret AOS_SIGNING_KEY "$keystore"
printf '%s\n%s\n' "$AOS_KEY_STORE_PASSWORD" "$AOS_KEY_PASSWORD" | \
    sh "$work/tools/pack_android.sh" --dev --encrypt-data --compile-lua --use-ldpak \
        --sign --keystore "$keystore" --key-alias "$AOS_ALIAS" "$project" "$output"

apks=("$output"/*.apk)
[[ ${#apks[@]} == 1 && -s "${apks[0]}" && "${apks[0]}" == *-android-arm64-v8a-signed.apk ]]
python3 "$ci" validate-apk "${apks[0]}"
python3 "$ci" metadata "$project" "$output" android-arm64-v8a --artifact "${apks[0]}" --signed
