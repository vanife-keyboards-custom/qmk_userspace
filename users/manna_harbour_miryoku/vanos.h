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

// ── Home-row-mod timing tune (docs/LAYOUT-RESEARCH.md, unified-layout/SPEC.md §3) ──
// miryoku:default ships TAPPING_TERM 200 with no decision-helper flags.

#undef TAPPING_TERM
#define TAPPING_TERM 250          // was 200

#define PERMISSIVE_HOLD           // hold resolves on a nested tap+release (faster, roll-safe)
#define FLOW_TAP_TERM 150         // suppress HRM hold during fast typing bursts

// CHORDAL_HOLD (opposite-hands rule — the biggest misfire reduction) is the next
// addition. On the split Corne it works from handedness automatically; on the
// unibody BM40 it needs a per-board `chordal_hold_layout` map, so it is added in a
// follow-up step (also watch BM40 flash: currently 94% full).
