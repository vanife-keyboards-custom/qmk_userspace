// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — CHORDAL_HOLD handedness map, shared by both boards.
//
// Included ONLY by keyboards/*/keymaps/vanos/keymap.c (never by the stock
// manna_harbour_miryoku keymap). It is a definition, not a declaration, so it must
// be included from exactly one .c per firmware build — which is the case: each
// build compiles only its own keymap.c.
//
// Written against LAYOUT_miryoku (defined per board in
// keymaps/manna_harbour_miryoku/config.h, which the vanos config.h includes), so
// the SAME 36-key map serves BM40 and Corne and the two boards cannot drift.
// This strong definition overrides the weak, geometry-guessed
// chordal_hold_layout that QMK generates into keyboard.c from keyboard.json.
//
// Positions LAYOUT_miryoku drops on each board (BM40: middle columns and the 2u
// spacebar; Corne: outer pinky columns and the ex2 keys) expand to KC_NO = 0 here.
// They are dead keys in this keymap, so their handedness is never read — but if the
// Corne's +6 pinky keys are ever mapped, give them 'L'/'R' in the same commit.

#pragma once

#ifdef CHORDAL_HOLD

// 'L'/'R' — a tap-hold key settles as HOLD only when interrupted by the other hand;
//           a same-hand roll always taps (this is the misfire fix).
// '*'     — exempt: may settle as hold with any key, either hand.
//
// All six thumb keys are '*' on purpose. They are Miryoku's layer-taps, and the
// layers they reach (Sym/Num on the left hand from the right thumb, Nav/Mouse on
// the right hand from the left thumb) are also used with same-hand keys — the
// opposite-hands rule would silently downgrade those holds to taps and make layer
// access unreliable. Thumbs are slow and deliberate, so they do not misfire the way
// the index-finger Shift does. QMK's docs suggest the same exemption.
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT_miryoku(
  'L', 'L', 'L', 'L', 'L',              'R', 'R', 'R', 'R', 'R',
  'L', 'L', 'L', 'L', 'L',              'R', 'R', 'R', 'R', 'R',
  'L', 'L', 'L', 'L', 'L',              'R', 'R', 'R', 'R', 'R',
  '*', '*', '*', '*', '*',              '*', '*', '*', '*', '*'
);

#endif // CHORDAL_HOLD
