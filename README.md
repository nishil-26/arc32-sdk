# Arc32 Arduino SDK (THEJAS32 RISC-V)

Full Arduino-IDE board package for the Arc32 board (THEJAS32 SoC, VEGA ET1031
RISC-V RV32IM core, 100 MHz, 256 KB SRAM).

**This SDK has been fully compiled and link-tested** against a real
`riscv64-unknown-elf-gcc 13.2.0` + `picolibc 1.8.6` toolchain (rv32im/ilp32
multilib). Three example sketches (Blink, SerialEcho, I2C_Scanner) build to
working `.elf` → `.bin` end to end, entry point `0x00200000` (SRAM base),
well under the 256 KB limit (largest example: 5.2 KB total).

## 1. Install the toolchain

Ubuntu/Debian:
```bash
sudo apt-get install gcc-riscv64-unknown-elf picolibc-riscv64-unknown-elf
```

This installs `riscv64-unknown-elf-gcc`/`g++` with `rv32im/ilp32` multilib
support, and `picolibc` (a small embedded C library — provides `string.h`,
`stdlib.h`, `malloc`, etc. with no OS dependency).

Other platforms: use the [xPack RISC-V GCC](https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/)
toolchain instead, which bundles picolibc-equivalent newlib-nano.

## 2. Install pyserial (for the upload tool)

```bash
pip install pyserial --break-system-packages
```

## 3. Install the board package into Arduino IDE

Copy the whole `arc32-sdk/` folder contents to:

```
~/Arduino15/packages/Arc32/hardware/riscv/1.0.0/
```
(Windows: `%LOCALAPPDATA%\Arduino15\packages\Arc32\hardware\riscv\1.0.0\`)

Then in `platform.txt`, set:
```
compiler.path={your toolchain bin path}/
```
matching wherever `riscv64-unknown-elf-gcc` actually lives
(`/usr/bin/` on Ubuntu after the apt install above).

Restart Arduino IDE. **Tools → Board → Arc32 (THEJAS32 RISC-V)** should
now appear.

## 4. Upload mode

- **Serial / UART (Default)**: JP1 jumper open. Default boot mode, used for
  programming via XMODEM over the CP2102N USB port.
- **SPI Flash (Standalone)**: JP1 jumper shorted. Board boots from the
  W25Q16 flash chip on every power-up (no need to be connected to a PC).
  *(Direct upload to flash isn't implemented in SDK v1.0 yet — program via
  UART mode first, flash-resident execution is a v1.1 goal.)*

## 5. Verified build chain

```
sketch.ino → sketch.cpp (Arduino.h auto-included)
           → riscv64-unknown-elf-g++ -march=rv32im_zicsr -mabi=ilp32
           → sketch.o
           → link against startup.o, main.o, plic_dispatch.o, wiring*.o,
             HardwareSerial.o, Print.o, Stream.o, WString.o, Wire.o,
             SPI.o, cxx_support.o
           → arc32.ld (entry @ 0x00200000, 256KB SRAM region)
           → sketch.elf
           → objcopy -O binary → sketch.bin
           → arc32_upload.py (XMODEM over UART0 @ 115200) → board SRAM
```

Important toolchain flag: **`-march=rv32im_zicsr`** (not just `rv32im`) —
the `_zicsr` extension is required for `csrr`/`csrw` instructions used in
interrupt/timer code; newer binutils rejects them without it.

## 6. Known limitations (v1.0)

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

## 7. File map

| Path | Purpose |
|---|---|
| `cores/arc32/thejas32.h` | Complete register map (every peripheral base address + bit field) |
| `cores/arc32/Arduino.h` | Top-level sketch include, Arduino API surface |
| `cores/arc32/startup.S` | Boot code + trap handler (RISC-V assembly) |
| `cores/arc32/arc32.ld` | Linker script (SRAM-only, entry @ 0x200000) |
| `cores/arc32/wiring*.c` | pinMode/digitalWrite/digitalRead/analogWrite/millis/delay |
| `cores/arc32/HardwareSerial.cpp` | UART driver (Serial/Serial1/Serial2) |
| `cores/arc32/SPI.cpp` | SPI0 master-mode driver |
| `cores/arc32/Wire.cpp` | I2C0/I2C1 master-mode driver |
| `cores/arc32/WString.cpp` | Arduino `String` class |
| `variants/arc32/pins_arduino.h` | D0–D29 → THEJAS32 GPIO/PWM pin mapping |
| `tools/arc32_upload.py` | XMODEM uploader (matches THEJAS32 boot ROM protocol) |
| `boards.txt` / `platform.txt` / `programmers.txt` | Arduino IDE board package definition |
