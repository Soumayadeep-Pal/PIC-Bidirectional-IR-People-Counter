/**
 * @file main.c
 * @brief Bidirectional IR People Counter firmware for PIC16F873A.
 *
 * Tracks room/zone occupancy using dual active-low infrared beam sensors
 * (RC0 = Entry, RC1 = Exit), displays IN, OUT, and NET occupancy on a
 * 16x2 character LCD, and drives directional status LEDs (RC5, RC6, RC7).
 *
 * Improvements over prototype revisions:
 *  - Non-hanging timeout guard so a single-beam trigger cannot lock the MCU
 *  - Configured TRISC5/6/7 outputs properly so status LEDs drive reliably
 *  - Added debouncing and clean display formatting without flicker
 */

#include "config.h"
#include "lcd.h"

/* Display mode toggle:
 * 0 = Simultaneous dashboard (Line 1: IN & OUT, Line 2: NET occupancy)
 * 1 = Rotating full-width view (cycles IN -> OUT -> NET every 1.5s)
 */
#define DISPLAY_MODE_ROTATING 0

static void hardware_init(void) {
    /* Configure all analog-capable pins (PORTA / PORTE) as digital I/O */
    ADCON1 = 0x06;

    /* IR sensor inputs */
    IR_ENTRY_TRIS = 1;
    IR_EXIT_TRIS  = 1;

    /* Indicator LED outputs */
    LED_IN_TRIS  = 0;
    LED_OUT_TRIS = 0;
    LED_NET_TRIS = 0;

    LED_IN_PIN  = 0;
    LED_OUT_PIN = 0;
    LED_NET_PIN = 0;

    lcd_init();
}

static void show_startup_banner(void) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("  PAUL SECURITY ");
    lcd_set_cursor(1, 0);
    lcd_print("   ELECTRONICS  ");
    __delay_ms(1500);
    lcd_clear();
}

/**
 * @brief Waits for a sensor pin to reach target_state within timeout_ms.
 * @return true if target_state was reached, false on timeout.
 */
static bool wait_for_entry_state(uint8_t target_state, uint16_t timeout_ms) {
    while (timeout_ms > 0U) {
        if (IR_ENTRY_PIN == target_state) {
            __delay_ms(DEBOUNCE_DELAY_MS);
            if (IR_ENTRY_PIN == target_state) {
                return true;
            }
        }
        __delay_ms(1);
        timeout_ms--;
    }
    return false;
}

static bool wait_for_exit_state(uint8_t target_state, uint16_t timeout_ms) {
    while (timeout_ms > 0U) {
        if (IR_EXIT_PIN == target_state) {
            __delay_ms(DEBOUNCE_DELAY_MS);
            if (IR_EXIT_PIN == target_state) {
                return true;
            }
        }
        __delay_ms(1);
        timeout_ms--;
    }
    return false;
}

static void render_dashboard(uint32_t count_in, uint32_t count_out, uint32_t net_count) {
    lcd_set_cursor(0, 0);
    lcd_print("IN:");
    lcd_print_padded_uint(count_in, 4);
    lcd_print(" OUT:");
    lcd_print_padded_uint(count_out, 4);

    lcd_set_cursor(1, 0);
    lcd_print("NET OCCUPANCY:");
    lcd_print_padded_uint(net_count, 2);
}

void main(void) {
    uint32_t count_in  = 0UL;
    uint32_t count_out = 0UL;
    uint32_t net_count = 0UL;

    hardware_init();
    show_startup_banner();
    render_dashboard(count_in, count_out, net_count);

    while (1) {
        /* ---------------- ENTRY SEQUENCE (RC0 -> RC1) ---------------- */
        if (IR_ENTRY_PIN == 0) {
            __delay_ms(DEBOUNCE_DELAY_MS);
            if (IR_ENTRY_PIN == 0) {
                /* Wait for person to clear Entry sensor, then cross Exit sensor */
                if (wait_for_entry_state(1, SENSOR_TIMEOUT_MS) &&
                    wait_for_exit_state(0, SENSOR_TIMEOUT_MS)  &&
                    wait_for_exit_state(1, SENSOR_TIMEOUT_MS)) {

                    count_in++;
                    net_count++;

                    LED_IN_PIN  = 1;
                    LED_NET_PIN = (net_count > 0UL) ? 1U : 0U;
                    render_dashboard(count_in, count_out, net_count);
                    __delay_ms(150);
                    LED_IN_PIN  = 0;
                }
            }
        }

        /* ---------------- EXIT SEQUENCE (RC1 -> RC0) ---------------- */
        if (IR_EXIT_PIN == 0) {
            __delay_ms(DEBOUNCE_DELAY_MS);
            if (IR_EXIT_PIN == 0) {
                /* Wait for person to clear Exit sensor, then cross Entry sensor */
                if (wait_for_exit_state(1, SENSOR_TIMEOUT_MS)  &&
                    wait_for_entry_state(0, SENSOR_TIMEOUT_MS) &&
                    wait_for_entry_state(1, SENSOR_TIMEOUT_MS)) {

                    count_out++;
                    if (net_count > 0UL) {
                        net_count--;
                    }

                    LED_OUT_PIN = 1;
                    LED_NET_PIN = (net_count > 0UL) ? 1U : 0U;
                    render_dashboard(count_in, count_out, net_count);
                    __delay_ms(150);
                    LED_OUT_PIN = 0;
                }
            }
        }
    }
}
