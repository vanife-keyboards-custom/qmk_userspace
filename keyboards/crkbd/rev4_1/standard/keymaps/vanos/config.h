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
#define VANOS_SPLIT_WATCHDOG
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
