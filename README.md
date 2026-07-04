#  Arc32 SDK & Hardware Ecosystem

This project encompasses both a high-performance physical development board designed in KiCad and a seamless Arduino Board Support Package (BSP) for zero-configuration software development.

Developed during an internship at **IIT Gandhinagar**, this project aims to make indigenous RISC-V silicon easily accessible to developers, students, and engineers worldwide.

---
## 🛠️ Hardware Overview

Arc32 v1 NEO is an open-source Arduino-compatible development board built around the indigenous **THEJAS32 SoC** (VEGA ET1031 RISC-V processor). The board combines native USB programming, onboard SPI Flash, integrated Wi-Fi/Bluetooth connectivity, and Arduino-style expansion headers into a compact development platform.

### ✨ Key Features

- **Processor:** THEJAS32 SoC (VEGA ET1031, 32-bit RISC-V RV32IM @ 100 MHz)
- **Memory:** 256 KB embedded SRAM + 2 MB W25Q16JVSS SPI NOR Flash
- **Wireless:** BW16 (RTL8720DN) module with Wi-Fi 4 and Bluetooth LE 5
- **USB Interface:** USB Type-C with onboard CP2102N USB-to-UART bridge
- **Power:** AMS1117-3.3V & AMS1117-1.2V regulators with 500 mA resettable polyfuse
- **Connectivity:** 24 GPIOs, PWM, UART, SPI, and I²C interfaces exposed through Arduino-compatible headers

## 📁 Hardware Resources

The complete hardware design is fully open-source and available under the `hardware/` directory.

```text
hardware/
└── arc32_v1_neo/
    ├── source_kicad/
    ├── production_files/
    └── renders/
```

###  `source_kicad/`
Contains the complete KiCad project for the board, including:
- Schematic (`.kicad_sch`)
- PCB Layout (`.kicad_pcb`)
- Project files (`.kicad_pro`)

These files can be edited directly using KiCad for customization or future revisions.

###  `production_files/`
Manufacturing-ready assets required for board fabrication and assembly:
- Gerber & Drill package (`.zip`)
- Bill of Materials (BOM)
- High-resolution schematic (PDF)
- Official Arc32 v1 NEO Datasheet

###  `renders/`
High-resolution 3D renders of the board, including:
- Top View
- Bottom View
- Angled Perspective
---

## Full Arduino-IDE board package for the Arc32 board:

## 🚀 Installation via Boards Manager (Recommended)

The easiest way to install the Arc32 SDK is through the Arduino IDE Boards Manager. This will automatically download the correct RISC-V GCC toolchain for your operating system (Windows, macOS, or Linux).

1. Open the Arduino IDE.
2. Go to **File** ➔ **Preferences** (on macOS: **Arduino** ➔ **Settings**).
3. In the **"Additional Boards Manager URLs"** field, paste the following link:
   ```text
   https://raw.githubusercontent.com/nishil-26/arc32-sdk/main/package_arc32_index.json
   ```

## Upload mode

- **Serial / UART (Default)**: JP1 jumper open. Default boot mode, used for
  programming via XMODEM over the CP2102N USB port.
- **SPI Flash (Standalone)**: JP1 jumper shorted. Board boots from the
  W25Q16 flash chip on every power-up (no need to be connected to a PC).
  
  
## Known limitations (v1.0)

- No `<algorithm>`/STL — `libstdc++` has no bare-metal rv32im port in this
  toolchain. Only the C++ language core + our own Print/Stream/String are
  available. Avoid `std::vector`, `std::function`, etc. in sketches.
- No floating point hardware — THEJAS32 has no FPU, all `float`/`double`
  math is done in software (slow but functional) via libgcc soft-float.
- `analogRead()` / ADC: THEJAS32 itself has no built-in ADC; the ARIES
  board used an external I2C ADC IC. Arc32 v1.0 does not yet include this
  — add an external ADC (e.g. ADS1115) over I2C0/I2C1 and use `Wire`
  directly until a dedicated `analogRead()` wrapper is written.
- SPI Flash direct upload not implemented — see Section 4 above.
- `INPUT_PULLUP` has no internal pull-up hardware on THEJAS32 GPIOs;
  treated as plain `INPUT` — add an external 10 KΩ pull-up resistor.
