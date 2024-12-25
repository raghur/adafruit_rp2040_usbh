// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h>

#undef RP_SPI_USE_SPI1
#define RP_SPI_USE_SPI1 TRUE

#undef RP_CORE1_START
#define RP_CORE1_START TRUE
// backlight on PWM6B - GP29
#undef RP_PWM_USE_PWM6
#define RP_PWM_USE_PWM6 TRUE

// audio on GP15 - PWM7B
#undef RP_PWM_USE_PWM7
#define RP_PWM_USE_PWM7 TRUE

