import sys
import time

from keys import tap, tap_d, press, release
from timing import jitter, jitter_up, sleep_ms
from job import (
    shoot, dash, flash_jump, jump_attack, jump_attack_delay, jump_down_delay, teleport_reset, rope,
    janus1, janus2, janus3, erda_fountain,
    teleport_setup_try, boss_buffs, guild_crit_buff, exp_buff, legion_meso_buff, wap_buff,
)
from common import is_enfolding3_teleport_reset_on_screen
import random

DJ = 10  # default wait-jitter percentage
NO_CONSUMABLE = "noconsumable" in sys.argv

if "resetconsumable" in sys.argv:
    print("\n[resetconsumable] -> resetting consumables")
    guild_crit_buff.reset()
    exp_buff.reset()
    legion_meso_buff.reset()
    wap_buff.reset()


def rotation_carcion():
    shoot()

def setup_carcion():
    jump_attack_delay(700)
    dash()
    dash()
    if janus1.try_use():
        sleep_ms(jitter_up(650, DJ))
    tap("c")
    sleep_ms(jitter(1600, DJ))
    if janus2.try_use():
        sleep_ms(jitter_up(650, DJ))
    if random.random() < 0.5:
        press('left')
        sleep_ms(jitter(1300, DJ))
        release('left')
        sleep_ms(jitter(50, DJ))
        dash()
    else:
        tap('left')
        sleep_ms(jitter(50, DJ))
        shoot()
        dash()
        press('left')
        sleep_ms(jitter(600, DJ))
        release('left')
        sleep_ms(jitter(50, DJ))
        dash()
        dash()
    if random.random() < 0.5:
        shoot()
    if janus3.try_use():
        sleep_ms(jitter_up(650, DJ))
    jump_attack()
    if erda_fountain.try_use():
        sleep_ms(jitter_up(700, DJ))
    tap('right')
    sleep_ms(jitter(50, DJ))
    attempts = 0
    while not is_enfolding3_teleport_reset_on_screen():
        attempts += 1
        if attempts % 10 == 0:
            print(f"[setup] -> full reset ({attempts})")
            full_reset_carcion()
        tap('right')
        sleep_ms(jitter(50, 20))
        teleport_reset()
        sleep_ms(jitter(50, 20))

def full_reset_carcion():
    tap('left')
    sleep_ms(jitter(50, 20))
    tap('left')
    sleep_ms(jitter(50, 20))
    dash()
    dash()
    dash()
    jump_down_delay(500)
    jump_down_delay(500)
    jump_down_delay(500)
    press('left')
    sleep_ms(jitter(50, 20))
    tap('e')
    sleep_ms(jitter(50, 20))
    tap('e')
    sleep_ms(jitter(50, 20))
    tap('e')
    sleep_ms(jitter(50, 20))
    release('left')
    sleep_ms(jitter(50, 20))
    flash_jump()
    flash_jump()
    flash_jump()
    flash_jump()
    dash()
    dash()
    dash()
    tap('right')
    sleep_ms(jitter(50, 20))
    tap('right')
    sleep_ms(jitter(200, 20))
    dash()
    tap_d('right', 500)
    rope(3000)
    jump_down_delay(700)
    jump_down_delay(700)
    teleport_setup_try(ignore_cooldown=True)
    sleep_ms(jitter_up(650, 20))
    teleport_reset(delayAfter=700)


def loot_carcion(rune_walker, last_rune_check_2):
    if random.random() < 0.5:
        dash()
    jump_attack_delay(700)
    tap("left")
    sleep_ms(jitter(50, 20))
    tap("left")
    sleep_ms(jitter(50, 20))
    shoot()
    shoot()
    shoot()
    shoot()
    tap('right')
    sleep_ms(jitter(50, 20))
    if last_rune_check_2 is not None and (time.monotonic() - last_rune_check_2) >= 15.0:
        if rune_walker.is_rune_inactive():
            print("\n[loot] -> rune on cooldown, skipping")
        elif rune_walker.find_rune() is not None:
            print("\n[loot] -> rune detected, walking to it")
            rune_walker.go()
            return
        else:
            print("\n[policy] -> no rune detected")
    attempts = 0
    while not is_enfolding3_teleport_reset_on_screen():
        attempts += 1
        if attempts % 10 == 0:
            print(f"[loot] -> full reset ({attempts})")
            full_reset_carcion()
        tap('right')
        sleep_ms(jitter(50, 20))
        teleport_reset()

def buff_script():
    if teleport_setup_try():
        sleep_ms(jitter_up(675, DJ))
    if boss_buffs.try_use():
        sleep_ms(jitter_up(1500, DJ))
    if not NO_CONSUMABLE:
        if guild_crit_buff.try_use():
            sleep_ms(jitter_up(500, DJ))
        if exp_buff.try_use():
            sleep_ms(jitter_up(500, DJ))
        if legion_meso_buff.try_use():
            sleep_ms(jitter_up(500, DJ))
        if wap_buff.try_use():
            sleep_ms(jitter_up(500, DJ))
