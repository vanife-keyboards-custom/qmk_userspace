// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — shared customizations layered on top of miryoku:default.
//
// Included ONLY by the `vanos` keymaps (both boards) via their config.h.
// miryoku:default (the stock `manna_harbour_miryoku` keymap) never includes this,
// so it stays pristine. This file is the single source of truth for vanos
// tweaks, so BM40 and Corne stay in sync.
//
// Loaded from the keymap config.h, which QMK includes AFTER the userspace config.h
// (build_keyboard.mk: user config @431, keymap config @459) — so the #undef/#define
// below reliably override Miryoku's defaults.

#pragma once

// ── Home-row-mod timing tune (docs/LAYOUT-RESEARCH.md, docs/UNIFIED-LAYOUT.md §3) ──
// miryoku:default ships TAPPING_TERM 200 with no decision-helper flags.
//
// The tune is the three flags below, NOT the term. QMK docs and Getreuer pair the
// recipe with 250, but that number is calibrated for someone adopting HRMs cold;
// this user has months of trained feel at 200, and the term is nearly inert during
// typing anyway — same-hand rolls are decided by CHORDAL_HOLD, nested cross-hand
// rolls by PERMISSIVE_HOLD, streak keys by FLOW_TAP, all regardless of its value.
// It only governs the hold-alone arm time and the lingering-finger case, which is
// exactly the trained feel worth preserving. So: keep 200, change one thing at a time.
// (250 also silently slowed Auto Shift, since stock's config.h:22 defines
// AUTO_SHIFT_TIMEOUT as TAPPING_TERM and AUTO_SHIFT_ENABLE = yes.)

#undef TAPPING_TERM
#define TAPPING_TERM 200          // = stock Miryoku, deliberately unchanged

#define PERMISSIVE_HOLD_PER_KEY   // hold resolves on a nested tap+release — thumbs/Shift only
#define FLOW_TAP_TERM 150         // suppress HRM hold during fast typing bursts
#define CHORDAL_HOLD              // opposite-hands rule — same-hand rolls always tap

// ⚠ CHORDAL_HOLD is not optional here: permissive hold alone turns every same-hand
// roll into a mod (`just` → `Ust`, since Miryoku puts LSFT_T on the index finger),
// which is strictly worse than stock Miryoku's 200 + no flags. The three defines
// above are one recipe (Getreuer / urob "timeless" HRMs) — do not ship a subset.
//
// PERMISSIVE_HOLD_PER_KEY, not bare PERMISSIVE_HOLD, since 2026-07-30 — bug 001
// (docs/bugs/001-word-initial-hrm-alt-misfire.md): the bare flag made word-initial
// `la` misfire as Alt and swallowed both letters (`lazygit` → `zygit`). Neither
// CHORDAL_HOLD nor FLOW_TAP can catch that case; see the header for the trace.
// The policy — fast path for thumbs and Shift only — lives in
// `vanos_permissive_hold.h`, included by each board's keymaps/vanos/keymap.c.
// ⚠ PER_KEY *replaces* the bare flag (action_tapping.c:233-239); dropping that
// header without removing this define turns Permissive Hold off everywhere.
//
// Handedness comes from `vanos_chordal_hold.h`, included by each board's
// keymaps/vanos/keymap.c, so both boards get the identical map (thumbs exempt).

// ── Mouse cursor precision tune (docs/POINTING-DEVICE-NAVIGATION.md) ─────────
// Step size of the very first cursor tick (stock Miryoku/QMK default: 8) —
// shrunk for more precise landings. Try lower (2, then 1) if 4 still overshoots
// small targets; each step down roughly halves both the first jump and the
// ramped-up cruise speed. Lives here, not in the shared config.h, because the
// stock `manna_harbour_miryoku` reference keymap shares that file and should
// stay byte-identical to upstream Miryoku's own mousekey tuning.
#undef MOUSEKEY_MOVE_DELTA
#define MOUSEKEY_MOVE_DELTA 4

// ── Mouse layer acceleration override (T/G/B → MS_ACL2/MS_ACL1/MS_ACL0) ──────
// Must define MIRYOKU_LAYER_MOUSE before miryoku_layer_selection.h runs its own
// `#if !defined(MIRYOKU_LAYER_MOUSE)` selection — see vanos_mouse.h for why this
// lives in its own file instead of inline here.
#include "vanos_mouse.h"
