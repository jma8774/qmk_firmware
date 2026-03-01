import json
import os
import threading
import urllib.request
import time
from pathlib import Path

_ENV_PATH = Path(__file__).parent / ".env"
if _ENV_PATH.exists():
    for line in _ENV_PATH.read_text().splitlines():
        line = line.strip()
        if line and not line.startswith("#") and "=" in line:
            key, _, value = line.partition("=")
            os.environ.setdefault(key.strip(), value.strip())

_WEBHOOK_URL = os.environ.get("DISCORD_WEBHOOK_URL", "")
_MENTION = "<@152957206025863168>"


def notify(message: str, numberOfSends: int = 1):
    if not _WEBHOOK_URL:
        return

    def _send():
        for _ in range(numberOfSends):
            try:
                data = json.dumps({"content": f"{_MENTION} {message}"}).encode()
                req = urllib.request.Request(
                    _WEBHOOK_URL,
                    data=data,
                    headers={
                        "Content-Type": "application/json",
                        "User-Agent": "QMK-Notify/1.0",
                    },
                )
                urllib.request.urlopen(req, timeout=5)
            except Exception:
                pass
            time.sleep(0.5)


    t = threading.Thread(target=_send, daemon=True)
    t.start()
    t.join(timeout=10)
