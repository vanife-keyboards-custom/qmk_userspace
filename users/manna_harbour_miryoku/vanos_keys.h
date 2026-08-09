// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — shared key-class predicates for the tap-hold callbacks.
//
// Header-only, `static inline`, no QMK weak overrides — so it is safe to include
// from anywhere, unlike vanos_thumb_hold.h / vanos_permissive_hold.h which each
// DEFINE a callback and must be included from exactly one .c per build.
//
// Extracted 2026-07-30 (bug 001) when the second callback needed the same two
// predicates as the first. Both `get_flow_tap_term()` and `get_permissive_hold()`
// draw the same line: thumbs and Shift get the fast path, the other home-row mods
// do not. One definition, so the two callbacks cannot drift into disagreeing.
//
// Nothing here is board-specific: every predicate keys off *what a key is*, never
// off a matrix position, so BM40 and Corne cannot drift and an alphas/nav change
// cannot invalidate it.

#pragma once

#include "quantum.h"               // keycode macros, IS_QK_* / QK_*_GET_*
#include "manna_harbour_miryoku.h" // enum miryoku_layers (U_BUTTON)

// ── "Is this a thumb key?" ───────────────────────────────────────────────────
//
// Every Miryoku thumb key is a layer-tap: LT(U_MEDIA,Esc) LT(U_NAV,Spc)
// LT(U_MOUSE,Tab) │ LT(U_SYM,Ent) LT(U_NUM,Bsp) LT(U_FUN,Del).
//
// The ONLY other layer-taps in the keymap are LT(U_BUTTON, Z) and
// LT(U_BUTTON, /) — those sit under the *pinkies*, where thumb rules must not
// apply. Excluding U_BUTTON is therefore exactly equivalent to "is a thumb key",
// without hardcoding a single matrix position.
//
// ⚠ If the Button layer is ever moved or replaced (e.g. relocating the Mouse
// layer onto the pinky holds to free a thumb for Shift), update this predicate
// in the same commit — it is the one place that encodes "pinky, not thumb".
static inline bool vanos_is_thumb_key(uint16_t keycode) {
    return IS_QK_LAYER_TAP(keycode) && QK_LAYER_TAP_GET_LAYER(keycode) != U_BUTTON;
}

// ── "Is this a Shift mod-tap?" ───────────────────────────────────────────────
//
// Matches LSFT_T(KC_F) and LSFT_T(KC_J), Miryoku's two index-finger Shifts.
//
// ⚠ MOD_LSFT here is the 5-bit encoding used *inside* a mod-tap keycode
// (modifiers.h:15-24: MOD_LSFT 0x02, MOD_RSFT 0x12). Do NOT reach for
// MOD_MASK_SHIFT — that is the 8-bit packed form (0x22) and would never match,
// silently returning false for every Shift key. Masking with MOD_LSFT catches
// left and right Shift, and any combined mod containing Shift.
static inline bool vanos_is_shift_mod_tap(uint16_t keycode) {
    return IS_QK_MOD_TAP(keycode) && (QK_MOD_TAP_GET_MODS(keycode) & MOD_LSFT);
}
