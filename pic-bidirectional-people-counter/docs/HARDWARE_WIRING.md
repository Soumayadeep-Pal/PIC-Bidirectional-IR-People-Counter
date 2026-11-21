# Hardware Wiring & Pinout Guide

## Target Microcontroller
- **MCU:** Microchip `PIC16F873A` (or `PIC16LF873A`), 28-pin PDIP/SOIC
- **Clock Source:** `8.000 MHz` External High-Speed (`HS`) Crystal Oscillator across `OSC1/CLKIN` (Pin 9) and `OSC2/CLKOUT` (Pin 10) with two `22 pF` load capacitors to `VSS`
- **Supply Voltage:** `5.0 V DC` regulated (`VDD` Pin 20, `VSS` Pins 8 & 19) with `100 nF` decoupling capacitor
- **MCLR Reset:** Pin 1 pulled up to `VDD` via a `10 kΩ` resistor

---

## Complete Pin Allocation Table

| Peripheral | Signal | PIC16F873A Pin | Register / Bit | Direction | Notes |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **Entry IR Sensor** | `IR_ENTRY` | Pin 11 | `RC0` | Input | Active-Low (`0` = beam broken) |
| **Exit IR Sensor** | `IR_EXIT` | Pin 12 | `RC1` | Input | Active-Low (`0` = beam broken) |
| **Entry LED** | `LED_IN` | Pin 16 | `RC5` | Output | Active-High via `330 Ω` series resistor |
| **Exit LED** | `LED_OUT` | Pin 17 | `RC6` | Output | Active-High via `330 Ω` series resistor |
| **Occupancy LED** | `LED_NET` | Pin 18 | `RC7` | Output | Active-High via `330 Ω` series resistor (`ON` when `NET > 0`) |
| **16x2 LCD** | `RS` (Register Select) | Pin 23 | `RB2` | Output | `0` = Command, `1` = Data |
| **16x2 LCD** | `EN` (Enable) | Pin 24 | `RB3` | Output | Requires `LVP = OFF` in config bits |
| **16x2 LCD** | `D4` (Data Bit 4) | Pin 25 | `RB4` | Output | 4-bit parallel data bus |
| **16x2 LCD** | `D5` (Data Bit 5) | Pin 26 | `RB5` | Output | 4-bit parallel data bus |
| **16x2 LCD** | `D6` (Data Bit 6) | Pin 27 | `RB6` | Output | 4-bit parallel data bus |
| **16x2 LCD** | `D7` (Data Bit 7) | Pin 28 | `RB7` | Output | 4-bit parallel data bus |

> **LCD Tie-Downs:** Connect LCD `R/W` (Pin 5) directly to `GND` (`VSS`) for write-only mode. Connect `V0` (Pin 3) to the wiper of a `10 kΩ` contrast potentiometer between `5V` and `GND`.

---

## Sensor Placement & Directional Logic

Mount the two IR break-beam or reflective obstacle sensors **15 cm to 30 cm apart** along the doorway/corridor threshold:

```text
  [OUTSIDE]  --->   [ IR_ENTRY (RC0) ]   --->   [ IR_EXIT (RC1) ]   --->  [INSIDE]
```

1. **Valid Entry (`IN++`, `NET++`):** `RC0` goes LOW first, clears HIGH, and `RC1` subsequently goes LOW and clears HIGH within `SENSOR_TIMEOUT_MS` (default `3000 ms`).
2. **Valid Exit (`OUT++`, `NET--`):** `RC1` goes LOW first, clears HIGH, and `RC0` subsequently goes LOW and clears HIGH within `SENSOR_TIMEOUT_MS`.
3. **False Trigger Protection:** If a person steps into only one beam and turns back, the sequence times out safely without locking up the microcontroller or corrupting the count.
