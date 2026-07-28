// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — restore Caps Lock on Shift + the Nav-layer CapsWord key.
//
// ╔═══════════════════════════════════════════════════════════════════════════╗
// ║  STATUS: NOT COMPILED IN — DISABLED ON OS LEVEL (2026-07-28)               ║
// ║                                                                           ║
// ║  Both keymap.c #includes are commented out. The code below is correct and  ║
// ║  was verified in a flashed build (process_record_user linked as a strong   ║
// ║  symbol, and `keyd monitor` is the way to confirm KC_CAPS on the wire) —   ║
// ║  but it changes nothing observable on this machine, because the desktop    ║
// ║  remaps the Caps Lock key away before any application sees it:             ║
// ║                                                                           ║
// ║      $ hyprctl getoption input:kb_options                                  ║
// ║      str: compose:caps                                                     ║
// ║                                                                           ║
// ║  Set by ~/.local/share/omarchy/default/hypr/input.conf:6 (sourced from     ║
// ║  hyprland.conf:11). XKB's compose:caps replaces Caps_Lock with Multi_key,  ║
// ║  so KC_CAPS starts a Compose sequence instead of toggling caps.            ║
// ║  ⚠ ~/.config/hypr/input.conf:6 has that line COMMENTED OUT — which does    ║
// ║  not disable it. The Omarchy default is sourced first (:11) and the user   ║
// ║  file (:18) only overrides when it actually assigns kb_options.            ║
// ║                                                                           ║
// ║  TO RE-ENABLE: move Compose off Caps Lock (docs/LATIN-INTL-RESEARCH.md §5  ║
// ║  recommends `kb_options = compose:ralt`, matching WinCompose on Windows),  ║
// ║  then uncomment the #include in both keymaps/vanos/keymap.c and reflash.   ║
// ╚═══════════════════════════════════════════════════════════════════════════╝
//
// Included ONLY by keyboards/*/keymaps/vanos/keymap.c (never by the stock
// manna_harbour_miryoku keymap — that build is the A/B reference and keeps
// upstream behaviour, bug included). Defines a weak QMK callback, so it must be
// included from exactly one .c per firmware build — same arrangement as
// vanos_chordal_hold.h and vanos_thumb_hold.h.
//
// ── THE BUG (upstream QMK, not ours) ────────────────────────────────────────
//
// Miryoku's only way to reach Caps Lock is a key override
// (manna_harbour_miryoku.c): Shift + CW_TOGG → KC_CAPS. There is no dedicated
// Caps Lock key anywhere in the layout.
//
// That override is unreachable on current QMK. In quantum/quantum.c the process
// chain runs
//
//     ... process_caps_word()  (line 384)   →   process_key_override()  (387)
//
// and process_caps_word() (quantum/process_keycode/process_caps_word.c:73)
// swallows the keycode unconditionally, before ever looking at the mods:
//
//     if (keycode == QK_CAPS_WORD_TOGGLE) {
//         if (record->event.pressed) { caps_word_toggle(); }
//         return false;                      // ← key override never runs
//     }
//
// So Shift+CW_TOGG just toggles Caps Word, exactly like a bare press.
// The ordering came in with qmk/qmk_firmware#21434 ("Allow key override to
// respect weak mods caused by caps word"), which moved process_caps_word ahead
// of process_key_override and silently killed Miryoku's Caps Lock. Symptom:
// "only Caps Word works, I can't get Caps Lock."
//
// ── THE FIX ─────────────────────────────────────────────────────────────────
//
// process_record_kb()/process_record_user() sits at quantum.c:355 — ahead of
// BOTH process_caps_word() and process_key_override() — so handling the
// combination here always wins, whatever order upstream settles on later. The
// stock key override is left in place (it is upstream code and costs nothing);
// it simply never gets the chance to fire.
//
// Shift is stripped from the report before the tap so the host sees a plain
// Caps Lock: some layouts and OS-level remaps treat Shift+CapsLock as a distinct
// binding. tap_code() already waits TAP_HOLD_CAPS_DELAY for KC_CAPS_LOCK
// (action.c:1060), which is what macOS needs to accept the press.

#pragma once

#include "quantum.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == CW_TOGG && record->event.pressed &&
        ((get_mods() | get_weak_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT)) {
        const uint8_t real = get_mods();
        const uint8_t weak = get_weak_mods();

        // Release Shift on the host, then tap a clean Caps Lock.
        del_oneshot_mods(MOD_MASK_SHIFT); // a one-shot Shift is consumed, as a key override would
        del_weak_mods(MOD_MASK_SHIFT);
        del_mods(MOD_MASK_SHIFT);
        send_keyboard_report();

        tap_code(KC_CAPS);

        // Shift is still physically held — put it back.
        set_weak_mods(weak);
        set_mods(real);
        send_keyboard_report();

        return false; // consumed: process_caps_word() never sees it
    }
    return true;
}
