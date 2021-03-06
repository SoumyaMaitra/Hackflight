/*
   SPI support for STM32F3 boards

   Copyright (C) 2018 Simon D. Levy 

   This file is part of Hackflight.

   Hackflight is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Hackflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <CrossPlatformSPI.h>

// Cleanflight drivers
extern "C" {
#include "platform.h"
#include "drivers/system.h"
#include "drivers/timer.h"
#include "drivers/time.h"
#include "drivers/bus_spi.h"
#include "drivers/bus_spi_impl.h"
#include "pg/bus_spi.h"
#include "io/serial.h"
#include "target.h"
}

static busDevice_t _bus;

void spi_init(SPI_TypeDef * instance, IO_t pin)
{
    SPIDevice device = SPIINVALID;

    if (instance == SPI1) {
        device = SPIDEV_1;
    }

    else if (instance == SPI2) {
        device = SPIDEV_2;
    }

    else if (instance == SPI3) {
        device = SPIDEV_3;
    }

    spiPinConfigure(spiPinConfig(0));
    spiPreInit();

    spiInit(device);

    spiBusSetInstance(&_bus, instance);

    _bus.busdev_u.spi.csnPin = pin;

    delay(100);

    IOInit(_bus.busdev_u.spi.csnPin, OWNER_MPU_CS, 0);
    IOConfigGPIO(_bus.busdev_u.spi.csnPin, SPI_IO_CS_CFG);
    IOHi(_bus.busdev_u.spi.csnPin);
    spiSetDivisor(_bus.busdev_u.spi.instance, SPI_CLOCK_FAST);
}

static bool _busWriteRegister(uint8_t subAddress, uint8_t data)
{
    return spiBusWriteRegister(&_bus, subAddress, data);
}

static bool _spiBusReadRegisterBuffer(uint8_t subAddress, uint8_t count, uint8_t * dest)
{
    return spiBusReadRegisterBuffer(&_bus, subAddress, dest, count);
}

static bool _spiBusTransfer(const uint8_t * send, uint8_t * recv, uint8_t count)
{
    return spiBusTransfer(&_bus, send, recv, count);
}


bool  cpspi_writeRegister(uint8_t subAddress, uint8_t data)
{
    return _busWriteRegister(subAddress, data);
}

bool  cpspi_readRegisters(uint8_t subAddress, uint8_t count, uint8_t * dest)
{
    return _spiBusReadRegisterBuffer(subAddress, count, dest);
}

bool cpspi_transfer(const uint8_t * send, uint8_t * recv, uint8_t count)
{
    return _spiBusTransfer(send, recv, count);
}
