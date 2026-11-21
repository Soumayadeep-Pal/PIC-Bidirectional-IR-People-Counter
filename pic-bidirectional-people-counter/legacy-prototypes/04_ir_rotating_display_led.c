#include <xc.h>
#define _XTAL_FREQ 8000000

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config CP = OFF

#define RS RB2
#define EN RB3
#define D4 RB4
#define D5 RB5
#define D6 RB6
#define D7 RB7

#define LED_IN   RC5
#define LED_OUT  RC6
#define LED_NET  RC7

void lcd_pulse() {
    EN = 1; __delay_us(20);
    EN = 0; __delay_us(20);
}

void lcd_send_nibble(unsigned char nib) {
    D4 = (nib >> 0) & 1;
    D5 = (nib >> 1) & 1;
    D6 = (nib >> 2) & 1;
    D7 = (nib >> 3) & 1;
    lcd_pulse();
}

void lcd_cmd(unsigned char cmd) {
    RS = 0;
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    __delay_ms(2);
}

void lcd_data(unsigned char data) {
    RS = 1;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    __delay_ms(2);
}

void lcd_init() {
    RS = 0;
    EN = 0;
    __delay_ms(20);

    lcd_send_nibble(0x03); __delay_ms(5);
    lcd_send_nibble(0x03); __delay_us(200);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    __delay_ms(2);
}

void lcd_print(const char *s) {
    while(*s) lcd_data(*s++);
}

void lcd_print_number(unsigned long n, unsigned char width) {
    char buf[12];
    int i = 0;

    if(n == 0) buf[i++] = '0';
    else {
        while(n > 0) {
            buf[i++] = (n % 10) + '0';
            n /= 10;
        }
    }

    int digits = i;
    while(digits < width) { lcd_data('0'); digits++; }
    while(i--) lcd_data(buf[i]);
}

void main() {

    unsigned long INcount = 0;
    unsigned long OUTcount = 0;
    unsigned long NET = 0;

    ADCON1 = 0x06;

    TRISC0 = 1;
    TRISC1 = 1;

    TRISC5 = 0;
    TRISC6 = 0;
    TRISC7 = 0;

    TRISB2 = 0; TRISB3 = 0;
    TRISB4 = 0; TRISB5 = 0;
    TRISB6 = 0; TRISB7 = 0;

        lcd_init();
// -------- STARTUP SCREEN --------
lcd_cmd(0x01);        // clear
//lcd_set_cursor(0);
lcd_print("  PAUL SECURITY ");
//lcd_set_cursor(0XC0);
lcd_cmd(0XC0);
lcd_print("   ELECTRONICS ");
__delay_ms(1500);     // show for 2 seconds
lcd_cmd(0x01); 

    unsigned char screen = 0;   // 0=IN, 1=OUT, 2=NET

    unsigned int timer = 0;     // rotation timer

    while(1) {

        // -------- ENTRY --------
        if(RC0 == 0) {
            while(RC0 == 0);
            while(RC1 == 1);
            while(RC1 == 0);

            INcount++;
            NET++;
            LED_IN = 1;
            __delay_ms(30);
            LED_IN = 0;
            
        }

        // -------- EXIT --------
        if(RC1 == 0) {
            while(RC1 == 0);
            while(RC0 == 1);
            while(RC0 == 0);

            if(NET > 0) NET--;
            OUTcount++;

            LED_OUT = 1;
            __delay_ms(30);
            LED_OUT = 0;
        }

       // LED_NET = (NET >= 0);

        // -------- DISPLAY ROTATION every ~300ms --------
        timer++;
        if(timer >= 80) {
            timer = 0;

            lcd_cmd(0x01); // clear

            if(screen == 0) {
                lcd_print("IN:");
                lcd_print_number(INcount, 7);
                screen = 1;
                LED_IN = 1;
                __delay_ms(1500);
                LED_IN = 0;
            }
            else if(screen == 1) {
                lcd_print("OUT:");
                lcd_print_number(OUTcount, 7);
                screen = 2;
                LED_OUT = 1;
                __delay_ms(1500);
                LED_OUT = 0;
            }
            else {
                lcd_print("NET:");
                lcd_print_number(NET, 7);
                screen = 0;
                LED_NET = 1;
                __delay_ms(1500);
                LED_NET = 0;
            }
        }

        __delay_ms(1);  // 1ms small delay (non blocking)
    }
}
