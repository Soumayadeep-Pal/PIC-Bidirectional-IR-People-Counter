#include <xc.h>
#define _XTAL_FREQ 8000000

// ---------------- CONFIG BITS ----------------
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config CP = OFF

// -------- LCD PIN DEFINITIONS --------
#define RS RB2
#define EN RB3
#define D4 RB4
#define D5 RB5
#define D6 RB6
#define D7 RB7

// -------- LCD FUNCTIONS --------
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

void lcd_print(const char *str) {
    while(*str) lcd_data(*str++);
}

void lcd_set_cursor(unsigned char pos) {
    lcd_cmd(0x80 | pos);
}

void lcd_print_number_clear(unsigned long num, unsigned char width) {
    char buf[12];
    int i = 0;

    if(num == 0) buf[i++] = '0';
    else {
        unsigned long n = num;
        while(n > 0) {
            buf[i++] = (n % 10) + '0';
            n /= 10;
        }
    }

    // Print number reversed
    int digits = i;
     while(digits < width) {
        lcd_data('0');
        digits++;
    }
    while(i--) {
        lcd_data(buf[i]);
    }

    // Print spaces to clear old digits
   
}
#define LED_IN   RC5
#define LED_OUT  RC6
#define LED_NET  RC7
// -------- MAIN CODE --------
void main() {

    unsigned long countIN = 0;
    unsigned long countOUT = 0;
    unsigned long total =0;

    ADCON1 = 0x06;   // Make PORTA and PORTC digital

    TRISC0 = 1;      // ENTRY IR SENSOR (Input)
    TRISC1 = 1;      // EXIT  IR SENSOR (Input)

    TRISB2 = 0; TRISB3 = 0;
    TRISB4 = 0; TRISB5 = 0;
    TRISB6 = 0; TRISB7 = 0;

    lcd_init();
// -------- STARTUP SCREEN --------
lcd_cmd(0x01);        // clear
lcd_set_cursor(0);
lcd_print("  PAUL SECURITY ");
lcd_set_cursor(0XC0);
lcd_print("   ELECTRONICS ");
__delay_ms(300);     // show for 2 seconds
lcd_cmd(0x01);        // clear again before main loop

    while(1) {

       if (RC0 == 0) {            // ENTRY triggered first
            while (RC0 == 0);      // wait for release
//            __delay_ms(100);

            // wait for EXIT sensor
            while (RC1 == 1);      // waiting for EXIT
            while (RC1 == 0);      // wait for release

            // Valid IN direction
            countIN++;
            total++;
//            __delay_ms(200);
LED_IN = 1;
LED_NET=1;
        }
LED_IN = 0;
LED_NET=0;
        // ---------------- OUT SEQUENCE ----------------
       if (RC1 == 0) {            // EXIT triggered first
            while (RC1 == 0);
//            __delay_ms(100);

            // wait for ENTRY sensor
            while (RC0 == 1);
            while (RC0 == 0);
LED_OUT = 1;
LED_NET=1;
            // Valid OUT direction
            if (total > 0) total--;
            countOUT++;

//            __delay_ms(200);
        }
LED_OUT = 0;
LED_NET=0;
// ---------- ROTATING DISPLAY ----------

lcd_set_cursor(0); lcd_print("IN:");
lcd_print_number_clear(countIN, 4); 
lcd_print(" OUT:"); 
lcd_print_number_clear(countOUT, 4); 
lcd_set_cursor(0xC0);
lcd_print("NET:");
lcd_print_number_clear(total, 4);


//unsigned int display_timer = 0;
//unsigned char display_mode = 0;   // 0 = IN, 1 = OUT, 2 = NET
//// ---------- NON-BLOCKING ROTATING DISPLAY ----------
//display_timer++;
//
//if(display_timer >= 1000) {   // ?100ms (800 loops × 125us approx)
//    display_timer = 0;
//    display_mode++;
//    if(display_mode > 2) display_mode = 0;
//}
//
//// Select screen
//lcd_cmd(0x01);  // clear screen
//
//if(display_mode == 0) {
//    lcd_set_cursor(0);
//    lcd_print(" IN COUNT ");
//    lcd_set_cursor(0xC0);
//    lcd_print_number_clear(countIN, 4);
//}
//
//else if(display_mode == 1) {
//    lcd_set_cursor(0);
//    lcd_print(" OUT COUNT ");
//    lcd_set_cursor(0xC0);
//    lcd_print_number_clear(countOUT, 4);
//}
//
//else if(display_mode == 2) {
//    lcd_set_cursor(0);
//    lcd_print(" NET COUNT ");
//    lcd_set_cursor(0xC0);
//    lcd_print_number_clear(total, 4);
//}

//        __delay_ms(100);
    }
}
