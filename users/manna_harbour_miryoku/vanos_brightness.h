// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — screen brightness on the Media layer's volume keys, with Shift.
//
//   Media layer (hold the Esc thumb), plus Shift (left home row, F position):
//     Shift + Vol-   →  F14   brightness of all screens −5 %
//     Shift + Vol+   →  F15   brightness of all screens +5 %
//     Shift + Stop   →  F16   all screens to 50 %
//   Without Shift the three keys are unchanged.
//
// Same keys and the same host side as the Cheapino knob on its MEDIA layer
// (keyboards/cheapino/keymaps/vanos/vanos_encoder.h has the host table: Hyprland
// binds XF86Launch5/6/7 to cheapino-brightness-all, Windows binds F14–F16 in
// AutoHotkey). Nothing new to install on a machine that already has
// `mise run install:cheapino-host`.
//
// Mechanism: QMK key overrides. Miryoku already compiles the feature in (it cannot
// be switched off — docs/devices/bm40/size-matrix.md), so this costs only the three
// table entries. While an override is active QMK holds Shift back from the host, so
// the host sees a plain F14, not Shift+F14. One press = one step: the host binds do
// not repeat while the key is held.
//
// Why Stop and not Mute for 50 %: Omarchy binds Shift+Mute (switch audio output) and
// Shift+Play (switch media source); overriding either here would take that away.
//
// Included by manna_harbour_miryoku.c, only when vanos.h defines
// VANOS_BRIGHTNESS_OVERRIDES — the stock manna_harbour_miryoku keymap never does.

#pragma once

#ifndef VANOS_KEY_BRIGHT_DOWN
#    define VANOS_KEY_BRIGHT_DOWN KC_F14 // XF86Launch5 under XKB's evdev rules
#    define VANOS_KEY_BRIGHT_UP KC_F15   // XF86Launch6
#    define VANOS_KEY_BRIGHT_MID KC_F16  // XF86Launch7
#endif

const key_override_t vanos_bright_down_override = ko_make_basic(MOD_MASK_SHIFT, KC_VOLD, VANOS_KEY_BRIGHT_DOWN);
const key_override_t vanos_bright_up_override   = ko_make_basic(MOD_MASK_SHIFT, KC_VOLU, VANOS_KEY_BRIGHT_UP);
const key_override_t vanos_bright_mid_override  = ko_make_basic(MOD_MASK_SHIFT, KC_MSTP, VANOS_KEY_BRIGHT_MID);

// Entries for the key_overrides[] array, each with its trailing comma.
#define VANOS_BRIGHTNESS_OVERRIDE_LIST \
    &vanos_bright_down_override, \
    &vanos_bright_up_override, \
    &vanos_bright_mid_override,
