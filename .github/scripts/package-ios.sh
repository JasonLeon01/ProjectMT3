#!/usr/bin/env bash
set -euo pipefail
[[ $# == 3 ]] || { echo 'Usage: package-ios.sh <project> <ludork-artifact> <output>' >&2; exit 1; }
source "$(dirname "$0")/package-macos-common.sh"
require_environment APPLE_TEAM_ID IOS_SIGNING_CERTIFICATE IOS_SIGNING_CERTIFICATE_PASSWORD IOS_PROVISIONING_PROFILE \
    APP_STORE_CONNECT_PRIVATE_KEY APP_STORE_CONNECT_KEY_ID APP_STORE_CONNECT_ISSUER_ID \
    IOS_TESTFLIGHT_GROUP IOS_USES_NON_EXEMPT_ENCRYPTION
init_package ios "$@"
apple="$project/.github/scripts/package-apple.py"
testflight="$project/.github/scripts/testflight.rb"
export CLANG_MODULE_CACHE_PATH="$work/module-cache"
export ASC_KEY_PATH="$work/api-key.p8"
decode_secret IOS_SIGNING_CERTIFICATE "$work/signing.p12"
decode_secret IOS_PROVISIONING_PROFILE "$work/profile.mobileprovision"
decode_secret APP_STORE_CONNECT_PRIVATE_KEY "$ASC_KEY_PATH"
python3 "$ci" metadata "$project" "$output" ios-arm64
python3 "$apple" ios-context "$project" "$work"
context="$work/ios-context.json"
identity=$(python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["identity"])' "$context")

# Also checks the packer's derived Bundle ID against the supplied profile.
printf '%s\n' "$IOS_SIGNING_CERTIFICATE_PASSWORD" | \
    sh "$work/tools/pack_ios.sh" --check --dev --encrypt-data --compile-lua --use-ldpak \
        --ignore-environment --team-id "$APPLE_TEAM_ID" --certificate "$work/signing.p12" \
        --provisioning-profile "$work/profile.mobileprovision" --signing-identity "$identity" "$project" "$work/packed"
ruby "$testflight" prepare "$context" "$output"

ios_exit() {
    local status=$?
    trap - EXIT
    if (( status != 0 )); then
        ruby "$testflight" failed "$context" "$output" || true
    fi
    cleanup_package_work ios || status=1
    exit "$status"
}
trap ios_exit EXIT

printf '%s\n' "$IOS_SIGNING_CERTIFICATE_PASSWORD" | \
    sh "$work/tools/pack_ios.sh" --dev --encrypt-data --compile-lua --use-ldpak \
        --ignore-environment --team-id "$APPLE_TEAM_ID" --certificate "$work/signing.p12" \
        --provisioning-profile "$work/profile.mobileprovision" --signing-identity "$identity" "$project" "$work/packed"
ipas=("$work/packed"/*.ipa)
[[ ${#ipas[@]} == 1 && -s "${ipas[0]}" ]]
ditto -x -k "${ipas[0]}" "$work/distribution"
rm -f -- "${ipas[0]}"
apps=("$work/distribution/Payload"/*.app)
[[ ${#apps[@]} == 1 ]]
app=${apps[0]}
python3 "$ci" validate "$app"
python3 "$apple" prepare-app "$project" "$work" "$app"

# Ludork removes its signing keychain when the packer exits; import into a new,
# explicitly addressed temporary keychain for the modified distribution bundle.
keychain="$work/ci.keychain-db"
keychain_password=$(openssl rand -hex 24)
security create-keychain -p "$keychain_password" "$keychain"
security set-keychain-settings -lut 21600 "$keychain"
security unlock-keychain -p "$keychain_password" "$keychain"
security import "$work/signing.p12" -P "$IOS_SIGNING_CERTIFICATE_PASSWORD" -k "$keychain" -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k "$keychain_password" "$keychain" >/dev/null
codesign --force --sign "$identity" --keychain "$keychain" --generate-entitlement-der \
    --entitlements "$work/entitlements.plist" "$app"
codesign --verify --deep --strict --verbose=2 "$app"
plutil -lint "$app/Info.plist"
ipa="$work/ProjectMT3-testflight.ipa"
ditto -c -k --norsrc --keepParent "$work/distribution/Payload" "$ipa"
python3 "$ci" metadata "$project" "$output" ios-arm64 --artifact "$ipa" --signed

# Only upload here; the API helper checks Apple processing and internal group
# membership explicitly. Never ask pilot to submit an external beta review.
fastlane pilot upload --ipa "$ipa" --api_key_path "$work/api-key.json" \
    --skip_submission true --skip_waiting_for_build_processing true
rm -f -- "$ipa"
ruby "$testflight" wait "$context" "$output"
