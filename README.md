# LPC810 Fan Controller
Tiny PID-based fan controller board and firmware to insert between 3-pin fans.

## Contents
* Eagle schematic and PCB
* Firmware
* PID simulator and monitor

## Eagle

### Board render
The board measures 0.45x0.95 inch.

![brd](https://raw.github.com/basilfx/LPC810-FanController/master/doc/board_top.png)
Board top render

![brd](https://raw.github.com/basilfx/LPC810-FanController/master/doc/board_bottom.png)
Board bottom render

## Firmware
The firmware can be found in the `firmware/` foler. It compiles fine with the [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded) toolchain. Just run `make`.

## Contrib
In the `contrib` folder contains some tools to test the PID controller. First, run `python setup.py build_ext --inplace` to build a Python wrapper for the exact same PID controller in the firmware. This requires Cython to be available.

## License
See the `LICENSE` file (MIT license).

This software makes use of other software. Please see `firmware/README.md` for more information.