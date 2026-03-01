import time
import winsound
from pathlib import Path

from common import load_template_gray, is_template_in_region
from timing import request_stop, jitter, sleep_ms
from notify import notify
from config import NOCHECK
import random
MINIMAP_REGION = {"top": 0, "left": 0, "width": 350, "height": 350}
ADMIN_REGION = {"top": 424, "left": 511, "width": 1494-511, "height": 831-424}
_carcion_tmpl = load_template_gray("carcion.png")
_tallahart_tmpl = load_template_gray("tallahart.png")
_admin_eye_tmpl = load_template_gray("maple_admin_eye.png")
_admin_text_tmpl = load_template_gray("maple_admin_text.png")
_last_check = 0.0
_last_admin_check = 0.0
_last_type_to_gm_check = None

_ALERT_PATH = Path(__file__).parent / "sounds" / "alert_loud.wav"


def _type_to_gm(type_string: callable, tap: callable):
    global _last_type_to_gm_check
    now = time.monotonic()
    if _last_type_to_gm_check != None and now - _last_type_to_gm_check < 10.0:
        print("[map_check] type to gm check skipped")
        return
    _last_type_to_gm_check = now

    print("[map_check] type to gm check")
    random_string = ["hello o-o", "hello", "hey there", "o-o"]
    sleep_ms(jitter(2000, 10))
    tap("enter", check=False)
    sleep_ms(jitter(200, 10))
    type_string(random_string[random.randint(0, len(random_string) - 1)], check=False)
    sleep_ms(jitter(200, 10))
    tap("enter", check=False)
    sleep_ms(jitter(200, 10))

def _play_alert():
    if _ALERT_PATH.exists():
        winsound.PlaySound(str(_ALERT_PATH), winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_LOOP)

def stop_alert():
    winsound.PlaySound(None, winsound.SND_PURGE)


def admin_check(type_string: callable, tap: callable):
    if NOCHECK:
        return
    global _last_admin_check
    now = time.monotonic()
    if now - _last_admin_check < 2.0:
        return
    _last_admin_check = now
    stopped = False
    if is_template_in_region(_admin_text_tmpl, ADMIN_REGION, grayscale=True, name="girl_text"):
        print("[map_check] maple admin TEXT detected — pausing")
        notify("ADMIN TEXT detected — bot paused", numberOfSends=5)
        _play_alert()
        request_stop()
        stopped = True
    elif is_template_in_region(_admin_eye_tmpl, ADMIN_REGION, grayscale=True, name="girl_eye"):
        print("[map_check] maple admin EYE detected — pausing")
        notify("ADMIN EYE detected — bot paused", numberOfSends=5)
        _play_alert()
        request_stop()
        stopped = True
    return stopped

def map_check(type_string: callable, tap: callable):
    if NOCHECK:
        return
    global _last_check
    now = time.monotonic()
    if now - _last_check < 1.0:
        return
    _last_check = now
    stopped = False
    if not is_template_in_region(_tallahart_tmpl, MINIMAP_REGION, grayscale=True, name="tallahart_minimap"):
        print("[map_check] tallahart not found on minimap — pausing")
        notify("Map lost (carcion not on minimap) — bot paused", numberOfSends=5)
        _play_alert()
        _type_to_gm(type_string, tap)
        request_stop()
        stopped = True
    return stopped
