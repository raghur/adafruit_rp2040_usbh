// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "c1.h"
#include "tusb.h"
#include "gpio.h"
#include "pio_usb.h"
#include "gpio.h"
#include "pio_usb_ll.h"
#include "hardware/sync.h"
// dummy implementation
void alarm_pool_add_repeating_timer_us(void) {}
void alarm_pool_create(void) {}

//uncomment next line if using adafruit feather usb host
// #define BOOST_CONVERTER_PIN GP18
// Initialize USB host stack on core1
void c1_usbh(void) {
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp                  = GP0;
    pio_cfg.tx_ch                   = 11; // has 12 dma channels;
    // pio_cfg.extra_error_retry_count = 10;
    pio_cfg.skip_alarm_pool         = true;

    #ifdef BOOST_CONVERTER_PIN
    pio_cfg.pin_dp                  = GP16;
    // only for adafruit feather that has a 5v/1amp boost converter that needs to be enabled
    // via sending 1 on GP18
    pio_cfg.pin_dp                   = GP16;
    gpio_init(BOOST_CONVERTER_PIN);
    gpio_set_dir(BOOST_CONVERTER_PIN, GPIO_OUT);
    gpio_put(BOOST_CONVERTER_PIN, 1);
    #endif

    tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(1);
    c1_start_timer();
}

// USB host stack main task
void c1_main_task(void) {
    tuh_task();
}
