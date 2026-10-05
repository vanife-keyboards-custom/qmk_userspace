// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

#pragma once
// vanos = miryoku:default engine + vanos tweaks.
#include "../manna_harbour_miryoku/config.h"    // reuse this board's LAYOUT_miryoku adapter
#include "vanos.h"                              // shared vanos customizations (timing tune)

// ── Split link watchdog — Corne only (the BM40 is not split) ──────────────────
// Reboots the slave half when the master stops talking to it, which QMK's own
// SPLIT_WATCHDOG_ENABLE does not do after the first successful connection. Fixes
// "right half dead until I replug the cable" after a KVM switch — the full
// diagnosis is in users/manna_harbour_miryoku/vanos_split_watchdog.h and
// docs/devices/corne-v4.md.
//
// KILL SWITCH: comment out the next line (and reflash both halves) to get stock
// QMK behaviour back. SPLIT_TRANSACTION_IDS_USER can stay either way — an unused
// transaction id costs nothing.
//#define VANOS_SPLIT_WATCHDOG // DIAGNOSTIC 2026-09-23: off for the A/B test, see keymap.c
#define SPLIT_TRANSACTION_IDS_USER VANOS_RPC_WATCHDOG

// Optional tuning (defaults in vanos_split_watchdog.h): ping every 100 ms, reboot
// the slave after 1000 ms of silence, 5000 ms grace before the first ping.
//#define VANOS_SPLIT_WATCHDOG_PING_INTERVAL 100
//#define VANOS_SPLIT_WATCHDOG_TIMEOUT       1000
//#define VANOS_SPLIT_WATCHDOG_BOOT_GRACE    5000

// ESCALATION, off by default: arm the RP2040 hardware watchdog on the slave too.
// Enable ONLY if the software watchdog above turns out not to recover the half —
// that result would mean the slave's main loop is hung, not just its link, and a
// hung loop cannot reboot itself. Read the ⚠ in vanos_split_watchdog.h first.
//#define VANOS_SPLIT_WATCHDOG_HW
//#define VANOS_SPLIT_WATCHDOG_HW_TIMEOUT    3000

// ── DIAGNOSTIC 2026-09-24: slow the split serial link ─────────────────────────
// Probe run at the stock 230400 baud (SELECT_SOFT_SERIAL_SPEED 1) showed ~15 % of
// RPC pings failing on a link with zero drops and zero slave resets — a noisy
// data wire, not power. 3 = 57600 baud, four times the noise margin. MUST match on
// both halves. If the ping failure rate collapses, keep a slower speed (try 2 =
// 115200 next) as the fix; if it does not, the errors are not timing-related.
#define SELECT_SOFT_SERIAL_SPEED 3

// ── Mode B: flat ("dumb") layer + Flat† toggle — Corne only ───────────────────
// Host side + rationale: docs/MODE-B-KANATA.md. Runtime callbacks (persistent
// toggle, exit chord, LED indicator): users/manna_harbour_miryoku/vanos_flat.h.
// Everything here is layout data that must exist before the Miryoku engine's
// selection headers run, which is why it sits in config.h.
#define VANOS_FLAT_LAYER

// One more Miryoku layer. The engine derives U_FLAT, TD(U_TD_U_FLAT) and the
// keymaps[] slot from this list (miryoku_babel/miryoku_layer_list.h is guarded by
// #if !defined, so this replaces it without touching the vendored file).
#define MIRYOKU_LAYER_LIST \
MIRYOKU_X(BASE,   "Base") \
MIRYOKU_X(EXTRA,  "Extra") \
MIRYOKU_X(TAP,    "Tap") \
MIRYOKU_X(BUTTON, "Button") \
MIRYOKU_X(NAV,    "Nav") \
MIRYOKU_X(MOUSE,  "Mouse") \
MIRYOKU_X(MEDIA,  "Media") \
MIRYOKU_X(NUM,    "Num") \
MIRYOKU_X(SYM,    "Sym") \
MIRYOKU_X(FUN,    "Fun") \
MIRYOKU_X(FLAT,   "Flat")

// The flat layer is the wire protocol kanata reads (docs/kanata/layout.kbd header):
// the 30 alpha codes of Miryoku's Tap layer (q..p a..' z../) + F13–F18 on the thumbs (outer-left → outer-right)
// + F19–F21 / F22–F24 on the outer pinky columns (top → bottom). It uses the
// board's full 3x6_3_ex2 layout directly, not LAYOUT_miryoku, because it is the one
// layer where the outer pinky keys carry codes. The ex2 inner keys stay XXX.
#define MIRYOKU_LAYERMAPPING_FLAT LAYOUT_split_3x6_3_ex2
#define MIRYOKU_LAYER_FLAT \
KC_F19, KC_Q,   KC_W,   KC_E,   KC_R,   KC_T,   XXX,     XXX, KC_Y,   KC_U,   KC_I,    KC_O,   KC_P,    KC_F22, \
KC_F20, KC_A,   KC_S,   KC_D,   KC_F,   KC_G,   XXX,     XXX, KC_H,   KC_J,   KC_K,    KC_L,   KC_QUOT, KC_F23, \
KC_F21, KC_Z,   KC_X,   KC_C,   KC_V,   KC_B,                 KC_N,   KC_M,   KC_COMM, KC_DOT, KC_SLSH, KC_F24, \
                        KC_F13, KC_F14, KC_F15,               KC_F16, KC_F17, KC_F18

// Flat† entry key: hold Spc (Nav) + double-tap the LEFT outer-top pinky key, or
// hold Bsp (Num) + double-tap the RIGHT outer-top pinky key — one-handed per half,
// because on two USB cables each half must be switchable by itself. These two
// adapters are LAYOUT_miryoku (../manna_harbour_miryoku/config.h) with that one
// XXX replaced; the Miryoku layer tables themselves are untouched. Both keys are
// XXX on every other layer, so nothing else changes.
#define MIRYOKU_LAYERMAPPING_NAV(\
      K00,  K01,  K02,  K03,  K04,              K05,  K06,  K07,  K08,  K09,\
      K10,  K11,  K12,  K13,  K14,              K15,  K16,  K17,  K18,  K19,\
      K20,  K21,  K22,  K23,  K24,              K25,  K26,  K27,  K28,  K29,\
      N30,  N31,  K32,  K33,  K34,              K35,  K36,  K37,  N38,  N39\
)\
LAYOUT_split_3x6_3_ex2(\
TD(U_TD_U_FLAT), K00, K01, K02, K03, K04, XXX,  XXX,  K05,  K06,  K07,  K08,  K09,  XXX,\
XXX,  K10,  K11,  K12,  K13,  K14,  XXX,  XXX,  K15,  K16,  K17,  K18,  K19,  XXX,\
XXX,  K20,  K21,  K22,  K23,  K24,              K25,  K26,  K27,  K28,  K29,  XXX,\
                  K32,  K33,  K34,              K35,  K36,  K37\
)
#define MIRYOKU_LAYERMAPPING_NUM(\
      K00,  K01,  K02,  K03,  K04,              K05,  K06,  K07,  K08,  K09,\
      K10,  K11,  K12,  K13,  K14,              K15,  K16,  K17,  K18,  K19,\
      K20,  K21,  K22,  K23,  K24,              K25,  K26,  K27,  K28,  K29,\
      N30,  N31,  K32,  K33,  K34,              K35,  K36,  K37,  N38,  N39\
)\
LAYOUT_split_3x6_3_ex2(\
XXX,  K00,  K01,  K02,  K03,  K04,  XXX,  XXX,  K05,  K06,  K07,  K08,  K09,  TD(U_TD_U_FLAT),\
XXX,  K10,  K11,  K12,  K13,  K14,  XXX,  XXX,  K15,  K16,  K17,  K18,  K19,  XXX,\
XXX,  K20,  K21,  K22,  K23,  K24,              K25,  K26,  K27,  K28,  K29,  XXX,\
                  K32,  K33,  K34,              K35,  K36,  K37\
)
