// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — Media layer with screen-brightness keys on its free bottom row.
//
// Same mechanism as vanos_mouse.h: defining MIRYOKU_LAYER_MEDIA before
// miryoku_babel/miryoku_layer_selection.h runs its `#if !defined(MIRYOKU_LAYER_MEDIA)`
// selection replaces the layer without editing a vendored file.
//
// The body is MIRYOKU_ALTERNATIVES_MEDIA_INVERTEDT (miryoku_layer_alternatives.h)
// copied verbatim, with three of the four U_NU ("available but not used") keys on the
// right hand's bottom row filled in. Right hand, as on the QWERTY base:
//
//     y        u        i        o        p
//     RGBtog   RGBmod   Vol+     RGBhue+  RGBsat+
//     h        j        k        l        '
//     ···      Prev     Vol-     Next     RGBval+
//     n        m        ,        .        /
//     OutAuto  Bri-     Bri50    Bri+     ···        <- new: M , .
//
// Bri-/Bri+ sit under Prev/Next, Bri50 under Vol-. No modifier needed: hold the Esc
// thumb, tap with the right hand. Additive: no existing position changes.
//
// The keys are F14/F15/F16, the ones the Cheapino knob already sends on its MEDIA
// layer (keyboards/cheapino/keymaps/vanos/vanos_encoder.h has the host table:
// Hyprland binds XF86Launch5/6/7 to cheapino-brightness-all, Windows binds F14–F16 in
// AutoHotkey). One press = one 5 % step; the host binds do not repeat on hold.
//
// ⚠ KNOWN LIMITATION (same as vanos_mouse.h): this hardcodes the INVERTEDT Nav
// variant and does not follow MIRYOKU_NAV / MIRYOKU_LAYERS=FLIP.

#pragma once

#define VANOS_KEY_BRIGHT_DOWN KC_F14 // XF86Launch5 under XKB's evdev rules
#define VANOS_KEY_BRIGHT_UP KC_F15   // XF86Launch6
#define VANOS_KEY_BRIGHT_MID KC_F16  // XF86Launch7

#define MIRYOKU_LAYER_MEDIA \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   U_NA,              RGB_TOG,           RGB_MOD,           KC_VOLU,           RGB_HUI,           RGB_SAI,           \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,              U_NU,              KC_MPRV,           KC_VOLD,           KC_MNXT,           RGB_VAI,           \
U_NA,              KC_ALGR,           TD(U_TD_U_FUN),    TD(U_TD_U_MEDIA),  U_NA,              OU_AUTO,           VANOS_KEY_BRIGHT_DOWN, VANOS_KEY_BRIGHT_MID, VANOS_KEY_BRIGHT_UP, U_NU,       \
U_NP,              U_NP,              U_NA,              U_NA,              U_NA,              KC_MSTP,           KC_MPLY,           KC_MUTE,           U_NP,              U_NP
