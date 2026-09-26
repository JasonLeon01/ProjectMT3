#!/usr/bin/env python3
"""Validate Apple signing inputs and adapt Ludork's device package for TestFlight."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import plistlib
import re
import secrets
import shlex
import subprocess


def require(condition, message):
    if not condition:
        raise ValueError(message)


def run(args, **kwargs):
    result = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
    if result.returncode:
        raise RuntimeError(f"{args[0]} failed: {result.stderr.decode(errors='replace').strip()}")
    return result.stdout


def certificate(path, platform):
    password = f"{platform.upper()}_SIGNING_CERTIFICATE_PASSWORD"
    command = ["openssl", "pkcs12", "-in", str(path), "-clcerts", "-nokeys", "-passin", f"env:{password}"]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Some exported p12 files use legacy RC2; OpenSSL 3 requires its legacy provider.
    if result.returncode and b"unsupported" in result.stderr.lower():
        result = subprocess.run(command + ["-legacy"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    require(result.returncode == 0, f"Cannot open {platform} p12; check its password and format")
    require(result.stdout.count(b"-----BEGIN CERTIFICATE-----") == 1, "Export a p12 containing exactly one signing identity")
    pem = result.stdout
    run(["openssl", "x509", "-checkend", "0", "-noout"], input=pem)
    subject = run(["openssl", "x509", "-subject", "-noout", "-nameopt", "RFC2253"], input=pem).decode()
    subject = re.sub(r"^subject\s*=\s*", "", subject.strip())
    team = os.environ["APPLE_TEAM_ID"]
    require(re.fullmatch(r"[A-Z0-9]{10}", team), "APPLE_TEAM_ID must have 10 uppercase letters/digits")
    expected = "Developer ID Application:" if platform == "macos" else "Apple Distribution:"
    require(f"CN={expected}" in subject and re.search(rf"(?:^|,)OU={re.escape(team)}(?:,|$)", subject.strip()),
            f"Expected a {expected} certificate for team {team}")
    der = run(["openssl", "x509", "-outform", "DER"], input=pem)
    return hashlib.sha1(der).hexdigest().upper(), der


def ios_context(project, work):
    identity, cert_der = certificate(work / "signing.p12", "ios")
    profile = plistlib.loads(run(["security", "cms", "-D", "-i", str(work / "profile.mobileprovision")]))
    team = os.environ["APPLE_TEAM_ID"]
    entitlements = profile.get("Entitlements", {})
    identifier = entitlements.get("application-identifier", "")
    require(profile.get("TeamIdentifier") == [team] and identifier.startswith(team + ".") and "*" not in identifier,
            "Profile must match this team and an explicit app identifier")
    require(not profile.get("ProvisionedDevices") and not profile.get("ProvisionsAllDevices")
            and entitlements.get("get-task-allow") is False and entitlements.get("beta-reports-active") is True,
            "Use an App Store Connect distribution profile, not development, ad-hoc or enterprise")
    expiration = profile.get("ExpirationDate")
    require(isinstance(expiration, datetime.datetime) and expiration.replace(tzinfo=datetime.timezone.utc) > datetime.datetime.now(datetime.timezone.utc),
            "Provisioning profile has expired")
    require(cert_der in profile.get("DeveloperCertificates", []), "p12 certificate is not included in the provisioning profile")
    uuid = profile.get("UUID", "")
    require(re.fullmatch(r"[A-Fa-f0-9-]{36}", uuid), "Invalid profile UUID")
    # Remember only our own installed profile/key for the workflow's always() cleanup.
    installed_profile = Path.home() / "Library/MobileDevice/Provisioning Profiles" / f"{uuid}.mobileprovision"
    require(not installed_profile.exists(), "Signing profile unexpectedly exists on this fresh runner")
    key_id = os.environ["APP_STORE_CONNECT_KEY_ID"]
    require(re.fullmatch(r"[A-Za-z0-9]+", key_id), "Invalid API key ID")
    transporter_key = Path.home() / ".appstoreconnect/private_keys" / f"AuthKey_{key_id}.p8"
    require(not transporter_key.exists(), "Transporter key unexpectedly exists on this fresh runner")
    (work / "installed-profile-path").write_text(str(installed_profile))
    (work / "transporter-key-path").write_text(str(transporter_key))
    non_exempt = os.environ["IOS_USES_NON_EXEMPT_ENCRYPTION"]
    require(non_exempt in ("true", "false"), "Set IOS_USES_NON_EXEMPT_ENCRYPTION to true or false after confirming compliance")
    code = os.environ.get("IOS_ENCRYPTION_EXPORT_COMPLIANCE_CODE", "").strip()
    require(non_exempt == "false" or bool(code), "Non-exempt encryption requires Apple's approved export compliance code for unattended distribution")
    metadata = work / "package-metadata.json"
    run([str(work / "tools/ScriptTools/ScriptTools"), "packaging-constants", "resolve-metadata", str(project), str(metadata), "--dev"])
    version = json.loads(metadata.read_text())["version"]
    context = {"bundle_id": identifier[len(team) + 1:], "version": version,
               "identity": identity, "uses_non_exempt_encryption": non_exempt == "true"}
    (work / "ios-context.json").write_text(json.dumps(context) + "\n")
    (work / "api-key.json").write_text(json.dumps({
        "key_id": os.environ["APP_STORE_CONNECT_KEY_ID"], "issuer_id": os.environ["APP_STORE_CONNECT_ISSUER_ID"],
        "key": (work / "api-key.p8").read_text(), "in_house": False,
    }) + "\n")


def prepare_keychain(work):
    keychain = work / "ci.keychain-db"
    marker = work / "keychain-search-list.json"
    require(not marker.exists(), "A signing keychain search list is already saved")
    previous = shlex.split(run(["security", "list-keychains", "-d", "user"]).decode())
    # Save before creating the keychain so cleanup can restore even a partial setup.
    marker.write_text(json.dumps(previous) + "\n")
    password = secrets.token_hex(24)
    run(["security", "create-keychain", "-p", password, str(keychain)])
    run(["security", "set-keychain-settings", "-lut", "21600", str(keychain)])
    run(["security", "unlock-keychain", "-p", password, str(keychain)])
    run(["security", "list-keychains", "-d", "user", "-s", str(keychain), *previous])
    run(["security", "import", str(work / "signing.p12"), "-P", os.environ["IOS_SIGNING_CERTIFICATE_PASSWORD"],
         "-k", str(keychain), "-T", "/usr/bin/codesign", "-T", "/usr/bin/security"])
    run(["security", "set-key-partition-list", "-S", "apple-tool:,apple:,codesign:",
         "-s", "-k", password, str(keychain)])
    identity = json.loads((work / "ios-context.json").read_text())["identity"]
    valid = run(["security", "find-identity", "-v", "-p", "codesigning", str(keychain)]).decode()
    if not re.search(rf"^\s*\d+\)\s+{re.escape(identity)}\b", valid, re.MULTILINE):
        matching = run(["security", "find-identity", "-p", "codesigning", str(keychain)]).decode()
        raise ValueError(f"Expected iOS signing identity is not valid in the temporary keychain:\n{matching}")
    print("Temporary iOS signing keychain is enabled and its signing identity is valid.")


def restore_keychains(work):
    marker = work / "keychain-search-list.json"
    if not marker.exists():
        return
    previous = json.loads(marker.read_text())
    require(isinstance(previous, list) and all(isinstance(path, str) for path in previous),
            "Invalid saved keychain search list")
    run(["security", "list-keychains", "-d", "user", "-s", *previous])
    marker.unlink()


def prepare_app(project, work, app):
    context = json.loads((work / "ios-context.json").read_text())
    info_path = app / "Info.plist"
    info = plistlib.loads(info_path.read_bytes())
    require(info.get("CFBundleIdentifier") == context["bundle_id"] and info.get("CFBundleShortVersionString") == context["version"],
            "Ludork app identifier/version does not match App Store Connect inputs")
    embedded = plistlib.loads(run(["security", "cms", "-D", "-i", str(app / "embedded.mobileprovision")]))
    original = plistlib.loads(run(["security", "cms", "-D", "-i", str(work / "profile.mobileprovision")]))
    require(embedded.get("UUID") == original.get("UUID"), "Built app uses an unexpected provisioning profile")
    entitlements = plistlib.loads(run(["codesign", "-d", "--entitlements", ":-", str(app)]))
    require(entitlements.get("application-identifier") == os.environ["APPLE_TEAM_ID"] + "." + context["bundle_id"]
            and entitlements.get("get-task-allow") is False and entitlements.get("beta-reports-active") is True,
            "Built app is not signed for App Store Connect")
    (work / "entitlements.plist").write_bytes(plistlib.dumps(entitlements))

    catalog = work / "Distribution.xcassets"
    iconset = catalog / "AppIcon.appiconset"
    iconset.mkdir(parents=True)
    (catalog / "Contents.json").write_text(json.dumps({"info": {"version": 1, "author": "xcode"}}))
    (iconset / "Contents.json").write_text(json.dumps({
        "images": [{"filename": "AppIcon.png", "idiom": "universal", "platform": "ios", "size": "1024x1024"}],
        "info": {"version": 1, "author": "xcode"},
    }))
    source = project / "Assets/System/icon.icns"
    if not source.is_file():
        source = project / "Assets/System/icon.png"
    run(["xcrun", "swift", str(project / ".github/scripts/ios-icon.swift"), str(source), str(iconset / "AppIcon.png")])
    partial = work / "icons.plist"
    run(["xcrun", "actool", str(catalog), "--compile", str(app), "--platform", "iphoneos",
         "--minimum-deployment-target", "15.0", "--target-device", "iphone", "--app-icon", "AppIcon",
         "--product-type", "com.apple.product-type.application", "--output-partial-info-plist", str(partial)])
    info.pop("CFBundleIconFiles", None)
    info.update(plistlib.loads(partial.read_bytes()))
    require(info.get("CFBundleIcons", {}).get("CFBundlePrimaryIcon", {}).get("CFBundleIconName") == "AppIcon",
            "actool did not generate distribution icon metadata")
    require((app / "Assets.car").is_file(), "Compiled icon asset catalog is missing")
    info["CFBundleVersion"] = context["build_number"]
    info["ITSAppUsesNonExemptEncryption"] = context["uses_non_exempt_encryption"]
    if context["uses_non_exempt_encryption"]:
        info["ITSEncryptionExportComplianceCode"] = os.environ["IOS_ENCRYPTION_EXPORT_COMPLIANCE_CODE"].strip()
    else:
        info.pop("ITSEncryptionExportComplianceCode", None)
    info_path.write_bytes(plistlib.dumps(info))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    cert = sub.add_parser("identity")
    cert.add_argument("path", type=Path)
    cert.add_argument("platform", choices=("macos", "ios"))
    for name in ("ios-context", "prepare-app"):
        item = sub.add_parser(name)
        item.add_argument("project", type=Path)
        item.add_argument("work", type=Path)
        if name == "prepare-app":
            item.add_argument("app", type=Path)
    for name in ("prepare-keychain", "restore-keychains"):
        sub.add_parser(name).add_argument("work", type=Path)
    args = parser.parse_args()
    if args.command == "identity":
        print(certificate(args.path, args.platform)[0])
    elif args.command == "ios-context":
        ios_context(args.project, args.work)
    elif args.command == "prepare-keychain":
        prepare_keychain(args.work)
    elif args.command == "restore-keychains":
        restore_keychains(args.work)
    else:
        prepare_app(args.project, args.work, args.app)


if __name__ == "__main__":
    main()
