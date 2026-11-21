/**
 * @file lcd.c
 * @brief Implementation of 16x2 HD44780 LCD driver in 4-bit mode.
 */

#include "config.h"
#include "lcd.h"

static void lcd_pulse_enable(void) {
    LCD_EN = 1;
    __delay_us(20);
    LCD_EN = 0;
    __delay_us(20);
}

static void lcd_send_nibble(uint8_t nibble) {
    LCD_D4 = (nibble >> 0) & 0x01U;
    LCD_D5 = (nibble >> 1) & 0x01U;
    LCD_D6 = (nibble >> 2) & 0x01U;
    LCD_D7 = (nibble >> 3) & 0x01U;
    lcd_pulse_enable();
}

void lcd_cmd(uint8_t cmd) {
    LCD_RS = 0;
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0FU);
    __delay_ms(2);
}

void lcd_data(uint8_t data) {
    LCD_RS = 1;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0FU);
    __delay_ms(2);
}

void lcd_clear(void) {
    lcd_cmd(LCD_CMD_CLEAR);
    __delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t base = (row == 0U) ? LCD_LINE_1_ADDR : LCD_LINE_2_ADDR;
    lcd_cmd(0x80U | (base + col));
}

void lcd_init(void) {
    LCD_RS_TRIS = 0;
    LCD_EN_TRIS = 0;
    LCD_D4_TRIS = 0;
    LCD_D5_TRIS = 0;
    LCD_D6_TRIS = 0;
    LCD_D7_TRIS = 0;

    LCD_RS = 0;
    LCD_EN = 0;
    __delay_ms(20);

    /* HD44780 4-bit initialization sequence */
    lcd_send_nibble(0x03U);
    __delay_ms(5);
    lcd_send_nibble(0x03U);
    __delay_us(200);
    lcd_send_nibble(0x03U);
    lcd_send_nibble(0x02U);

    lcd_cmd(LCD_CMD_4BIT_2LINE);
    lcd_cmd(LCD_CMD_DISPLAY_ON);
    lcd_cmd(LCD_CMD_ENTRY_MODE);
    lcd_clear();
}

void lcd_print(const char *str) {
    while (*str != '\0') {
        lcd_data((uint8_t)(*str++));
    }
}

void lcd_print_padded_uint(uint32_t num, uint8_t width) {
    char buf[12];
    uint8_t i = 0;

    if (num == 0UL) {
        buf[i++] = '0';
    } else {
        while (num > 0UL && i < sizeof(buf)) {
            buf[i++] = (char)((num % 10UL) + '0');
            num /= 10UL;
        }
    }

    uint8_t digits = i;
    while (digits < width) {
        lcd_data('0');
        digits++;
    }

    while (i > 0U) {
        lcd_data((uint8_t)buf[--i]);
    }
}
