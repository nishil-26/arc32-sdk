#  Arc32 SDK & Hardware Ecosystem

This project encompasses both a high-performance physical development board designed in KiCad and a seamless Arduino Board Support Package (BSP) for zero-configuration software development.

Developed during an internship at **IIT Gandhinagar**, this project aims to make indigenous RISC-V silicon easily accessible to developers, students, and engineers worldwide.

---

## 🛠️ The Hardware: Arc32 v1 NEO

### Core Specifications
* **Core Processor:** THEJAS32 SoC (VEGA ET1031 RISC-V RV32IM core @ 100 MHz, 256 KB unified SRAM).
* **Wireless Co-Processor:** Ai-Thinker RTL8720DN BW16 module, introducing dual-band Wi-Fi 4 and Bluetooth LE 5.0 capabilities.
* **Power & Interface:** Robust USB Type-C interface, integrated CP2102N USB-to-UART bridge, dual AMS1117 regulation stages (3.3V and 1.2V), and a dedicated 500mA PTC polyfuse.


### 📁 Hardware Source Files
The board is 100% open-source hardware. All files are located in the [`hardware/arc32_v1_neo/`](hardware/arc32_v1_neo/) directory:
* **[`source_kicad/`](hardware/arc32_v1_neo/source_kicad/)**: The raw, fully editable KiCad design files (`.kicad_sch`, `.kicad_pcb`).
* **[`production_files/`](hardware/arc32_v1_neo/production_files/)**: Ready-to-order manufacturing assets, including the Gerber/Drill ZIP, a high-resolution PDF schematic, and the **Official Arc32 Datasheet**.
* **[`renders/`](hardware/arc32_v1_neo/renders/)**: 3D layout visualizations of the physical board.

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
