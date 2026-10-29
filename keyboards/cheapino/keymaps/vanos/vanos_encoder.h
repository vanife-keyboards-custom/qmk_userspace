// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — layer-aware encoder, status LED and pomodoro for the Cheapino.
// Include once, from keymaps/vanos/keymap.c. Spec and test plan:
// docs/devices/cheapino/encoder.md (kboards repo). Background:
// docs/ENCODER-RESEARCH.md (R10, C1, C8; the pomodoro is new).
//
// Cheapino-only (the only encoder board in the fleet), so it lives with this
// keymap rather than in users/: nothing here has to stay in sync with BM40/Corne.
//
// ── What the knob does, per active layer ────────────────────────────────────
//
//   layer              rotate CCW / CW               push (tap)
//   BASE, TAP, EXTRA   volume − / +                  play/pause
//   NAV                keyboard layout prev / next   back to US
//   MOUSE              wheel up / down (Shift → ↔)   middle click
//   MEDIA              brightness of ALL screens     all screens to 50 %
//   NUM                ↓ / ↑                         play/pause
//   SYM                undo / redo                   play/pause
//   FUN                pomodoro length − / + 5 min   play/pause
//   BUTTON             volume − / +                  play/pause
//
//   Push, in priority order:
//     1. pomodoro alarm flashing  → acknowledge (stops the flashing)
//     2. long press (≥ 600 ms)    → start / cancel the pomodoro, on any layer
//     3. a layer is LOCKED        → unlock (default layer back to BASE)
//     4. otherwise                → the layer's tap action from the table
//
// "Locked" = Miryoku's double-tap layer lock (TD(U_TD_U_<layer>)), which makes
// the layer the *default* layer. Nothing else in Miryoku unlocks it except
// another double-tap on the BASE lock, so the push is the quick way out.
//
// Brightness and layout need the host: the firmware only sends key chords, and
// Hyprland binds them (docs/devices/cheapino/host/cheapino.lua):
//   Ctrl + XF86MonBrightnessUp/Down      → cheapino-brightness-all +5% / 5%-
//   Ctrl + Shift + XF86MonBrightnessUp   → cheapino-brightness-all 50%
//   XF86Launch9 (F18)                    → "pomodoro done" notification
// Layout switching reuses the existing Ctrl+Shift+7/8/9/6 binds in
// ~/.config/hypr/input.lua, so it works without any new host config.
//
// ── Status LED (the single WS2812 on the RP2040-Zero, left half) ─────────────
//
//   pomodoro alarm     red, fast flash, until acknowledged (gives up after 15 min)
//   layer held         solid layer colour
//   layer locked       layer colour, slow blink
//   pomodoro running   your normal colour + a short red blip every 5 s
//   FUN rotate         flash of the new length: green (5 min) → red (60 min)
//   otherwise          your normal colour (whatever RGB_* set, read from EEPROM)
//
// Indicator colours are written with the *_noeeprom calls, so they never wear
// or change the saved RGB setting.
//
// Owns QMK callbacks: encoder_update_user, process_record_user,
// housekeeping_task_user. Nothing else in the Cheapino vanos build defines
// them; if vanos_caps_lock.h is ever re-enabled, merge its process_record_user
// into the one below.

#pragma once

#include "quantum.h"
#include "manna_harbour_miryoku.h" // U_BASE … U_FUN layer indices

// The push key's keycode. config.h maps CHEAPINO_ENCODER_PUSH to this before
// the Miryoku layout macro expands, so the keymap carries it on every layer.
#define VANOS_ENC_PUSH QK_USER_0

// ── Tunables ─────────────────────────────────────────────────────────────────
#ifndef VANOS_ENC_LONG_PRESS_MS
#    define VANOS_ENC_LONG_PRESS_MS 600
#endif
#ifndef VANOS_POMODORO_DEFAULT_MIN
#    define VANOS_POMODORO_DEFAULT_MIN 25
#endif
#define VANOS_POMODORO_STEP_MIN 5
#define VANOS_POMODORO_MAX_MIN 60
#define VANOS_POMODORO_ALARM_MAX_MS (15UL * 60 * 1000)
// Tapped once when a pomodoro ends, for a desktop notification. Comment out to
// keep the firmware silent towards the host.
#define VANOS_POMODORO_DONE_KEY KC_F18 // XF86Launch9 under XKB's evdev rules
#define VANOS_LED_VAL 120             // indicator brightness, 0–255
#define VANOS_LED_BOOT_QUIET_MS 3000  // leave the board's boot hue sweep alone

// Keyboard layouts as configured in ~/.config/hypr/input.lua
// (kb_layout = "us,de,ru,ua") and the chords that select each one there.
static const uint16_t vanos_kb_layouts[] = {
    C(S(KC_7)), // 0 US
    C(S(KC_8)), // 1 DE
    C(S(KC_9)), // 2 RU
    C(S(KC_6)), // 3 UA
};
#define VANOS_KB_LAYOUT_COUNT (sizeof(vanos_kb_layouts) / sizeof(vanos_kb_layouts[0]))

// ── State ────────────────────────────────────────────────────────────────────
static uint8_t  vanos_kb_layout     = 0; // firmware's idea of the active layout
static uint16_t vanos_push_timer    = 0;
static uint8_t  pomodoro_minutes    = VANOS_POMODORO_DEFAULT_MIN;
static bool     pomodoro_running    = false;
static uint32_t pomodoro_started    = 0;
static bool     pomodoro_alarm      = false;
static uint32_t pomodoro_alarm_from = 0;
// A short LED override (feedback for pomodoro actions), drawn on top of
// everything except the alarm. count blinks; count 0 = solid for solid_ms.
static uint32_t led_flash_start = 0;
static uint16_t led_flash_len   = 0;
static uint8_t  led_flash_hue   = 0;
static uint8_t  led_flash_count = 0;
#define VANOS_LED_FLASH_STEP_MS 150

static void vanos_led_flash(uint8_t hue, uint8_t count, uint16_t solid_ms) {
    led_flash_hue   = hue;
    led_flash_count = count;
    led_flash_start = timer_read32();
    led_flash_len   = count ? 2 * count * VANOS_LED_FLASH_STEP_MS : solid_ms;
}

// Hues on QMK's 0–255 wheel (color.h only has full HSV triplets).
#define HUE_RED 0
#define HUE_ORANGE 21
#define HUE_YELLOW 43
#define HUE_GREEN 85
#define HUE_TEAL 106
#define HUE_CYAN 128
#define HUE_BLUE 170
#define HUE_PURPLE 191
#define HUE_MAGENTA 213
#define HUE_PINK 234

// The layer the knob acts on: the highest momentary layer, else the default
// layer (which is where a Miryoku double-tap lock puts it).
static uint8_t vanos_effective_layer(void) {
    return layer_state ? get_highest_layer(layer_state) : get_highest_layer(default_layer_state);
}

static bool vanos_layer_locked(void) {
    return get_highest_layer(default_layer_state) != U_BASE;
}

// ── Pomodoro ─────────────────────────────────────────────────────────────────
static uint8_t pomodoro_hue(void) {
    // 5 min → green (85), 60 min → red (0)
    return 85 - (uint16_t)85 * (pomodoro_minutes - VANOS_POMODORO_STEP_MIN) /
                    (VANOS_POMODORO_MAX_MIN - VANOS_POMODORO_STEP_MIN);
}

static void pomodoro_toggle(void) {
    pomodoro_running = !pomodoro_running;
    if (pomodoro_running) {
        pomodoro_started = timer_read32();
        vanos_led_flash(HUE_GREEN, 3, 0);
    } else {
        vanos_led_flash(HUE_RED, 1, 0);
    }
}

static void pomodoro_task(void) {
    if (pomodoro_running && timer_elapsed32(pomodoro_started) >= (uint32_t)pomodoro_minutes * 60 * 1000) {
        pomodoro_running    = false;
        pomodoro_alarm      = true;
        pomodoro_alarm_from = timer_read32();
#ifdef VANOS_POMODORO_DONE_KEY
        tap_code(VANOS_POMODORO_DONE_KEY);
#endif
    }
    if (pomodoro_alarm && timer_elapsed32(pomodoro_alarm_from) >= VANOS_POMODORO_ALARM_MAX_MS) {
        pomodoro_alarm = false;
    }
}

// ── Encoder rotation ─────────────────────────────────────────────────────────
bool encoder_update_user(uint8_t index, bool clockwise) {
    // Our units report the direction reversed; see
    // ../manna_harbour_miryoku/cheapino_encoder.h for the evidence.
    clockwise = !clockwise;

    switch (vanos_effective_layer()) {
        case U_NAV: // C8: keyboard layout dial
            vanos_kb_layout = (vanos_kb_layout + (clockwise ? 1 : VANOS_KB_LAYOUT_COUNT - 1)) % VANOS_KB_LAYOUT_COUNT;
            tap_code16(vanos_kb_layouts[vanos_kb_layout]);
            break;
        case U_MOUSE: // wheel; a held Shift makes it horizontal in most apps
            tap_code(clockwise ? MS_WHLD : MS_WHLU);
            break;
        case U_MEDIA: // brightness of every screen, via the host bind
            tap_code16_delay(clockwise ? C(KC_BRIU) : C(KC_BRID), 10);
            break;
        case U_NUM:
            tap_code(clockwise ? KC_UP : KC_DOWN);
            break;
        case U_SYM: // explicit Ctrl chords: Miryoku's default U_UND/U_RDO are
                    // KC_UNDO/KC_AGIN, which Linux apps mostly ignore
            tap_code16(clockwise ? C(S(KC_Z)) : C(KC_Z));
            break;
        case U_FUN: // pomodoro length; takes effect at the next start
            if (clockwise && pomodoro_minutes < VANOS_POMODORO_MAX_MIN) pomodoro_minutes += VANOS_POMODORO_STEP_MIN;
            if (!clockwise && pomodoro_minutes > VANOS_POMODORO_STEP_MIN) pomodoro_minutes -= VANOS_POMODORO_STEP_MIN;
            vanos_led_flash(pomodoro_hue(), 0, 800);
            break;
        default: // BASE, TAP, EXTRA, BUTTON
            tap_code_delay(clockwise ? KC_VOLU : KC_VOLD, 10);
            break;
    }
    return false; // skip encoder_update_kb's default (volume)
}

// ── Encoder push ─────────────────────────────────────────────────────────────
static void vanos_push_tap(void) {
    if (vanos_layer_locked()) {
        default_layer_set((layer_state_t)1 << U_BASE);
        return;
    }
    switch (vanos_effective_layer()) {
        case U_NAV:
            vanos_kb_layout = 0;
            tap_code16(vanos_kb_layouts[0]);
            break;
        case U_MOUSE:
            tap_code(MS_BTN3);
            break;
        case U_MEDIA:
            tap_code16_delay(C(S(KC_BRIU)), 10);
            break;
        default:
            tap_code_delay(KC_MPLY, 10);
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode != VANOS_ENC_PUSH) return true;

    if (record->event.pressed) {
        vanos_push_timer = timer_read();
        return false;
    }
    // Decided on release, so a long press never also fires the tap action.
    if (pomodoro_alarm) {
        pomodoro_alarm = false;
    } else if (timer_elapsed(vanos_push_timer) >= VANOS_ENC_LONG_PRESS_MS) {
        pomodoro_toggle();
    } else {
        vanos_push_tap();
    }
    return false;
}

// ── Status LED ───────────────────────────────────────────────────────────────
static uint8_t vanos_layer_hue(uint8_t layer) {
    switch (layer) {
        case U_NAV:    return HUE_CYAN;
        case U_MOUSE:  return HUE_YELLOW;
        case U_MEDIA:  return HUE_PURPLE;
        case U_NUM:    return HUE_BLUE;
        case U_SYM:    return HUE_GREEN;
        case U_FUN:    return HUE_ORANGE;
        case U_BUTTON: return HUE_TEAL;
        case U_TAP:    return HUE_MAGENTA;
        default:       return HUE_PINK; // EXTRA
    }
}

#define LED_USER 0xFFFFFFFF // sentinel: show the saved (EEPROM) colour

static uint32_t vanos_led_pack(uint8_t h, uint8_t s, uint8_t v) {
    return ((uint32_t)h << 16) | ((uint32_t)s << 8) | v;
}

static uint32_t vanos_led_wanted(void) {
    uint32_t now = timer_read32();
    bool     on;

    if (pomodoro_alarm) {
        on = (timer_elapsed32(pomodoro_alarm_from) / 200) % 2 == 0;
        return vanos_led_pack(HUE_RED, 255, on ? 255 : 0);
    }
    if (led_flash_len && timer_elapsed32(led_flash_start) < led_flash_len) {
        if (led_flash_count == 0) return vanos_led_pack(led_flash_hue, 255, VANOS_LED_VAL);
        on = (timer_elapsed32(led_flash_start) / VANOS_LED_FLASH_STEP_MS) % 2 == 0;
        return vanos_led_pack(led_flash_hue, 255, on ? VANOS_LED_VAL : 0);
    }
    if (layer_state) {
        return vanos_led_pack(vanos_layer_hue(get_highest_layer(layer_state)), 255, VANOS_LED_VAL);
    }
    if (vanos_layer_locked()) {
        on = (now / 600) % 2 == 0;
        return vanos_led_pack(vanos_layer_hue(get_highest_layer(default_layer_state)), 255, on ? VANOS_LED_VAL : 0);
    }
    if (pomodoro_running && timer_elapsed32(pomodoro_started) % 5000 < 120) {
        return vanos_led_pack(HUE_RED, 255, VANOS_LED_VAL);
    }
    return LED_USER;
}

static void vanos_led_task(void) {
    static uint32_t shown = LED_USER;
    if (timer_read32() < VANOS_LED_BOOT_QUIET_MS) return;

    uint32_t want = vanos_led_wanted();
    if (want == shown) return;

    if (want == LED_USER) {
        rgblight_reload_from_eeprom();
        if (!rgblight_is_enabled()) rgblight_disable_noeeprom();
    } else {
        if (shown == LED_USER) {
            rgblight_enable_noeeprom();
            rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
        }
        rgblight_sethsv_noeeprom(want >> 16, (want >> 8) & 0xFF, want & 0xFF);
    }
    shown = want;
}

void housekeeping_task_user(void) {
    pomodoro_task();
    vanos_led_task();
}
