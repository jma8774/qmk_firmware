import time
import winsound
from pathlib import Path
import sys
import cv2

from common import load_template, is_template_in_region
from timing import request_stop, jitter, sleep_ms
import random

NOCHECK = "nocheck" in sys.argv
MINIMAP_REGION = {"top": 0, "left": 0, "width": 350, "height": 350}
ADMIN_REGION = {"top": 424, "left": 511, "width": 1494-511, "height": 831-424}
_carcion_tmpl = cv2.cvtColor(load_template("carcion.png"), cv2.COLOR_BGR2GRAY)
_admin_eye_tmpl = cv2.cvtColor(load_template("maple_admin_eye.png"), cv2.COLOR_BGR2GRAY)
_admin_text_tmpl = cv2.cvtColor(load_template("maple_admin_text.png"), cv2.COLOR_BGR2GRAY)
_last_check = 0.0
_last_admin_check = 0.0
_last_type_to_gm_check = 0.0

_ALERT_PATH = Path(__file__).parent / "sounds" / "alert_loud.wav"


def _type_to_gm(type_string: callable, tap: callable):
    def type_string(string: str):
        for char in string:
            if char == " ":
                tap("space")
            else:
                tap(char)
            sleep_ms(jitter(75, 10))

    now = time.monotonic()
    if now - _last_type_to_gm_check < 10.0:
        return
    _last_type_to_gm_check = now

    random_string = ["hello o-o", "hello", "heyyyy", "o-o"]
    tap("enter")
    sleep_ms(jitter(100, 10))
    type_string(random_string[random.randint(0, len(random_string) - 1)])
    sleep_ms(jitter(100, 10))
    tap("enter")
    sleep_ms(jitter(100, 10))

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
    if now - _last_admin_check < 3.0:
        return
    _last_admin_check = now

    if is_template_in_region(_admin_text_tmpl, ADMIN_REGION, grayscale=True):
        print("[map_check] maple admin TEXT detected — pausing")
        _play_alert()
        request_stop()
    elif is_template_in_region(_admin_eye_tmpl, ADMIN_REGION, grayscale=True):
        print("[map_check] maple admin EYE detected — pausing")
        _play_alert()
        request_stop()


def map_check(type_string: callable, tap: callable):
    if NOCHECK:
        return
    global _last_check
    now = time.monotonic()
    if now - _last_check < 1.0:
        return
    _last_check = now
    if not is_template_in_region(_carcion_tmpl, MINIMAP_REGION, grayscale=True):
        print("[map_check] carcion not found on minimap — pausing")
        _play_alert()
        _type_to_gm(type_string, tap)
        request_stop()
