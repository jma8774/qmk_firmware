from keys import tap, tap_d
from timing import jitter, jitter_up, sleep_ms
from job import (
    shoot, dash, flash_jump, jump_attack, jump_down_delay, teleport_reset,
    janus1, janus2, janus3, erda_fountain,
    teleport_setup_try, boss_buffs,
)

DJ = 10  # default wait-jitter percentage


def rotation_carcion():
    shoot()


def setup_carcion():
    jump_down_delay(100)
    jump_down_delay(500)
    tap("up")
    tap("up")
    sleep_ms(jitter(500, DJ))
    tap("right")
    tap("right")
    jump_down_delay(100)
    jump_down_delay(500)
    jump_down_delay(100)
    jump_down_delay(500)
    if janus1.try_use():
        sleep_ms(jitter_up(650, DJ))
    jump_attack()
    tap_d("right", 600)
    if janus2.try_use():
        sleep_ms(jitter_up(650, DJ))
    tap_d("right", 600)
    flash_jump()
    if janus3.try_use():
        sleep_ms(jitter_up(650, DJ))
    flash_jump()
    tap_d("right", 200)
    if erda_fountain.try_use():
        sleep_ms(jitter_up(700, DJ))
    tap_d("left", 300)
    teleport_reset()


def loot_carcion():
    jump_down_delay(100)
    jump_down_delay(600)
    for _ in range(4):
        jump_attack()
    teleport_reset()


def buff_script():
    if teleport_setup_try():
        sleep_ms(jitter_up(675, DJ))
    if boss_buffs.try_use():
        sleep_ms(jitter_up(1500, DJ))
