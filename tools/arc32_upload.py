#!/usr/bin/env python3
"""
arc32_upload.py — Arc32 / THEJAS32 Upload Tool

Implements the XMODEM protocol to transfer a compiled .bin file over
UART0 (via the CP2102N USB-UART bridge) into THEJAS32 SRAM, exactly as
described in the THEJAS32 datasheet section 2.2.5 (Boot modes -> UART Mode).

Sequence:
  1. Open serial port at specified baud (must match GPIO25-31 baud-strap,
     default 115200 for the Arc32 board's fixed resistor divider).
  2. Toggle DTR/RTS to force the board into reset + bootloader (auto-reset
     circuit on CP2102N RTS -> 100nF -> PUSH_RESETN, see Section G of the
     schematic connection table).
  3. Wait for the bootloader's 'C' (NAK-equivalent start) byte.
  4. Send the binary in 128-byte XMODEM blocks (checksum mode) or
     1K blocks (CRC mode) -- this implementation uses classic 128-byte
     checksum XMODEM for maximum bootloader compatibility.
  5. Send EOT, wait for ACK, done.

Usage:
  python3 arc32_upload.py --port /dev/ttyUSB0 --baud 115200 --file sketch.bin
"""

import argparse
import sys
import time
import os

try:
    import serial
except ImportError:
    print("ERROR: pyserial not installed. Run: pip install pyserial --break-system-packages")
    sys.exit(1)

# XMODEM protocol bytes
SOH = 0x01   # Start of 128-byte block
STX = 0x02   # Start of 1024-byte block (not used here)
EOT = 0x04   # End of transmission
ACK = 0x06
NAK = 0x15
CAN = 0x18   # Cancel
C   = 0x43   # 'C' - receiver wants CRC mode (we still send checksum-compatible)

BLOCK_SIZE = 128
MAX_RETRIES = 10
TIMEOUT_S = 10


def checksum(data: bytes) -> int:
    return sum(data) & 0xFF


def crc16(data: bytes) -> int:
    crc = 0x0000
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
            crc &= 0xFFFF
    return crc


def pad_block(data: bytes, block_size: int = BLOCK_SIZE) -> bytes:
    if len(data) < block_size:
        data += b"\x1A" * (block_size - len(data))  # CTRL-Z padding
    return data


def reset_into_bootloader(ser: serial.Serial):
    """
    Pulse RTS low then high. CP2102N RTS line is wired through a 100nF
    cap to PUSH_RESETN (see schematic Section E), producing a brief
    reset pulse on THEJAS32. After reset, BOOT_SEL (pulled HIGH by R5,
    default) selects UART boot mode automatically.
    """
    ser.rts = False
    ser.dtr = True
    time.sleep(0.1)
    ser.rts = True
    time.sleep(0.1)
    ser.rts = False
    time.sleep(0.3)   # allow bootloader to initialize and start listening


def wait_for_handshake(ser: serial.Serial, timeout_s: float = TIMEOUT_S) -> str:
    """
    THEJAS32 bootloader sends 'C' repeatedly to request CRC-mode XMODEM,
    or NAK for checksum mode, depending on ROM version. We accept either.
    """
    start = time.time()
    while time.time() - start < timeout_s:
        b = ser.read(1)
        if b:
            if b[0] == C:
                return "crc"
            elif b[0] == NAK:
                return "checksum"
        time.sleep(0.01)
    raise TimeoutError("No handshake byte ('C' or NAK) received from bootloader. "
                        "Check BOOT_SEL jumper is OPEN (UART mode) and board is powered.")


def send_block(ser: serial.Serial, block_num: int, data: bytes, mode: str) -> bool:
    block_num_byte = block_num & 0xFF
    inv_block_num = (~block_num_byte) & 0xFF

    if mode == "crc":
        crc = crc16(data)
        packet = bytes([SOH, block_num_byte, inv_block_num]) + data + \
                 bytes([(crc >> 8) & 0xFF, crc & 0xFF])
    else:
        chk = checksum(data)
        packet = bytes([SOH, block_num_byte, inv_block_num]) + data + bytes([chk])

    for attempt in range(MAX_RETRIES):
        ser.write(packet)
        ser.flush()
        resp = ser.read(1)
        if resp and resp[0] == ACK:
            return True
        elif resp and resp[0] == CAN:
            raise RuntimeError("Transfer cancelled by receiver (bootloader sent CAN).")
        # NAK or timeout -> retry same block
    return False


def xmodem_send(ser: serial.Serial, filepath: str):
    with open(filepath, "rb") as f:
        data = f.read()

    total_size = len(data)
    print(f"[Arc32] Binary size: {total_size} bytes")

    if total_size > 256 * 1024:
        print("WARNING: binary exceeds 256KB SRAM. Upload will likely fail "
              "or corrupt memory beyond available SRAM.")

    print("[Arc32] Resetting board into bootloader...")
    reset_into_bootloader(ser)

    print("[Arc32] Waiting for bootloader handshake...")
    mode = wait_for_handshake(ser)
    print(f"[Arc32] Bootloader ready (mode={mode}). Starting XMODEM transfer...")

    block_num = 1
    offset = 0
    total_blocks = (total_size + BLOCK_SIZE - 1) // BLOCK_SIZE

    while offset < total_size:
        chunk = data[offset:offset + BLOCK_SIZE]
        chunk = pad_block(chunk)

        ok = send_block(ser, block_num, chunk, mode)
        if not ok:
            raise RuntimeError(f"Block {block_num} failed after {MAX_RETRIES} retries.")

        pct = int(100 * (block_num) / total_blocks)
        print(f"\r[Arc32] Uploading: block {block_num}/{total_blocks} ({pct}%)", end="", flush=True)

        offset += BLOCK_SIZE
        block_num += 1

    print()  # newline after progress bar

    # Send EOT, expect ACK
    for attempt in range(MAX_RETRIES):
        ser.write(bytes([EOT]))
        ser.flush()
        resp = ser.read(1)
        if resp and resp[0] == ACK:
            print("[Arc32] Transfer complete. Bootloader jumping to user code at 0x00200000.")
            return
    raise RuntimeError("EOT not acknowledged by bootloader.")


def main():
    parser = argparse.ArgumentParser(description="Arc32 XMODEM upload tool")
    parser.add_argument("--port", required=True, help="Serial port (e.g. /dev/ttyUSB0 or COM5)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (must match GPIO25-31 strap)")
    parser.add_argument("--file", required=True, help="Path to compiled .bin file")
    parser.add_argument("--mode", default="uart", choices=["uart", "spiflash"],
                         help="Upload mode (spiflash requires BOOT_SEL jumper shorted)")
    parser.add_argument("--reset-pin", default="auto",
                         help="Reset mechanism: 'auto' uses CP2102N RTS auto-reset circuit")
    args = parser.parse_args()

    if not os.path.isfile(args.file):
        print(f"ERROR: file not found: {args.file}")
        sys.exit(1)

    if args.mode == "spiflash":
        print("ERROR: SPI Flash direct upload not yet implemented in v1.0.")
        print("Workaround: upload via UART mode first (writes to SRAM and runs),")
        print("            then use a separate flashing sketch to write W25Q16 yourself,")
        print("            or use an external SPI flash programmer.")
        sys.exit(1)

    print(f"[Arc32] Opening {args.port} @ {args.baud} baud...")
    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except serial.SerialException as e:
        print(f"ERROR: could not open serial port: {e}")
        sys.exit(1)

    try:
        xmodem_send(ser, args.file)
    except Exception as e:
        print(f"\n[Arc32] UPLOAD FAILED: {e}")
        sys.exit(1)
    finally:
        ser.close()

    print("[Arc32] Done. Press the RESET button or re-power the board to run your sketch again.")


if __name__ == "__main__":
    main()
