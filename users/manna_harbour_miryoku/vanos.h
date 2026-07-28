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

#define PERMISSIVE_HOLD           // hold resolves on a nested tap+release (faster, roll-safe)
#define FLOW_TAP_TERM 150         // suppress HRM hold during fast typing bursts
#define CHORDAL_HOLD              // opposite-hands rule — same-hand rolls always tap

// ⚠ CHORDAL_HOLD is not optional here: PERMISSIVE_HOLD alone turns every same-hand
// roll into a mod (`just` → `Ust`, since Miryoku puts LSFT_T on the index finger),
// which is strictly worse than stock Miryoku's 200 + no flags. The three defines
// above are one recipe (Getreuer / urob "timeless" HRMs) — do not ship a subset.
//
// Handedness comes from `vanos_chordal_hold.h`, included by each board's
// keymaps/vanos/keymap.c, so both boards get the identical map (thumbs exempt).
