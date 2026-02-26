import time
import winsound
from pathlib import Path

from common import grab_region, match_template, load_template
from timing import request_stop

MINIMAP_REGION = {"top": 0, "left": 0, "width": 350, "height": 350}
_carcion_tmpl = load_template("carcion.png")
_last_check = 0.0

_ALERT_PATH = Path(__file__).parent / "sounds" / "alert_loud.wav"


def _play_alert():
    if _ALERT_PATH.exists():
        winsound.PlaySound(str(_ALERT_PATH), winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_LOOP)


def stop_alert():
    winsound.PlaySound(None, winsound.SND_PURGE)


def map_check():
    global _last_check
    now = time.monotonic()
    if now - _last_check < 1.0:
        return
    _last_check = now
    frame = grab_region(MINIMAP_REGION)
    if match_template(frame, _carcion_tmpl) is None:
        print("[map_check] carcion not found on minimap — pausing")
        _play_alert()
        request_stop()
