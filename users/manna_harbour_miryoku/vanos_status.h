// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — typing-state indicators, shared by every board's LED code.
//
// This file answers "what transient state should the LED show?"; each board
// decides how to draw it (Cheapino: its one status LED, vanos_encoder.h;
// BM40: the rot180 indicator LEDs). Board-independent, header-only, static
// inline, defines no QMK callback.
//
// ── Configuration ────────────────────────────────────────────────────────────
// Every indicator is OFF unless the board's keymap config.h defines its switch,
// so a board that does not opt in compiles none of this (0 B).
//
//   VANOS_IND_ONESHOT     one-shot Shift pending           (vanos_oneshot_shift.h)
//   VANOS_IND_CAPS_WORD   Caps Word on
//   VANOS_IND_MODS        Gui / Alt / Ctrl held (home-row or layer mods, once
//                         QMK has decided "hold"). Shift is left out by default:
//                         it is held constantly while typing and would make the
//                         LED flicker. Override the set with VANOS_IND_MODS_MASK
//                         (8-bit packed form, e.g. MOD_MASK_CAG | MOD_MASK_SHIFT).
//   VANOS_IND_MEDIA       Media layer held: the live RGB colour BLINKS, so Media
//                         no longer looks like Base; the first RGB key pressed
//                         while it is held makes it steady, so the colour can be
//                         judged while adjusting it.
//
// Draw order (highest first), recommended for every board:
//   one-shot Shift > Caps Word > held layer > Media > mods > locked layer
// The first two last a moment and matter most mid-word; a held layer is the
// next thing you need to know; mods are background information.
//
// Patterns are timing-only (no colour), so the board keeps its own palette:
//   vanos_ind_fast_blink()    100 ms on / 100 ms off      — one-shot Shift
//   vanos_ind_double_blink()  two 100 ms flashes per 1 s  — Caps Word
//   vanos_ind_media_blink()   250 ms on / 250 ms off      — Media, untouched

#pragma once

#include "quantum.h"
#include "manna_harbour_miryoku.h" // U_MEDIA

#ifndef VANOS_IND_MODS_MASK
#    define VANOS_IND_MODS_MASK MOD_MASK_CAG
#endif

static inline bool vanos_ind_fast_blink(uint32_t now) {
    return (now / 100) % 2 == 0;
}

static inline bool vanos_ind_double_blink(uint32_t now) {
    uint16_t ph = now % 1000;
    return ph < 100 || (ph >= 200 && ph < 300);
}

static inline bool vanos_ind_media_blink(uint32_t now) {
    return (now / 250) % 2 == 0;
}

static inline bool vanos_ind_oneshot(void) {
#ifdef VANOS_IND_ONESHOT
    return (get_oneshot_mods() & MOD_MASK_SHIFT) != 0;
#else
    return false;
#endif
}

static inline bool vanos_ind_caps_word(void) {
#ifdef VANOS_IND_CAPS_WORD
    return is_caps_word_on();
#else
    return false;
#endif
}

// Held mods from the configured set (0 = none, or the indicator is off).
static inline uint8_t vanos_ind_mods(void) {
#ifdef VANOS_IND_MODS
    return get_mods() & VANOS_IND_MODS_MASK;
#else
    return 0;
#endif
}

#ifdef VANOS_IND_MEDIA
static bool vanos_ind_media_touched = false; // an RGB key was pressed during this Media hold
#endif

static inline bool vanos_ind_media_held(void) {
#ifdef VANOS_IND_MEDIA
    bool held = layer_state && get_highest_layer(layer_state) == U_MEDIA;
    if (!held) vanos_ind_media_touched = false; // re-arm for the next hold
    return held;
#else
    return false;
#endif
}

// true = blink (untouched), false = steady (an RGB key was used this hold).
static inline bool vanos_ind_media_blinking(void) {
#ifdef VANOS_IND_MEDIA
    return !vanos_ind_media_touched;
#else
    return false;
#endif
}

// Call from process_record_user(); never consumes the key.
static inline void vanos_status_process_record(uint16_t keycode, keyrecord_t *record) {
#ifdef VANOS_IND_MEDIA
    if (record->event.pressed && (IS_UNDERGLOW_KEYCODE(keycode) || IS_RGB_MATRIX_KEYCODE(keycode))) {
        vanos_ind_media_touched = true;
    }
#else
    (void)keycode;
    (void)record;
#endif
}
