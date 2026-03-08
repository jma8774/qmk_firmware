// Script engine – non-blocking command runner implementation.

#include "script_engine.h"
#include "timing.h"
#include "../common.h"
#include QMK_KEYBOARD_H   // brings in timer, register_code, etc.

// ---------------------------------------------------------------------------
// runner_start
// ---------------------------------------------------------------------------

void runner_start(runner_t *r, const cmd_t *script, runner_mode_t mode) {
    r->script    = script;
    r->mode      = mode;
    r->ip        = 0;
    r->wake_time = timer_read32();  // eligible to run immediately
    r->active    = true;
}

// ---------------------------------------------------------------------------
// runner_stop
// ---------------------------------------------------------------------------

void runner_stop(runner_t *r) {
    r->active = false;
}

// ---------------------------------------------------------------------------
// runner_task  –  call once per matrix_scan_user()
// ---------------------------------------------------------------------------
//
// Processes instant commands (PRESS / RELEASE / TAP / CALL) in a tight loop
// so they execute within a single scan cycle.
//
// Yields (returns) on:
//   CMD_WAIT_MS / CMD_WAIT_JITTER / CMD_WAIT_JITTER_DOWN / CMD_WAIT_JITTER_UP  –  sets wake_time, returns
//   CMD_END  –  sets active = false, returns

void runner_task(runner_t *r) {
    if (!r->active) {
        return;
    }

    // Still sleeping?
    if (!timer_expired32(timer_read32(), r->wake_time)) {
        return;
    }

    while (r->active) {
        const cmd_t *c = &r->script[r->ip];
        switch (c->type) {

            case CMD_PRESS:
                uprintf("[runner_task] CMD_PRESS: char=%s (keycode=%d)\n", keycode_to_string(c->kc), c->kc);
                register_code(c->kc);
                break;

            case CMD_RELEASE:
                uprintf("[runner_task] CMD_RELEASE: char=%s (keycode=%d)\n", keycode_to_string(c->kc), c->kc);
                unregister_code(c->kc);
                break;

            case CMD_TAP: {
                // a = base tap duration ms, b = jitter pct (0 = no jitter)
                uint16_t delay = c->a ? (uint16_t)c->a : 50;
                if (c->b) {
                    delay = (uint16_t)jitter(delay, (uint8_t)c->b);
                }
                uprintf("[runner_task] CMD_TAP: char=%s (keycode=%d), delay in between=%u ms\n", keycode_to_string(c->kc), c->kc, delay);
                tap_code_delay(c->kc, delay);
                break;
            }

            case CMD_WAIT_MS:
                uprintf("[runner_task] CMD_WAIT_MS: delay=%lu ms\n", c->a);
                r->wake_time = timer_read32() + c->a;
                r->ip++;
                return;  // yield

            case CMD_WAIT_JITTER:
                {
                    uint32_t delay = jitter(c->a, (uint8_t)c->b);
                    uprintf("[runner_task] CMD_WAIT_JITTER: delay=%lu (base delay=%lu ms, jitter=%lu%%)\n", delay, c->a, c->b);
                    r->wake_time = timer_read32() + delay;
                }
                r->ip++;
                return;  // yield

            case CMD_WAIT_JITTER_DOWN:
                {
                    uint32_t delay = jitter_down(c->a, (uint8_t)c->b);
                    uprintf("[runner_task] CMD_WAIT_JITTER_DOWN: delay=%lu (max=%lu ms, jitter down=%lu%%)\n", delay, c->a, c->b);
                    r->wake_time = timer_read32() + delay;
                }
                r->ip++;
                return;  // yield

            case CMD_WAIT_JITTER_UP:
                {
                    uint32_t delay = jitter_up(c->a, (uint8_t)c->b);
                    uprintf("[runner_task] CMD_WAIT_JITTER_UP: delay=%lu (min=%lu ms, jitter up=%lu%%)\n", delay, c->a, c->b);
                    r->wake_time = timer_read32() + delay;
                }
                r->ip++;
                return;  // yield

            case CMD_CALL:
                r->last_result = c->fn ? c->fn() : false;
                uprintf("[runner_task] CMD_CALL %s: success=%d\n", c->fn_name ? c->fn_name : "?", r->last_result);
                break;

            case CMD_CHANCE:
                r->last_result = timing_chance((uint8_t)c->a);
                uprintf("[runner_task] CMD_CHANCE: last_result=%d\n", r->last_result);
                break;

            case CMD_SKIP_TRUE:
                // uprintf("[runner_task] CMD_SKIP_TRUE: last_result=%d\n", r->last_result);
                if (r->last_result) r->ip += (uint16_t)c->a;
                break;

            case CMD_SKIP_FALSE:
                // uprintf("[runner_task] CMD_SKIP_FALSE: last_result=%d\n", r->last_result);
                if (!r->last_result) r->ip += (uint16_t)c->a;
                break;

            case CMD_END:
                uprintf("[runner_task] CMD_END\n");
                r->active = false;
                return;  // script finished
        }

        r->ip++;
    }
}
