// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — Base layer override: one-shot Shift on the left inner thumb
// (docs/FEATURE-BACKLOG.md P1-B, docs/SHIFT-RESEARCH.md Option B). Shared by
// every vanos board.
//
//   Tab•Mou  →  OSSft•Mou
//
//   tap                       → one-shot Shift: the NEXT key is shifted, then it
//                               clears itself (camelCase: `camel` tap `case`).
//   tap again while pending   → Caps Word (cancels the pending one-shot).
//   tap while Caps Word is on → Caps Word off.
//   hold                      → Mouse layer, exactly as before.
//   Tab                       → still on the Fun layer: hold Del•Fun + this thumb.
//
// The home-row Shifts on F / J are untouched; this is an addition, not a swap.
//
// ── Why a Base-layer override plus a process_record hook ──────────────────────
//
// LT() only takes a basic keycode as its tap, so LT(U_MOUSE, OSM(MOD_LSFT)) does
// not exist. The key is therefore LT(U_MOUSE, KC_LSFT): KC_LSFT is a placeholder
// that is never sent and makes the intent readable in the keymap; the tap is
// intercepted by vanos_oneshot_shift_process_record() (vanos_oneshot_shift.h)
// and turned into a one-shot. The hold is never intercepted, so the Mouse layer
// is QMK's own LT.
//
// Everything keyed off "is a thumb" keeps working without edits: the key is still
// an LT() on a layer other than U_BUTTON (vanos_keys.h), so the Flow Tap
// exemption, per-key Permissive Hold, Chordal Hold ('*' on thumbs) and the BM40
// held-layer LED all see it exactly as they saw LT(U_MOUSE, KC_TAB).
//
// Miryoku picks MIRYOKU_LAYER_BASE in miryoku_babel/miryoku_layer_selection.h,
// guarded by `#if !defined(MIRYOKU_LAYER_BASE)`, which is the same mechanism as
// vanos_mouse.h. The body is MIRYOKU_ALTERNATIVES_BASE_QWERTY verbatim except for
// the one thumb. The board mappings (incl. the BM40 rot180) only rearrange this
// 4×10 table, so every board picks it up.
//
// ⚠ KNOWN LIMITATION (as vanos_mouse.h): this hardcodes QWERTY, non-flipped. If
// MIRYOKU_ALPHAS or MIRYOKU_LAYERS ever change in custom_rules.mk, the #error
// below stops the build instead of silently forcing QWERTY.
//
// Flash: one-shot mods are core QMK (no feature flag; NO_ACTION_ONESHOT is not
// set), Caps Word is already compiled in. The only new code is the hook.
//
// This file is included from vanos.h (i.e. from config.h), so it holds #defines only.

#pragma once

#if defined(MIRYOKU_ALPHAS_COLEMAKDH) || defined(MIRYOKU_ALPHAS_COLEMAK) || defined(MIRYOKU_ALPHAS_COLEMAKDHK) || \
    defined(MIRYOKU_ALPHAS_AZERTY) || defined(MIRYOKU_ALPHAS_DVORAK) || defined(MIRYOKU_ALPHAS_HALMAK) ||          \
    defined(MIRYOKU_ALPHAS_WORKMAN) || defined(MIRYOKU_ALPHAS_QWERTZ) || defined(MIRYOKU_ALPHAS_BEAKL15) ||        \
    defined(MIRYOKU_LAYERS_FLIP)
#    error "vanos_oneshot_layers.h hardcodes the QWERTY non-flipped Base layer; update it for the new MIRYOKU_ALPHAS/LAYERS"
#endif

// The thumb key. One name, used by the layer table and by the hook.
#define VANOS_OSSFT_MOU LT(U_MOUSE, KC_LSFT)

// A pending one-shot Shift expires after 2 s if no key follows (QMK default:
// never). Same value as the kanata mirror, (one-shot-press 2000 lsft).
#ifndef ONESHOT_TIMEOUT
#    define ONESHOT_TIMEOUT 2000
#endif

#define MIRYOKU_LAYER_BASE \
KC_Q,              KC_W,              KC_E,              KC_R,              KC_T,              KC_Y,              KC_U,              KC_I,              KC_O,              KC_P,              \
LGUI_T(KC_A),      LALT_T(KC_S),      LCTL_T(KC_D),      LSFT_T(KC_F),      KC_G,              KC_H,              LSFT_T(KC_J),      LCTL_T(KC_K),      LALT_T(KC_L),      LGUI_T(KC_QUOT),   \
LT(U_BUTTON,KC_Z), ALGR_T(KC_X),      KC_C,              KC_V,              KC_B,              KC_N,              KC_M,              KC_COMM,           ALGR_T(KC_DOT),    LT(U_BUTTON,KC_SLSH),\
U_NP,              U_NP,              LT(U_MEDIA,KC_ESC),LT(U_NAV,KC_SPC),  VANOS_OSSFT_MOU,   LT(U_SYM,KC_ENT),  LT(U_NUM,KC_BSPC), LT(U_FUN,KC_DEL),  U_NP,              U_NP

// ── On/off toggle key, Media layer (hold Esc•Med + right-hand H) ─────────────
//
// VANOS_OSSFT_TOGG switches the thumb between one-shot Shift (default) and stock
// Tab•Mou. Stored in EEPROM (vanos_oneshot_shift.h), so it survives a replug.
// It sits on the Media layer's only free home-row slot (U_NU at H), mirroring
// Caps Word at Nav + H. The Media layer itself is defined once, in vanos_media.h
// (it also carries the brightness keys); vanos.h includes that file after this
// one, so VANOS_OSSFT_TOGG is defined by the time the layer is.
//
// QK_USER_1: QK_USER_0 is taken by the Cheapino encoder push (VANOS_ENC_PUSH).
#define VANOS_OSSFT_TOGG QK_USER_1
