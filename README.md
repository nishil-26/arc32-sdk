# Arc32 SDK (THEJAS32 RISC-V)

Full Arduino-IDE board package for the Arc32 board (THEJAS32 SoC, VEGA ET1031
RISC-V RV32IM core, 100 MHz, 256 KB SRAM).

## 🚀 Installation via Boards Manager (Recommended)

The easiest way to install the Arc32 SDK is through the Arduino IDE Boards Manager. This will automatically download the correct RISC-V GCC toolchain for your operating system (Windows, macOS, or Linux).

1. Open the Arduino IDE.
2. Go to **File** ➔ **Preferences** (on macOS: **Arduino** ➔ **Settings**).
3. In the **"Additional Boards Manager URLs"** field, paste the following link:
   ```text
   [https://raw.githubusercontent.com/nishil-26/arc32-sdk/main/package_arc32_index.json](https://raw.githubusercontent.com/nishil-26/arc32-sdk/main/package_arc32_index.json)

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
