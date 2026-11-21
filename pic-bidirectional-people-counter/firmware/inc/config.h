/**
 * @file config.h
 * @brief Hardware pin mappings, oscillator settings, and PIC16F873A configuration bits.
 * @project PIC Bidirectional IR People Counter
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

/* System Oscillator Frequency (8 MHz External High-Speed Crystal) */
#define _XTAL_FREQ 8000000UL

/* Configuration Bits for PIC16F873A / PIC16LF873A */
#pragma config FOSC  = HS       /* High-Speed Crystal/Resonator Oscillator */
#pragma config WDTE  = OFF      /* Watchdog Timer Disabled */
#pragma config PWRTE = ON       /* Power-up Timer Enabled for supply stabilization */
#pragma config BOREN = ON       /* Brown-out Reset Enabled */
#pragma config LVP   = OFF      /* Low-Voltage Programming Disabled (frees RB3 for LCD EN) */
#pragma config CPD   = OFF      /* Data EEPROM Memory Code Protection Off */
#pragma config CP    = OFF      /* Flash Program Memory Code Protection Off */

/* ---------------- IR SENSOR PIN DEFINITIONS (Active-Low) ---------------- */
#define IR_ENTRY_PIN        RC0
#define IR_EXIT_PIN         RC1
#define IR_ENTRY_TRIS       TRISC0
#define IR_EXIT_TRIS        TRISC1

/* ---------------- STATUS INDICATOR LED DEFINITIONS ---------------- */
#define LED_IN_PIN          RC5
#define LED_OUT_PIN         RC6
#define LED_NET_PIN         RC7
#define LED_IN_TRIS         TRISC5
#define LED_OUT_TRIS        TRISC6
#define LED_NET_TRIS        TRISC7

/* ---------------- 16x2 CHARACTER LCD (4-BIT MODE) ---------------- */
#define LCD_RS              RB2
#define LCD_EN              RB3
#define LCD_D4              RB4
#define LCD_D5              RB5
#define LCD_D6              RB6
#define LCD_D7              RB7

#define LCD_RS_TRIS         TRISB2
#define LCD_EN_TRIS         TRISB3
#define LCD_D4_TRIS         TRISB4
#define LCD_D5_TRIS         TRISB5
#define LCD_D6_TRIS         TRISB6
#define LCD_D7_TRIS         TRISB7

/* ---------------- APPLICATION TIMING & LIMITS ---------------- */
#define SENSOR_TIMEOUT_MS   3000U   /* Max time (ms) allowed to cross both IR beams */
#define DEBOUNCE_DELAY_MS   25U     /* IR sensor signal debounce settling time */
#define DISPLAY_ROTATE_MS   1500U   /* Interval for rotating single-metric view */

#endif /* CONFIG_H */
