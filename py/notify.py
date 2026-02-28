import json
import threading
import urllib.request

_WEBHOOK_URL = "https://discord.com/api/webhooks/1477317205405335817/kcerkhwV5-DooRsyN1rhGulmDJ4hkM6G5yh-H0RfUE-YiGX77XVT8SXcdRaFPFFRg7Rj"
_MENTION = "<@152957206025863168>"


def notify(message: str):
    if not _WEBHOOK_URL:
        return

    def _send():
        try:
            data = json.dumps({"content": f"{_MENTION} {message}"}).encode()
            req = urllib.request.Request(
                _WEBHOOK_URL,
                data=data,
                headers={"Content-Type": "application/json"},
            )
            urllib.request.urlopen(req, timeout=5)
        except Exception:
            pass

    threading.Thread(target=_send, daemon=True).start()
