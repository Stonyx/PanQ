# PanQ - IT8528 Interactions

This is an overhauled version of the `panq` tool originally developed by [guedou](https://github.com/guedou).  Lots of operational help was provided by [r-pufky](https://github.com/r-pufky) to make this happen.

Below is the original README with some minor alterations.

The `panq` (QNAP reversed) binary provides commands to interact with the ITE IT8528 Super I/O Controller on a many QNAP NAS units.  Its main goals are to read the temperature sensors and to control the fan speeds.  The [lib/libuLinux_hal.so](libuLinux_hal.so) was first reversed with [radare2](https://github.com/radare/radare2), [Ghidra](https://github.com/NationalSecurityAgency/ghidra), and [QBDI](https://qbdi.quarkslab.com/), then the [necessary functions](src/it8528.c) were reimplemented.


## Implemented Commands

The reimplemented functions from `libuLinux_hal.so` provide the following `panq` features:
```
$ panq
Control the I8528 Super I/O controller chip on QNAP NAS units

Usage: panq { COMMAND | help }

Available commands:
  check                   - detect the Super I/O controller
  fan1 [speed_percentage] - get or set the fan #1 speed
  fan2 [speed_percentage] - get or set the fan #2 speed
  fan3 [speed_percentage] - get or set the fan #3 speed
  fan4 [speed_percentage] - get or set the fan #4 speed
  help                    - this help message
  log                     - display fan speed & temperature
  test [libuLinux_hal.so] - test functions against libuLinux_hal.so
  temp1                   - retrieve the temperature of sensor #1
  temp2                   - retrieve the temperature of sensor #2
  temp3                   - retrieve the temperature of sensor #3
  temp4                   - retrieve the temperature of sensor #4
  temp5                   - retrieve the temperature of sensor #5
```


## Notes

- the binary needs `libcap-ng` and `libseccomp2` to be built.
- to run `panq` as as regular user, use `make capability` 


## More Functionalities

The original `libuLinux_hal.so` library contains interesting `ec_sys_*` functions that seems to indicate that the following functionalities could be implemented in `panq`:
- control more LEDs
- enclosure opening detection
- automatic fan speed (`ec_sys_set_tfan_auto()`, `ec_sys_set_qfan_auto()`...)
