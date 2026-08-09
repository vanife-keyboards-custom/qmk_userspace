// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — per-key Permissive Hold, shared by both boards.
//
// Included ONLY by keyboards/*/keymaps/vanos/keymap.c (never by the stock
// manna_harbour_miryoku keymap). This is a definition of a QMK weak callback, so it
// must be included from exactly one .c per firmware build — which is the case: each
// build compiles only its own keymap.c. Same arrangement as vanos_thumb_hold.h.
//
// Enabled by PERMISSIVE_HOLD_PER_KEY in vanos.h. ⚠ That define REPLACES the bare
// PERMISSIVE_HOLD rather than refining it: quantum/action_tapping.c:233-239 resolves
// TAP_GET_PERMISSIVE_HOLD as `#ifdef PER_KEY → callback / #elif PERMISSIVE_HOLD →
// true`. If this header is ever dropped without also removing the PER_KEY define,
// the weak default returns false and Permissive Hold is off EVERYWHERE, silently
// breaking fast thumb layer access.
//
// ── WHY: bug 001 (docs/bugs/001-word-initial-hrm-alt-misfire.md) ─────────────
//
// Word-initial `la` was being swallowed. `l` is LALT_T(KC_L) (right ring), `a` is
// LGUI_T(KC_A) (left pinky). On a nested roll — a released before l — Permissive
// Hold settles `l` as a HOLD (action_tapping.c:371-403), so `l` is never typed and
// `a` is emitted as Alt+a, which is inert in Hyprland and readline. Both characters
// vanish; `lazygit` becomes `zygit`.
//
// Neither existing mitigation can catch it, and this is the important part:
//   • CHORDAL_HOLD abstains by design — l and a are OPPOSITE hands, and the
//     opposite-hands rule only suppresses *same-hand* rolls (:348).
//   • FLOW_TAP_TERM cannot protect the FIRST key of a burst: it needs a preceding
//     flow-tap key within the term. KC_SPC is in QMK's list (:1082-1095) so `l` is
//     safe mid-prose, but KC_ENT is not — so `l` typed at a shell prompt right
//     after Enter is naked. Worse, once Permissive Hold has settled `l` as a hold,
//     the buffered-key Flow Tap pass (:324-342) is never reached, because it lives
//     on the released-first branch.
//
// So the trigger is Permissive Hold itself, and the fix has to be here.
//
// ── THE POLICY ───────────────────────────────────────────────────────────────
//
// Same line the Flow Tap callback already draws (vanos_thumb_hold.h): the fast path
// is for keys genuinely needed *mid-burst*; everything else can wait out
// TAPPING_TERM.
//
//   thumbs  → true.  Layer access must not require a 200 ms dwell, and thumbs do
//                    not misfire this way — they are slow and deliberate, and they
//                    are '*' (Chordal-Hold-exempt) in vanos_chordal_hold.h, so
//                    Permissive Hold is currently the ONLY thing making a fast
//                    thumb roll reach its layer. The proper fix for thumbs is
//                    per-key HOLD_ON_OTHER_KEY_PRESS (docs/LAYOUT-RESEARCH.md:184-209),
//                    still unimplemented — do not remove this branch before that lands.
//   Shift   → true.  Needed *during* a burst constantly: sentence capitals,
//                    CamelCase, ! and ?. Same exemption, same reasoning, as Flow Tap.
//   rest    → false. Gui/Alt/Ctrl HRMs, AltGr on X and ., and the Button layer-taps
//                    on Z and /. Gui/Alt/Ctrl mid-burst is exactly what we do not
//                    want — that is the premise of Flow Tap — so nothing of value is
//                    lost. Z is deliberately included: it is the third consecutive
//                    tap-hold key in `lazygit`.
//
// COST: a deliberate Gui/Alt/Ctrl chord must now dwell past TAPPING_TERM (200 ms)
// before the other key is pressed, instead of resolving on that key's release. That
// is stock Miryoku behaviour, which this user ran for months (vanos.h:18-29).
// If 200 ms proves too tight, raise TAPPING_TERM in a SEPARATE change — and pin
// AUTO_SHIFT_TIMEOUT explicitly when doing so, per the warning in vanos.h.

#pragma once

#include "quantum.h"    // keyrecord_t, get_permissive_hold() signature
#include "vanos_keys.h" // vanos_is_thumb_key(), vanos_is_shift_mod_tap()

#ifdef PERMISSIVE_HOLD_PER_KEY

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    return vanos_is_thumb_key(keycode) || vanos_is_shift_mod_tap(keycode);
}

#endif // PERMISSIVE_HOLD_PER_KEY
