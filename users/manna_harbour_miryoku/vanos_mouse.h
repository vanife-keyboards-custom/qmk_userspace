// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — Mouse layer acceleration + wl-kbptr trigger override, shared by both boards.
//
// Miryoku picks the Mouse layer's keycode table itself, in
// miryoku_babel/miryoku_layer_selection.h, guarded by
// `#if !defined(MIRYOKU_LAYER_MOUSE)`. Defining MIRYOKU_LAYER_MOUSE here, before
// that file is processed, short-circuits its own INVERTEDT/VI/FLIP selection
// logic and wins outright — so the whole miryoku_babel/ tree stays untouched and
// safe to re-vendor from upstream later, and this one addition lives in its own
// file rather than growing vanos.h.
//
// ⚠ KNOWN LIMITATION: this hardcodes the INVERTEDT Nav variant. The body below is
// MIRYOKU_ALTERNATIVES_MOUSE_INVERTEDT (miryoku_layer_alternatives.h) copied
// verbatim with three substitutions on the left hand's index-inner column (T/G/B
// on the base QWERTY layer). It does NOT reproduce Miryoku's #elif chain over
// MIRYOKU_NAV_INVERTEDT / MIRYOKU_NAV_VI / MIRYOKU_LAYERS_FLIP — if MIRYOKU_NAV is
// ever changed away from INVERTEDT in custom_rules.mk, this override will keep
// forcing the inverted-T shape (with the T/G/B accel keys) regardless, instead of
// auto-adapting the way Miryoku's own selection does. Not solved generically here;
// revisit this file by hand if that ever changes.
//
// T/G/B (left hand, index-inner column) become mouse-acceleration presets, reached
// by the left hand while the right hand drives the cursor on this same layer:
//   T (row 1) → MS_ACL2 — burst/fast, full speed instantly
//   G (row 2) → MS_ACL1 — half speed
//   B (row 3) → MS_ACL0 — precision/slow
// Each replaces the U_NA (KC_NO) that sat there upstream, so this is additive.
//
// Y/H (right hand, index-inner column, rows 1/2) send LGUI(KC_Y)/LGUI(KC_H) — Win+Y
// and Win+H — which Hyprland binds to launch wl-kbptr (see
// ~/.config/hypr/bindings.conf on the laptop): H fires the one-shot floating+click
// auto-detect, Y enters the sticky cursor submap. Both replace U_NU (KC_NO,
// "available but not used") upstream, so this is additive too: no other position on
// the Mouse layer changes.

#pragma once

#define MIRYOKU_LAYER_MOUSE \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   MS_ACL2,           LGUI(KC_Y),        KC_WH_L,           KC_MS_U,           KC_WH_R,           KC_WH_U,           \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           MS_ACL1,           LGUI(KC_H),        KC_MS_L,           KC_MS_D,           KC_MS_R,           KC_WH_D,           \
U_NA,              KC_ALGR,           TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  MS_ACL0,           U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,             \
U_NP,              U_NP,              U_NA,              U_NA,              U_NA,              KC_BTN2,           KC_BTN1,           KC_BTN3,           U_NP,              U_NP
