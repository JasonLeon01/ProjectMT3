#!/usr/bin/env python3
"""Bounded timeout retries for Apple services, without rebuilding packages."""
import argparse
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import time


ATTEMPTS = 3
TIMEOUT = re.compile(r"\b(?:timed?\s*out|timeout)\b|NSURLErrorDomain[^\n]*-1001|\b504\b", re.I)


def request(args, *, label, timeout, received=None, retryable=None):
    for attempt in range(1, ATTEMPTS + 1):
        print(f"{label}: attempt {attempt}/{ATTEMPTS}", flush=True)
        with subprocess.Popen(args, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT, start_new_session=True) as process:
            expired = False
            try:
                output, _ = process.communicate(timeout=timeout)
            except subprocess.TimeoutExpired:
                expired = True
                # Transporter spawns children; stop the whole attempt before retrying.
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                output, _ = process.communicate()
        text = output.decode(errors="replace")
        print(text, end="" if text.endswith("\n") else "\n", flush=True)
        timed_out = expired or (retryable(text) if retryable else process.returncode != 0 and TIMEOUT.search(text))
        if process.returncode == 0 and not timed_out:
            return text
        if not timed_out:
            raise RuntimeError(f"{label} failed (exit {process.returncode}); not a timeout")
        if received is not None and received(text):
            return text
        if attempt == ATTEMPTS:
            raise RuntimeError(f"{label} timed out after {ATTEMPTS} attempts")
        delay = 15 * attempt
        print(f"{label} timed out; retrying in {delay}s", flush=True)
        time.sleep(delay)


def report(text):
    start, end = text.find("{"), text.rfind("}")
    try:
        value = json.loads(text[start:end + 1])
    except (ValueError, TypeError):
        return {}
    return value if isinstance(value, dict) else {}


def notary_pending(text):
    status = report(text).get("status")
    if status in ("Accepted", "Invalid", "Rejected"):
        return False
    return status == "In Progress" or bool(TIMEOUT.search(text))


def notarize(app, work):
    archive = work / "notary.zip"
    subprocess.run(["ditto", "-c", "-k", "--norsrc", "--keepParent", str(app), str(archive)], check=True)
    credentials = ["--key", str(work / "notary.p8"), "--key-id", os.environ["APP_STORE_CONNECT_KEY_ID"],
                   "--issuer", os.environ["APP_STORE_CONNECT_ISSUER_ID"]]
    submitted = request(["xcrun", "notarytool", "submit", str(archive), *credentials, "--output-format", "json"],
                        label="Apple notarization upload", timeout=900,
                        received=lambda text: bool(report(text).get("id")))
    submission_id = report(submitted).get("id")
    if not submission_id:
        raise RuntimeError("Apple notarization did not return a submission ID")
    print(f"Continuing notarization submission {submission_id}", flush=True)
    # Wait retries always use the same submission; Apple's processing continues on timeout.
    try:
        result = request(["xcrun", "notarytool", "wait", submission_id, *credentials,
                          "--timeout", "15m", "--output-format", "json"],
                         label="Apple notarization processing", timeout=960,
                         retryable=notary_pending)
        if report(result).get("status") != "Accepted":
            raise RuntimeError(f"Apple rejected notarization submission {submission_id}")
    except RuntimeError:
        try:
            subprocess.run(["xcrun", "notarytool", "log", submission_id, *credentials], timeout=120, check=False)
        except (OSError, subprocess.TimeoutExpired) as error:
            print(f"Cannot retrieve notarization log: {error}", flush=True)
        raise
    request(["xcrun", "stapler", "staple", str(app)], label="Apple notarization ticket", timeout=300)
    archive.unlink()


def upload(ipa, work, output):
    helper = Path(__file__).with_name("testflight.rb")

    def received(_text):
        # An upload can succeed at Apple even if the client loses the response.
        result = subprocess.run(["ruby", str(helper), "uploaded", str(work / "ios-context.json"), str(output)], check=False)
        if result.returncode not in (0, 2):
            raise RuntimeError("Cannot check whether Apple received the timed-out upload")
        return result.returncode == 0

    request(["fastlane", "pilot", "upload", "--ipa", str(ipa), "--api_key_path", str(work / "api-key.json"),
             "--skip_submission", "true", "--skip_waiting_for_build_processing", "true"],
            label="TestFlight upload", timeout=1800, received=received)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    macos = sub.add_parser("notarize")
    macos.add_argument("app", type=Path)
    macos.add_argument("work", type=Path)
    ios = sub.add_parser("upload")
    ios.add_argument("ipa", type=Path)
    ios.add_argument("work", type=Path)
    ios.add_argument("output", type=Path)
    args = parser.parse_args()
    if args.command == "notarize":
        notarize(args.app, args.work)
    else:
        upload(args.ipa, args.work, args.output)


if __name__ == "__main__":
    main()
