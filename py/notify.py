import json
import threading
import urllib.request
import time

_WEBHOOK_URL = "https://discord.com/api/webhooks/1477317205405335817/kcerkhwV5-DooRsyN1rhGulmDJ4hkM6G5yh-H0RfUE-YiGX77XVT8SXcdRaFPFFRg7Rj"
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
