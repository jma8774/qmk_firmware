from keys import tap_d
from timing import jitter, jitter_up, sleep_ms, chance
from job import (
    rope, dash, flash_jump, jump_attack, teleport_reset,
    janus1, janus2, janus3, erda_fountain,
    teleport_setup_try, boss_buffs,
)

DJ = 10  # default wait-jitter percentage


def rotation_odium():
    for _ in range(4):
        jump_attack()
    if chance(15):
        jump_attack()
    teleport_reset()
    sleep_ms(jitter_up(150, DJ))


def setup_odium():
    rope()
    dash()
    if janus1.try_use():
        sleep_ms(jitter_up(560, DJ))
    dash()
    flash_jump()
    if janus2.try_use():
        sleep_ms(jitter_up(560, DJ))
    dash()
    flash_jump()
    if janus3.try_use():
        sleep_ms(jitter_up(560, DJ))
    flash_jump()
    tap_d("right", 400)
    if erda_fountain.try_use():
        sleep_ms(jitter_up(600, DJ))
    sleep_ms(jitter(200, DJ))
    teleport_reset()


def buff_script():
    if teleport_setup_try():
        sleep_ms(jitter_up(675, DJ))
    if boss_buffs.try_use():
        sleep_ms(jitter_up(1500, DJ))
