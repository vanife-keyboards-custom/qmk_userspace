// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// vanos — re-armable slave watchdog for split boards (Corne v4 only today).
//
// Included ONLY by keyboards/*/keymaps/vanos/keymap.c (never by the stock
// manna_harbour_miryoku keymap). These are definitions of QMK's weak callbacks, so
// this header must be included from exactly one .c per firmware build — which is the
// case: each build compiles only its own keymap.c. Same arrangement as
// vanos_chordal_hold.h and vanos_thumb_hold.h.
//
// Inert unless SPLIT_KEYBOARD, so including it from the BM40 keymap would compile to
// nothing. Kill switch: comment out VANOS_SPLIT_WATCHDOG in the board's keymap
// config.h — everything below disappears and stock QMK behaviour returns.
//
// ── THE PROBLEM ──────────────────────────────────────────────────────────────
//
// Symptom: after switching the monitor's KVM, the Corne's right half is dead until
// the USB cable is physically unplugged and replugged. The left half keeps working.
// See docs/devices/corne-v4.md § "Right half dies after a KVM switch".
//
// QMK already has SPLIT_WATCHDOG_ENABLE, and crkbd/rev4_1 turns it on
// (keyboards/crkbd/rev4_1/info.json:77 → split.transport.watchdog). It does NOT
// cover this case: as implemented in QMK 4ffb1ab16c (0.33.11) it is a BOOT-RACE
// GUARD ONLY, verified in the pinned tree —
//
//   * split_util.c:113  split_watchdog_task() resets the slave only while
//                       !split_watchdog_done, and split_watchdog_started is stamped
//                       once in split_post_init and never re-stamped.
//   * transactions.c:792  the master sends PUT_WATCHDOG only until the first
//                       success, so once the halves have talked the slave's `done`
//                       flag is stuck true and can never go false again.
//   * split_util.c:246  the slave's own is_transport_connected() is always true —
//                       connection_errors is only ever incremented master-side.
//
// So a slave that connects once and *then* loses the link is unrecoverable in
// firmware, which is exactly why only a power cycle fixes it. (QMK's docs read as if
// the watchdog were continuous; the implementation is not. Re-check this comment when
// the qmk_firmware pin moves — if upstream makes it re-armable, delete this file.)
//
// ── THE FIX ──────────────────────────────────────────────────────────────────
//
// A ping the master repeats forever, and a slave that reboots itself when the pings
// stop. mcu_reset() on RP2040 is a plain NVIC_SystemReset()
// (platforms/chibios/bootloaders/uf2boot.c:20) — a firmware restart, NOT a jump to
// the RPI-RP2 bootloader, so a spurious reset costs a reboot, not a dead keyboard.
//
// Division of labour with the built-in watchdog, which stays enabled:
//   boot        — the built-in one (3000 ms, one shot) handles "never connected".
//   steady state — this one (1000 ms, repeating) handles "connected, then lost".
// The boot grace below is deliberately longer than the built-in timeout so the two
// never race for the same reset.
//
// Runs on both halves out of one binary: the master branch pings, the slave branch
// watches. housekeeping_task_user() is called from the shared main loop
// (quantum/main.c:75 → keyboard.c:422), so it runs on the slave too — the same
// premise the built-in slave watchdog relies on (keyboard.c:715).
//
// ⚠ Known behaviour, not a bug: if the inter-half cable is genuinely broken or
// unplugged, the slave will reboot every VANOS_SPLIT_WATCHDOG_BOOT_GRACE ms forever
// (each reboot clears `linked`, so the slow grace period applies, not the 1 s
// steady-state timeout). That is the intended cost of automatic recovery — a
// keyboard that heals itself in ~1 s in the case we care about. A single half in
// use on its own is unaffected: whichever half holds the cable is the master, and
// the master branch never resets anything.

#pragma once

#if defined(SPLIT_KEYBOARD) && defined(VANOS_SPLIT_WATCHDOG)

#    include "quantum.h"
#    include "transactions.h" // transaction_register_rpc, transaction_rpc_send
#    include "bootloader.h"   // mcu_reset

// How often the master pings. One tiny split transaction per interval; at 100 ms
// that is ~1 % of the transactions the matrix sync already performs.
#    ifndef VANOS_SPLIT_WATCHDOG_PING_INTERVAL
#        define VANOS_SPLIT_WATCHDOG_PING_INTERVAL 100
#    endif

// Steady-state: how long the slave tolerates silence before rebooting. 1000 ms = 10
// missed pings, so a momentary hiccup or a busy master cannot trigger a reset, and
// recovery still lands well inside "I pressed a key and nothing happened".
#    ifndef VANOS_SPLIT_WATCHDOG_TIMEOUT
#        define VANOS_SPLIT_WATCHDOG_TIMEOUT 1000
#    endif

// Boot: how long the slave waits for the master's FIRST ping. Must stay above the
// built-in SPLIT_WATCHDOG_TIMEOUT (3000 ms, split_util.c:87) so the built-in guard
// owns the boot race — and above the worst case where the master half is sitting in
// its bootloader being flashed while the slave is powered through the link.
#    ifndef VANOS_SPLIT_WATCHDOG_BOOT_GRACE
#        define VANOS_SPLIT_WATCHDOG_BOOT_GRACE 5000
#    endif

// Optional escalation, OFF by default — see docs/devices/corne-v4.md for when to
// turn it on. The software watchdog above can only fire if the slave's main loop is
// still running. If the right half turns out to hang outright (loop stopped, e.g.
// stalled in the PIO serial driver or brown-out-stunned), nothing in software can
// notice; the RP2040's hardware watchdog can. pico-sdk's watchdog_enable() resets
// every peripheral except ROSC/XOSC (hardware_watchdog/watchdog.c:40), i.e. a clean
// boot, and the SDK unit is compiled into every RP2040 build already
// (platforms/chibios/vendors/RP/RP2040.mk:32).
//
// ⚠ Why it is not on by default: it reboots the half whenever the main loop stalls
// for ANY reason, including legitimate ones (a flash/wear-levelling erase can block
// for tens of ms, and QMK gives no guarantee about the worst case). Hence the
// deliberately loose 3000 ms — do not tighten it without measuring.
#    ifdef VANOS_SPLIT_WATCHDOG_HW
#        include "hardware/watchdog.h"
#        ifndef VANOS_SPLIT_WATCHDOG_HW_TIMEOUT
#            define VANOS_SPLIT_WATCHDOG_HW_TIMEOUT 3000
#        endif
#    endif

// Slave-side state. Written in the RPC callback, read in housekeeping — both run in
// the same main-loop context, so no synchronisation is needed.
static uint32_t vanos_wd_last_heard = 0;     // when the master last spoke
static bool     vanos_wd_linked     = false; // has the link been up since this boot?

// Runs on the slave when the master's ping lands. The payload is irrelevant —
// arrival is the signal.
void vanos_split_watchdog_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    vanos_wd_last_heard = timer_read32();
    vanos_wd_linked     = true;
}

void keyboard_post_init_user(void) {
    // Registered on both halves: harmless on the master, required on the slave.
    transaction_register_rpc(VANOS_RPC_WATCHDOG, vanos_split_watchdog_handler);
    vanos_wd_last_heard = timer_read32();

#    ifdef VANOS_SPLIT_WATCHDOG_HW
    if (!is_keyboard_master()) {
        watchdog_enable(VANOS_SPLIT_WATCHDOG_HW_TIMEOUT, false);
    }
#    endif
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        static uint32_t last_ping = 0; // 0 => elapsed is huge => ping on the first pass
        if (timer_elapsed32(last_ping) >= VANOS_SPLIT_WATCHDOG_PING_INTERVAL) {
            last_ping = timer_read32();
            // Failure is expected and ignored: a dead link is the slave's problem to
            // fix by rebooting, and the master must keep working solo either way.
            uint8_t ping = 0;
            transaction_rpc_send(VANOS_RPC_WATCHDOG, sizeof(ping), &ping);
        }
        return;
    }

    // ── slave ──
#    ifdef VANOS_SPLIT_WATCHDOG_HW
    watchdog_update(); // feed it while the loop is healthy
#    endif

    const uint32_t limit = vanos_wd_linked ? VANOS_SPLIT_WATCHDOG_TIMEOUT : VANOS_SPLIT_WATCHDOG_BOOT_GRACE;
    if (timer_elapsed32(vanos_wd_last_heard) > limit) {
        mcu_reset();
    }
}

#endif // SPLIT_KEYBOARD && VANOS_SPLIT_WATCHDOG
