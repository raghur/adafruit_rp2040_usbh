# Changes from upstream - branch: https://github.com/raghur/adafruit_rp2040_usbh/tree/raghu

1. Removed vendored in source for `tiny_usb` (back to submodule)
2. Works with latest `tiny_usb` master instead of a 1yr old commit
3. Make jerry rigged pico with USB port as the primary target
3. Update `Pico-PIO-USB` to latest master
4. Updated `README.md` (this file)

## Problems/issues

Need help with the following niggles:

1. EEPROM emulation is broken; When writing to EEPROM, values are saved but USB is broken and needs to unplugged & replugged.
2. RBGLIGHT_ENABLE does not work; USB enumerates but the keyboard is not functional
3. Can/maybe interferes with the host system suspend/wakeup - though I think that might be a weird USB enumeration issue with pi picos.
    - Not sure about this since sometimes everything works flawlessly vs other time my system won't go to sleep and there's 
    errors in journal about USB.
4. Also conflicts with DMA channel - so anything in QMK that uses DMA indirectly will likely break - ex SPI; 
    - ChibiOs has its own tracking of DMA channels but PIO USB uses rp sdk directly.
    - Workaround - set DMA channel to 11 in `c1_usbh.c`.Obviously not foolproof as ChibiOs could still go ahead and allocate it.

# USB-to-USB convertor using [ Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico-1-technical-specification)

## Hardware

1. Build your own: follow the [guide here](https://github.com/jfedor2/hid-remapper/blob/master/HARDWARE.md)
2. Buy - Adafruit Feather 5723 [Feather with USB port](https://www.adafruit.com/product/5723)

## Software

This uses [Sekigon's Keyboard Quantizer mini-full branch](https://github.com/sekigon-gonnoc/qmk_firmware/tree/keyboard/sekigon/keyboard_quantizer/mini-full/keyboards/sekigon/keyboard_quantizer/mini)

Code in the repo works directly with USB host pin DP+ on GPIO1

If you're using an adafruit feather USB Host, then uncomment
```c
//uncomment next line if using adafruit feather usb host
// #define BOOST_CONVERTER_PIN GP18

```

## Available keymaps

- test - my (admittedly poorly named) keymap
- default - default Generic ANSI 104 keys.

## How to use this repository

After [setup your qmk environment](https://github.com/qmk/qmk_firmware/blob/master/docs/newbs_getting_started.md), clone this repository to `keyboards/converter` then run

```shell
git clone https://github.com/whyaaronbailey/adafruitrp2040_usbh.git _your_qmk_repo/keyboards/converter/adafruit_rp2040_usbh
cd _your_qmk_repo/keyboards/converter/adafruit_rp2040_usbh
   git submodule update --init --recursive
cd ../../..
make converter/adafruit_rp2040_usbh:_your_choice:uf2 
```

where `_your_choice` can be `default` forgeneric 104-key ANSI keyboard or `test` for my custom keymap

# Annexure
![Pi PICO pinout](https://microcontrollerslab.com/wp-content/uploads/2021/01/Raspberry-Pi-Pico-pinout-diagram.svg)
