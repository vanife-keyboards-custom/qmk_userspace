// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// No layers here: the keymap comes from the Miryoku engine
// (users/manna_harbour_miryoku/manna_harbour_miryoku.c, wired via USER_NAME in
// rules.mk). This file exists only to carry the shared CHORDAL_HOLD handedness map,
// which must live in a .c and uses this board's LAYOUT_miryoku from config.h.

#include QMK_KEYBOARD_H
#include "vanos_chordal_hold.h"
#include "vanos_thumb_hold.h"      // thumb tap-hold behaviour (Flow Tap exemption)
#include "vanos_permissive_hold.h" // per-key Permissive Hold (bug 001: word-initial `la`)
#include "vanos_oneshot_shift.h"   // Tab•Mou thumb tap → one-shot Shift / Caps Word (vanos_oneshot_layers.h)

// Shared by vanos_rot180, which #includes this file. If vanos_caps_lock.h is ever
// re-enabled, merge its process_record_user into this one.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return vanos_oneshot_shift_process_record(keycode, record);
}

// DISABLED ON OS LEVEL — Caps Lock is remapped away by the desktop, so the firmware
// fix has no observable effect on this machine. Hyprland ships `kb_options =
// compose:caps` (~/.local/share/omarchy/default/hypr/input.conf:6), which XKB turns
// into Multi_key: KC_CAPS never reaches an application as Caps Lock. Re-enable this
// include the moment Compose moves off Caps Lock. See docs/UNIFIED-LAYOUT.md §2.
//#include "vanos_caps_lock.h"    // Shift + CapsWord key -> Caps Lock (upstream override is dead)
