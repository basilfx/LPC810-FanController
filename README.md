# LPC810 Fan Controller
Tiny PID-based fan controller board and firmware to insert between 3-pin fans.

## Contents
* Eagle schematic and PCB
* Firmware
* PID simulator and monitor

## Eagle

### Board render
The board measures 0.45x0.95 inch.

![brd](https://raw.github.com/basilfx/lpc810-fancontroller/master/doc/board_top.png)
Board top render

![brd](https://raw.github.com/basilfx/lpc810-fancontroller/master/doc/board_bottom.png)
Board bottom render

## Firmware
The firmware can be compiled with the [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded) toolchain. Just run `make`.

## License
See the `LICENSE` file (MIT license).

This software makes use of other software. Please see `firmware/README.md` for more information.