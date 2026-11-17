# pfu-mars-firmware

Reconstructed sources of the "PFU Mars" document scanner, Fujitsu *fi 6750, 6770a, 6770*, SDC 0P00, MDC 0N00 (Aug 8th 2014).

| Folder | Controller | MCU | Compiler |
|---|---|---|---|
| `sdc/` | scanner controller: host interface (SCSI, USB), image path, calibration, operator panel | Renesas SH7144 (SH-2) | Hitachi SuperH C compiler 5.1 |
| `mdc/` | mechanics controller: motors, sensors, paper feed, imprinter | Renesas H8S/2312 | Hitachi H8S, H8/300 C compiler 4.0 |

## Building

There is no build system here. The sources need the original Renesas toolchains:

- SDC `shc -cpu=sh2 -optimize=0 -include=include` (the two C files of `lib/` with `-optimize=1`),
  `asmsh -cpu=sh2`, the linker with `link/boot.sub` and `link/app.sub`, the librarian with
  `link/rtlib.sub`. Every file names its sections itself.
- MDC `ch38 -cpu=2000a:24 -optimize=0 -speed`, `asm38`, the linker with `link/boot.sub` and
  `link/app.sub`; the section options of single files are in the comments of those two files.

The firmware binary is created by linking the two programs and combining them into one S-record image.

## Addresses

The sources follow the link. Section starts, ends and cross references are symbols. What stays
a number: the hardware's addresses (on-chip registers, external devices, flash sectors), the
addresses one program knows of the other (they are linked apart; marked in the sources and in
the `DEFINE` lines), and in the SDC a few second names inside another file's object
(`DEFINE` lines in `sdc/link/`).
