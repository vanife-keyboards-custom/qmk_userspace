// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — tap behaviour of the one-shot Shift thumb (VANOS_OSSFT_MOU) and its
// on/off toggle (VANOS_OSSFT_TOGG). The keys, the layer tables and the rationale
// live in vanos_oneshot_layers.h.
//
// Header-only, static inline, defines no QMK callback: each vanos keymap.c owns
// process_record_user() and calls this first (same pattern as vanos_flat.h).
//
// The double tap is state-based, not timed: the second tap asks "is a one-shot
// Shift pending?". There is no tap-dance, so the first tap never waits out a
// window, and there is no timing to tune.
//
// ── On/off ───────────────────────────────────────────────────────────────────
// Bit 0 of the user EEPROM word (eeconfig_*_user; nothing else in vanos uses
// it) stores "one-shot Shift DISABLED", so a fresh or reset EEPROM (all zeros)
// means ON. Off = the thumb tap sends plain Tab, i.e. stock Tab•Mou. It is read
// lazily on the first relevant key, so no keyboard_post_init_user() is needed
// (BM40 rot180 and the Corne already define one). eeconfig_update_user() only
// writes on change, and the toggle is a rare manual action, so flash wear is a
// non-issue on the RP2040 boards' wear-levelled EEPROM.

#pragma once

#include "quantum.h"
#include "manna_harbour_miryoku.h" // U_MOUSE (used by VANOS_OSSFT_MOU)

#define VANOS_OSSFT_EE_DISABLED (1UL << 0)

static int8_t vanos_ossft_on = -1; // -1 = not read from EEPROM yet

static inline bool vanos_oneshot_shift_enabled(void) {
    if (vanos_ossft_on < 0) {
        vanos_ossft_on = (eeconfig_read_user() & VANOS_OSSFT_EE_DISABLED) ? 0 : 1;
    }
    return vanos_ossft_on;
}

static inline void vanos_oneshot_shift_toggle(void) {
    bool     on = !vanos_oneshot_shift_enabled();
    uint32_t ee = eeconfig_read_user();
    vanos_ossft_on = on;
    eeconfig_update_user(on ? (ee & ~VANOS_OSSFT_EE_DISABLED) : (ee | VANOS_OSSFT_EE_DISABLED));
    if (!on) {
        del_oneshot_mods(MOD_MASK_SHIFT); // leave nothing pending behind
        caps_word_off();
    }
}

static inline bool vanos_oneshot_shift_process_record(uint16_t keycode, keyrecord_t *record) {
    if (keycode == VANOS_OSSFT_TOGG) {
        if (record->event.pressed) vanos_oneshot_shift_toggle();
        return false;
    }
    if (keycode != VANOS_OSSFT_MOU || record->tap.count == 0) {
        return true; // not ours, or the hold (Mouse layer): QMK handles it
    }
    if (!vanos_oneshot_shift_enabled()) {
        // Off: behave exactly like stock LT(U_MOUSE, KC_TAB).
        if (record->event.pressed) {
            register_code(KC_TAB);
        } else {
            unregister_code(KC_TAB);
        }
        return false;
    }
    if (record->event.pressed) {
        if (is_caps_word_on()) {
            caps_word_off();
        } else if (get_oneshot_mods() & MOD_MASK_SHIFT) {
            del_oneshot_mods(MOD_MASK_SHIFT);
            caps_word_on();
        } else {
            add_oneshot_mods(MOD_BIT(KC_LSFT));
        }
    }
    return false; // never let the KC_LSFT placeholder reach the host
}
