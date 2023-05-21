# JTAG library for FT232H on Pikes Peak / Storey Peak FPGA Boards - on Windows

This driver allows using the on-board FT232 chip as a JTAG interface in Quartus for the Pikes Peak and Storey Peak FPGA accelerator boards.

This is a Windows specific driver, which is based on [this driver](https://github.com/ruurdk/jtag-quartus-ft232h), which can build under Windows but isn't Windows native (it's a hassle to setup the toolchain).

## Installation

Install pre-reqs: install the latest version of MS Visual Studio Community Edition (tested up to VS 2022).


Build: open the solution file and build everything.


To install, copy `jtag_hw_ft323h_lib.dll` to your quartus `bin64` directory (eg. `C:/intelFPGA/22.1std/quartus/bin64`).

## Initialization

Before using this driver, run OpenOCD to initialize the FT232H:

```sh
openocd \
    -f interface/ftdi/um232h.cfg \
    -c "adapter speed 2000; transport select jtag; jtag newtap auto0 tap -irlen 10 -expected-id 0x029070dd; init; exit;"
```

To initialize the interface at different clock speeds, change the `adapter_khz` parameter.


OpenOCD prebuilt binaries can be downloaded from <https://github.com/xpack-dev-tools/openocd-xpack/releases>.

## Current state

- JTAG scan chain and programming of the FPGA works
- Initialization of the MPSSE is not implemented --> run OpenOCD once to init the device

### Download time

The following table present the bitstream download time at different clock
speeds for 5SGSMD5 with the bitstream size of 213,798,880 bits (cca 26 MB).

| Clock rate [MHz] | Download time [mm:ss] |
|------------------|-----------------------|
|  2               | 03:51                 |
|  4               | 02:50                 |
|  6               | 02:24                 |
| 10               | 02:03                 |
| 20               | 01:54                 |


## View debug log (with Debug build)

Download DebugView (from the SysInternals suite) and capture events.