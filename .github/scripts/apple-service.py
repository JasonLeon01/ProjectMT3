#!/usr/bin/env python3
"""Bounded transient-failure retries for Apple services, without rebuilding packages."""
import argparse
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import time


ATTEMPTS = 3
HTTP_STATUS = re.compile(
    r"\b(?:HTTP(?:/\d+(?:\.\d+)?)?(?:\s+(?:status|error|response))?(?:\s+code)?|"
    r"(?:response\s+)?status\s+code)\s*[:=]?\s*([45]\d{2})\b", re.I)
URL_ERROR = re.compile(r"\b(?:NSURLErrorDomain|kCFErrorDomainCFNetwork)\s+Code\s*=\s*(-\d+)\b", re.I)
TIMEOUT = re.compile(
    r"\b(?:timed?\s+out|(?:request|connection|read|socket|gateway)\s+timeout|"
    r"timeout (?:while|waiting|expired|error|occurred)|"
    r"(?:Socket|Connect|Read|Write)?Timeout(?:Error|Exception)|Net::(?:Open|Read|Write)Timeout)\b", re.I)
NETWORK = re.compile(
    r"\b(?:connection (?:was )?(?:lost|reset|refused|aborted)|broken pipe|"
    r"could not (?:connect to (?:the )?server|resolve host)|"
    r"temporary failure in name resolution|network is unreachable|"
    r"internet connection appears to be offline|SocketException|UnknownHostException)\b", re.I)
SERVICE = re.compile(r"\b(?:service (?:is |temporarily )?unavailable|internal server error|bad gateway|too many requests)\b", re.I)
PERMANENT = re.compile(
    r"\b(?:unauthorized|forbidden|authentication (?:failed|failure)|permission denied|"
    r"(?:invalid|incorrect) (?:credentials|password|username|API key)|"
    r"(?:invalid|expired|revoked|untrusted) (?:certificate|signature|ticket)|"
    r"(?:certificate|signature|ticket) (?:is |has been |has )?(?:invalid|expired|revoked|untrusted)|"
    r"certificate verify failed|SSL certificate problem|SSLHandshakeException|PKIX path building failed|"
    r"code object is not signed|"
    r"no such file or directory|(?:asset )?validation (?:failed|error)|"
    r"redundant binary upload|build number .+already (?:exists|been used)|"
    r"builds cannot be assigned|cannot add internal group)\b", re.I)


def failure_reason(text, expired=False):
    text = re.sub(r"\x1b\[[0-9;]*m", "", text)
    statuses = {int(code) for code in HTTP_STATUS.findall(text)}
    url_errors = {int(code) for code in URL_ERROR.findall(text)}
    if (report(text).get("status") in ("Invalid", "Rejected") or PERMANENT.search(text)
            or statuses - {408, 429, 500, 502, 503, 504}
            or url_errors - {-1001, -1003, -1004, -1005, -1006, -1009}):
        return "permanent failure"
    if expired or -1001 in url_errors or statuses & {408, 504} or TIMEOUT.search(text):
        return "timeout"
    if url_errors or NETWORK.search(text):
        return "network interruption"
    if statuses & {429, 500, 502, 503} or SERVICE.search(text):
        return "temporary service failure"
    return None


def request(args, *, label, timeout, received=None, completed=None, pending=None):
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
        # A submission ID is authoritative even when the client loses its response.
        if completed is not None and completed(text):
            return text
        is_pending = pending is not None and pending(text)
        if process.returncode == 0 and not expired and not is_pending:
            return text
        reason = failure_reason(text, expired)
        if is_pending and reason is None:
            reason = "processing still pending"
        detail = f"exit {process.returncode}, attempt {attempt}/{ATTEMPTS}"
        if reason is None or reason == "permanent failure":
            raise RuntimeError(f"{label} failed ({detail}); {reason or 'no transient failure identified'}")
        if received is not None and received():
            print(f"{label}: Apple already received the upload ({detail})", flush=True)
            return text
        if attempt == ATTEMPTS:
            raise RuntimeError(f"{label} failed after {ATTEMPTS} attempts: {reason} ({detail})")
        delay = 15 * attempt
        print(f"{label}: {reason} ({detail}); retrying in {delay}s", flush=True)
        time.sleep(delay)
        # BuildUpload can become visible during the backoff, before Build exists.
        if received is not None and received():
            print(f"{label}: Apple received the upload during backoff", flush=True)
            return text


def report(text):
    start, end = text.find("{"), text.rfind("}")
    try:
        value = json.loads(text[start:end + 1])
    except (ValueError, TypeError):
        return {}
    return value if isinstance(value, dict) else {}


def notarize(app, work):
    archive = work / "notary.zip"
    subprocess.run(["ditto", "-c", "-k", "--norsrc", "--keepParent", str(app), str(archive)], check=True)
    credentials = ["--key", str(work / "notary.p8"), "--key-id", os.environ["APP_STORE_CONNECT_KEY_ID"],
                   "--issuer", os.environ["APP_STORE_CONNECT_ISSUER_ID"]]
    submitted = request(["xcrun", "notarytool", "submit", str(archive), *credentials, "--output-format", "json"],
                        label="Apple notarization upload", timeout=900,
                        completed=lambda text: bool(report(text).get("id")))
    submission_id = report(submitted).get("id")
    if not submission_id:
        raise RuntimeError("Apple notarization did not return a submission ID")
    print(f"Continuing notarization submission {submission_id}", flush=True)
    # Wait retries always use the same submission; Apple's processing continues on timeout.
    try:
        result = submitted
        if report(result).get("status") not in ("Accepted", "Invalid", "Rejected"):
            result = request(["xcrun", "notarytool", "wait", submission_id, *credentials,
                              "--timeout", "15m", "--output-format", "json"],
                             label=f"Apple notarization processing {submission_id}", timeout=960,
                             completed=lambda text: report(text).get("status") == "Accepted",
                             pending=lambda text: report(text).get("status") == "In Progress")
        if report(result).get("status") != "Accepted":
            raise RuntimeError(f"Apple rejected notarization submission {submission_id}")
    except RuntimeError:
        try:
            subprocess.run(["xcrun", "notarytool", "log", submission_id, *credentials], timeout=120, check=False)
        except (OSError, subprocess.TimeoutExpired) as error:
            print(f"Cannot retrieve notarization log: {error}", flush=True)
        raise
    request(["xcrun", "stapler", "staple", str(app)],
            label=f"Apple notarization ticket {submission_id}", timeout=300)
    archive.unlink()


def validate(app):
    request(["xcrun", "stapler", "validate", str(app)],
            label=f"Apple notarization ticket validation {app}", timeout=300)
    request(["spctl", "--assess", "--type", "execute", "--verbose=2", str(app)],
            label=f"Gatekeeper assessment {app}", timeout=300)


def upload(ipa, work, output):
    helper = Path(__file__).with_name("testflight.rb")

    def received():
        # An upload can succeed at Apple even if the client loses the response.
        result = subprocess.run(["ruby", str(helper), "uploaded", str(work / "ios-context.json"), str(output)], check=False)
        if result.returncode not in (0, 2):
            raise RuntimeError(f"Cannot confirm whether Apple received the upload (query exit {result.returncode}); not re-uploading")
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
    validation = sub.add_parser("validate")
    validation.add_argument("app", type=Path)
    ios = sub.add_parser("upload")
    ios.add_argument("ipa", type=Path)
    ios.add_argument("work", type=Path)
    ios.add_argument("output", type=Path)
    args = parser.parse_args()
    if args.command == "notarize":
        notarize(args.app, args.work)
    elif args.command == "validate":
        validate(args.app)
    else:
        upload(args.ipa, args.work, args.output)


if __name__ == "__main__":
    main()
