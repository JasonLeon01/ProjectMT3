#!/usr/bin/env bash
set -euo pipefail
[[ $# == 3 ]] || { echo 'Usage: package-macos.sh <project> <ludork-artifact> <output>' >&2; exit 1; }
source "$(dirname "$0")/package-macos-common.sh"
require_environment APPLE_TEAM_ID MACOS_SIGNING_CERTIFICATE MACOS_SIGNING_CERTIFICATE_PASSWORD \
    APP_STORE_CONNECT_PRIVATE_KEY APP_STORE_CONNECT_KEY_ID APP_STORE_CONNECT_ISSUER_ID
init_package macos "$@"
decode_secret MACOS_SIGNING_CERTIFICATE "$work/signing.p12"
decode_secret APP_STORE_CONNECT_PRIVATE_KEY "$work/notary.p8"

# Explicit Developer ID identity prevents an iOS distribution p12 or ad-hoc fallback.
identity=$(python3 "$project/.github/scripts/package-apple.py" identity "$work/signing.p12" macos)
printf '%s\n' "$MACOS_SIGNING_CERTIFICATE_PASSWORD" | \
    sh "$work/tools/pack_project.sh" --dev --encrypt-data --compile-lua --use-ldpak \
        --ignore-environment --certificate "$work/signing.p12" --signing-identity "$identity" \
        --notarize --notary-key "$work/notary.p8" --notary-key-id "$APP_STORE_CONNECT_KEY_ID" \
        --notary-key-issuer "$APP_STORE_CONNECT_ISSUER_ID" "$project" "$work/dist"

apps=("$work/dist"/*.app)
[[ ${#apps[@]} == 1 ]]
app=${apps[0]}
python3 "$ci" validate "$app/Contents/Resources"
codesign --verify --deep --strict --verbose=2 "$app"
xcrun stapler validate "$app"
spctl --assess --type execute --verbose=2 "$app"
archive="$output/$(basename "$app" .app)-macos-arm64.zip"
ditto -c -k --sequesterRsrc --keepParent "$app" "$archive"
ditto -x -k "$archive" "$work/verify"
restored="$work/verify/$(basename "$app")"
codesign --verify --deep --strict --verbose=2 "$restored"
xcrun stapler validate "$restored"
spctl --assess --type execute --verbose=2 "$restored"
python3 "$ci" metadata "$project" "$output" macos-arm64 --artifact "$archive" --signed --notarized
