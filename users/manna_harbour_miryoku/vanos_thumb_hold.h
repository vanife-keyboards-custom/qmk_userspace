// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — thumb tap-hold behaviour, shared by both boards.
//
// Included ONLY by keyboards/*/keymaps/vanos/keymap.c (never by the stock
// manna_harbour_miryoku keymap). These are definitions of QMK's weak callbacks,
// so this header must be included from exactly one .c per firmware build — which
// is the case: each build compiles only its own keymap.c. Same arrangement as
// vanos_chordal_hold.h.
//
// Nothing here is board-specific: the rule keys off *what a key is* (a layer-tap
// to a thumb layer), never off a matrix position, so BM40 and Corne cannot drift
// and an alphas/nav change cannot invalidate it.

#pragma once

#include "quantum.h"    // keyrecord_t, keycode macros, is_flow_tap_key()
#include "vanos_keys.h" // vanos_is_thumb_key(), vanos_is_shift_mod_tap()

// ── Flow Tap exemptions: thumbs, and Shift ───────────────────────────────────
//
// Two independent exemptions share one callback. Both return 0, which is how QMK
// spells "Flow Tap disabled for this key" (docs/tap_hold.md:534).
//
// (2) SHIFT — docs/LAYOUT-RESEARCH.md:255-261, item 1 of Getreuer's ideas, rated
//     "highest value" there. FLOW_TAP_TERM fires uniformly across all home-row
//     mods, but Shift is not like the others: Gui/Alt/Ctrl are essentially never
//     wanted mid-burst (that is the whole premise of Flow Tap), whereas Shift is
//     needed *during* a burst constantly — sentence capitals, CamelCase, ! and ?.
//
//     Failure it removes: typing "hello World" fast. The W is left-hand, so under
//     CHORDAL_HOLD the Shift must come from the right index, LSFT_T(KC_J). The
//     preceding space is LT(U_NAV, KC_SPC) whose tap keycode KC_SPC is in QMK's
//     default Flow Tap list, so if J lands within 150 ms of it, J is forced to
//     tap:  "hello jworld".  Signature: a stray j or f right before a letter that
//     should have been capitalised.
//
//     This is Getreuer's TYPING_STREAK_MODMASK (~MOD_MASK_SHIFT) reached through
//     an upstream hook instead of his qmk_firmware fork.
//
// (1) THUMBS —
//
// THE BUG (present in the flashed firmware, introduced by FLOW_TAP_TERM in
// vanos.h — stock Miryoku does not have it):
//
//   QMK's default is_flow_tap_key() (quantum/action_tapping.c:1083) matches on a
//   tap-hold key's *tap* keycode, and its list is KC_A..KC_Z, KC_SPC, , . ; /
//   The left primary thumb is LT(U_NAV, KC_SPC) — tap keycode KC_SPC — so QMK
//   classifies the Nav thumb as a letter key. Press it within FLOW_TAP_TERM
//   (150 ms) of a letter and Flow Tap forces it to settle as a TAP immediately:
//   you get a space, the Nav layer never activates, and the next key lands on the
//   base layer. Symptom: "Nav randomly fails when I'm typing fast, but always
//   works if I pause first."
//
//   Flow Tap runs BEFORE the hold decision (flow_tap_key_if_within_term() is
//   consulted first), so HOLD_ON_OTHER_KEY_PRESS on thumbs cannot rescue this —
//   it needs its own fix.
//
// THE FIX: return 0 (= Flow Tap disabled) for thumb keys; keep QMK's default
// behaviour for everything else, so the home-row mods — the keys Flow Tap exists
// for — are untouched.
//
// Scope: only LT(U_NAV, KC_SPC) is affected today; the other five thumbs have tap
// keycodes (Esc/Tab/Ent/Bsp/Del) that are not in the default list. Covering all
// thumbs costs nothing and survives a re-assignment of any thumb tap.
#ifdef FLOW_TAP_TERM

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    if (vanos_is_thumb_key(keycode)) {
        return 0; // thumbs never participate in Flow Tap
    }
    // Shift mod-taps (LSFT_T on both index fingers) never participate either.
    // (Predicate and its MOD_LSFT-vs-MOD_MASK_SHIFT trap: vanos_keys.h.)
    if (vanos_is_shift_mod_tap(keycode)) {
        return 0;
    }
    // QMK's default rule, verbatim (quantum/action_tapping.c:1099).
    return (is_flow_tap_key(keycode) && is_flow_tap_key(prev_keycode)) ? FLOW_TAP_TERM : 0;
}

#endif // FLOW_TAP_TERM
