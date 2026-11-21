/**
 * @file lcd.h
 * @brief 16x2 HD44780 Character LCD Driver (4-bit parallel interface).
 */

#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* HD44780 Command Set */
#define LCD_CMD_CLEAR           0x01U
#define LCD_CMD_HOME            0x02U
#define LCD_CMD_ENTRY_MODE      0x06U
#define LCD_CMD_DISPLAY_ON      0x0CU
#define LCD_CMD_4BIT_2LINE      0x28U
#define LCD_LINE_1_ADDR         0x00U
#define LCD_LINE_2_ADDR         0x40U

void lcd_init(void);
void lcd_clear(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
void lcd_print_padded_uint(uint32_t num, uint8_t width);

#endif /* LCD_H */
