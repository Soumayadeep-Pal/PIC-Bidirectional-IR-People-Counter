# PIC Bidirectional IR People Counter

A modular, production-ready embedded C firmware repository for a **bidirectional infrared people counter and occupancy monitor** built on the **Microchip PIC16F873A** microcontroller (`XC8` compiler).

Designed for entrances, hallways, labs, and retail counters, the system uses dual IR sensors to detect direction of travel, tracks cumulative entries (`IN`), exits (`OUT`), and real-time net occupancy (`NET`), displays live counts on a `16x2` HD44780 LCD, and provides visual feedback via dedicated directional LEDs.

---

## Key Features

- **Bidirectional Sequence Detection:** Distinguishes `IN` (`RC0 -> RC1`) from `OUT` (`RC1 -> RC0`) traversal sequences.
- **Anti-Lockup Timeout Guard:** Bounded state transitions (`3000 ms` default window) prevent the firmware from hanging if a single sensor is momentarily blocked or someone turns back mid-threshold.
- **Signal Debouncing:** Built-in `25 ms` digital debounce filtering eliminates false counts caused by IR sensor chatter.
- **Live 16x2 LCD Dashboard:** Displays zero-padded `IN`, `OUT`, and `NET OCCUPANCY` metrics simultaneously in 4-bit parallel mode without screen flicker.
- **Visual LED Indicators:** Dedicated status LEDs for entry events (`RC5`), exit events (`RC6`), and active room occupancy (`RC7`).
- **Clean Modular Architecture:** Separated hardware configuration (`config.h`), HD44780 LCD driver (`lcd.h` / `lcd.c`), and application state machine (`main.c`).

---

## Repository Structure

```text
pic-bidirectional-people-counter/
├── firmware/
│   ├── inc/
│   │   ├── config.h            # Oscillator, pragma config bits, and pin definitions
│   │   └── lcd.h               # 16x2 HD44780 LCD driver interface
│   └── src/
│       ├── lcd.c               # 4-bit LCD driver implementation
│       └── main.c              # Bidirectional IR counter state machine & main loop
├── mplabx/
│   └── pic-ir-counter.X/       # Clean MPLAB X IDE project (build artifacts excluded)
│       ├── Makefile
│       └── nbproject/
├── docs/
│   └── HARDWARE_WIRING.md      # Complete PIC16F873A pinout, wiring & mounting guide
├── legacy-prototypes/          # Historical development iterations for reference
├── .gitignore                  # Ignores XC8 build/dist outputs and IDE private files
├── LICENSE                     # MIT License
└── README.md
```

---

## Hardware Summary

- **Microcontroller:** Microchip `PIC16F873A` @ `8 MHz` HS Crystal
- **Sensors:** 2x Active-Low IR Obstacle / Break-Beam Sensors (`RC0` Entry, `RC1` Exit)
- **Display:** `16x2` Character LCD (HD44780 compatible, 4-bit mode on `RB2–RB7`)
- **Indicators:** 3x Status LEDs (`RC5` Entry, `RC6` Exit, `RC7` Net Occupancy)

See [`docs/HARDWARE_WIRING.md`](docs/HARDWARE_WIRING.md) for the full pin allocation table and physical sensor spacing recommendations.

---

## Build & Flash Instructions

### Prerequisites
- **MPLAB X IDE** (v6.00 or newer)
- **Microchip XC8 Compiler** (v2.40+ / v3.10 tested)
- **PIC16Fxxx Device Family Pack (`PIC16Fxxx_DFP`)**
- Hardware programmer such as **PICkit 3**, **PICkit 4**, **PICkit 5**, or **MPLAB Snap**

### Steps to Build
1. Clone this repository:
   ```bash
   git clone https://github.com/<your-username>/pic-bidirectional-people-counter.git
   ```
2. Open **MPLAB X IDE** and select **File -> Open Project...**
3. Navigate to `mplabx/pic-ir-counter.X` and open the project.
4. Click **Clean and Build Main Project** (`Shift + F11`) to compile the firmware with `XC8`.
5. Connect your programmer to the target board's ICSP header (`MCLR`, `VDD`, `VSS`, `PGD/RB7`, `PGC/RB6`) and click **Make and Program Device Main Project**.

---

## Refurbishments Made from Original Archive

If you are migrating from the earlier prototype workspace (`PIC File.zip`), the following engineering improvements have been applied:
1. **Stripped 200+ Build & Binary Artifacts:** Removed `build/`, `dist/`, `.o`, `.p1`, `.elf`, `.hex`, and `nbproject/private/` files so Git history stays clean and lightweight.
2. **Fixed Missing `TRISC` Output Configuration:** Earlier drafts toggled `RC5`, `RC6`, and `RC7` without clearing `TRISC5:7`, leaving the LED pins in high-impedance input mode. All LED pins are now properly initialized as digital outputs.
3. **Eliminated Infinite Blocking Loops:** Replaced unbounded `while(RC1 == 1);` waits with `wait_for_entry_state()` / `wait_for_exit_state()` timeout guards so partial entries never freeze the counter.
4. **Modularized Source Tree:** Split monolithic `newmain.c` code into reusable `config.h`, `lcd.h`, `lcd.c`, and `main.c` modules.

---

## License

This project is licensed under the **MIT License** — see the [`LICENSE`](LICENSE) file for details.
