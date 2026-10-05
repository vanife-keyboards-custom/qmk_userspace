// Copyright 2026 Ivan Melnychuk (@vanife)
// SPDX-License-Identifier: GPL-2.0-or-later
// Based on Miryoku by Manna Harbour — https://github.com/manna-harbour/miryoku

// No layers here: the keymap comes from the Miryoku engine
// (users/manna_harbour_miryoku/manna_harbour_miryoku.c, wired via USER_NAME in
// rules.mk). This file exists only to carry the shared CHORDAL_HOLD handedness map,
// which must live in a .c and uses this board's LAYOUT_miryoku from config.h.
//
// The Corne is split, so QMK could guess handedness from geometry — but we use the
// same explicit map as the BM40 so the two boards behave identically (in particular
// the '*' thumbs, which a geometry guess would mark 'L'/'R').

#include QMK_KEYBOARD_H
#include "vanos_chordal_hold.h"
#include "vanos_thumb_hold.h"      // thumb tap-hold behaviour (Flow Tap exemption)
#include "vanos_permissive_hold.h" // per-key Permissive Hold (bug 001: word-initial `la`)
#include "vanos_split_watchdog.h"  // slave half reboots itself if the master goes quiet
                                   // (split boards only — inert without SPLIT_KEYBOARD)
// DISABLED ON OS LEVEL — Caps Lock is remapped away by the desktop, so the firmware
// fix has no observable effect on this machine. Hyprland ships `kb_options =
// compose:caps` (~/.local/share/omarchy/default/hypr/input.conf:6), which XKB turns
// into Multi_key: KC_CAPS never reaches an application as Caps Lock. Re-enable this
// include the moment Compose moves off Caps Lock. See docs/UNIFIED-LAYOUT.md §2.
//#include "vanos_caps_lock.h"    // Shift + CapsWord key -> Caps Lock (upstream override is dead)

// ── DIAGNOSTIC (see rules.mk CONSOLE_ENABLE): split-link self-report ─────────
// 2026-09-23, "slave half dead after hot-plugging TRRS cables". Two probes:
//
//  1. Once a second the USB half prints, via uprintf (NOT gated on debug_enable):
//       role, hand, whether split_common currently considers the other half
//       reachable, and every matrix row in hex (rows 0-3 = left, 4-7 = right).
//     link=0 forever  → no reply on the serial line at all (cable / jack / GP12).
//     link=1 but the far half's rows stay 00 under keypresses → link up, matrix
//     state not arriving.
//  2. Both halves light all LEDs solid at boot (master green, slave red). The
//     slave runs this on its own, so a LIT but silent half = powered and running
//     firmware, only the data line is dead; a DARK slave = no power over the
//     cable (VCC/GND) or no boot.
//
// matrix_init_user/matrix_scan_user are used because keyboard_post_init_user and
// housekeeping_task_user are owned by vanos_split_watchdog.h.
#ifdef CONSOLE_ENABLE
#    include "split_util.h"
#    include "transactions.h"
#    include "version.h"      // QMK_BUILDDATE for the log tag
// Serial speed as the log shows it. The real SERIAL_USART_SPEED lives in the
// driver header, out of reach here; SELECT_SOFT_SERIAL_SPEED is what we set.
#    ifndef SELECT_SOFT_SERIAL_SPEED
#        define SELECT_SOFT_SERIAL_SPEED 1
#    endif
#    define VANOS_DIAG_SPEED (SELECT_SOFT_SERIAL_SPEED == 0 ? 460800 : SELECT_SOFT_SERIAL_SPEED == 1 ? 230400 : SELECT_SOFT_SERIAL_SPEED == 2 ? 115200 : SELECT_SOFT_SERIAL_SPEED == 3 ? 57600 : SELECT_SOFT_SERIAL_SPEED == 4 ? 38400 : 19200)

void matrix_init_user(void) {
    debug_enable = true;
    debug_matrix = true;
}

//  3. Ping probe (only when the real watchdog is compiled out, so the two never
//     double up): the master sends the same RPC the watchdog uses, every 100 ms,
//     and counts ok/fail plus the longest gap between two successful pings. The
//     watchdog reboots the slave at a 1000 ms gap — so maxgap tells you directly
//     whether it was firing on a healthy link. An RPC is FOUR un-retried serial
//     transactions (transport.c → soft_serial_transaction, no retry loop), while
//     the matrix sync retries ten times, so pings can fail while typing is fine.
//  4. Slave self-report in the ping reply: its own uptime and the RP2040's
//     last-reset cause (CHIP_RESET register). The master counts a reset whenever
//     the slave's uptime goes backwards and prints the cause:
//       POR  = power-on / brown-out  → the 3.3 V through the cable sagged
//       RUN  = RUN pin               → reset button / RUN line glitch
//       PSM  = software / watchdog   → firmware reset itself
#    ifndef VANOS_SPLIT_WATCHDOG
#        include "hardware/structs/vreg_and_chip_reset.h"
typedef struct __attribute__((packed)) {
    uint32_t up_ms;
    uint8_t  reset; // bit0 POR, bit1 RUN, bit2 PSM
} diag_pong_t;

static void diag_ping_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    uint32_t    cr   = vreg_and_chip_reset_hw->chip_reset;
    diag_pong_t pong = {
        .up_ms = timer_read32(),
        .reset = ((cr & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_POR_BITS) ? 1 : 0) | ((cr & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_BITS) ? 2 : 0) | ((cr & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_BITS) ? 4 : 0),
    };
    if (out_buflen >= sizeof(pong)) memcpy(out_data, &pong, sizeof(pong));
}
void keyboard_post_init_user(void) {
    transaction_register_rpc(VANOS_RPC_WATCHDOG, diag_ping_handler);
}
#    endif

void matrix_scan_user(void) {
    static uint32_t last_report = 0;
    static bool     leds_done   = false;

    if (!leds_done && timer_read32() > 1000) { // rgb_matrix_init() has run by now
        leds_done = true;
#    ifdef RGB_MATRIX_ENABLE
        // Sign-of-life, DIM (val 8 ≈ 1/30 of the earlier brightness, ~2 mA per
        // half): master green, slave red on its own; the slave turns green only
        // once the master's RGB sync reaches it. So on the far half:
        //   dark  = no power arriving through the cable (VCC/GND contact)
        //   red   = powered and running, data line dead
        //   green = link up
        rgb_matrix_enable_noeeprom();
        rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv_noeeprom(is_keyboard_master() ? 85 : 0, 255, 8);
#    endif
    }

    if (!is_keyboard_master()) return;

    // Count every link loss, even ones shorter than the 1 s report period:
    // a marginal jack shows up as a drops counter that climbs while you touch it.
    static bool     link_prev = false;
    static uint16_t drops     = 0;
    bool            link_now  = is_transport_connected();
    if (link_prev && !link_now) drops++;
    link_prev = link_now;

    static uint16_t ping_ok = 0, ping_fail = 0;
    static uint32_t max_gap = 0;
    static uint32_t slave_up = 0;
    static uint16_t slave_resets = 0;
    static uint8_t  slave_reset_cause = 0;
#    ifndef VANOS_SPLIT_WATCHDOG
    static uint32_t last_ping = 0, last_ok = 0;
    if (timer_elapsed32(last_ping) >= 100) {
        last_ping    = timer_read32();
        uint8_t     ping = 0;
        diag_pong_t pong = {0};
        if (transaction_rpc_exec(VANOS_RPC_WATCHDOG, sizeof(ping), &ping, sizeof(pong), &pong)) {
            if (last_ok != 0) {
                uint32_t gap = timer_elapsed32(last_ok);
                if (gap > max_gap) max_gap = gap;
            }
            last_ok = timer_read32();
            ping_ok++;
            if (slave_up != 0 && pong.up_ms < slave_up) slave_resets++; // uptime went backwards = slave rebooted
            slave_up          = pong.up_ms;
            slave_reset_cause = pong.reset;
        } else {
            ping_fail++;
        }
    }
#    endif

    if (timer_elapsed32(last_report) < 1000) return;
    last_report = timer_read32();

    // Build identity, so a pasted log can never be mistaken for another build.
    static const char *my_cause = NULL;
    if (my_cause == NULL) {
        uint32_t cr = vreg_and_chip_reset_hw->chip_reset;
        my_cause    = (cr & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_PSM_RESTART_BITS) ? "PSM" : (cr & VREG_AND_CHIP_RESET_CHIP_RESET_HAD_RUN_BITS) ? "RUN" : "POR";
    }
    uprintf("fw=%s baud=%u wd=%s my_cause=%s master=%u left=%u link=%u drops=%u ping=%u/%u maxgap=%lums up=%lus slave_up=%lus slave_resets=%u cause=%s%s%s rows:", QMK_BUILDDATE, (unsigned)VANOS_DIAG_SPEED,
#    ifdef VANOS_SPLIT_WATCHDOG
            "on",
#    else
            "off",
#    endif
            my_cause, is_keyboard_master(), is_keyboard_left(), link_now, drops, ping_ok, ping_fail, max_gap, timer_read32() / 1000, slave_up / 1000, slave_resets, (slave_reset_cause & 1) ? "POR" : "", (slave_reset_cause & 2) ? "RUN" : "", (slave_reset_cause & 4) ? "PSM" : "");
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        uprintf(" %02X", (unsigned)matrix_get_row(r));
    }
    uprintf("\n");
}
#endif
