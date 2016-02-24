/*
Copyright (c) 2011-2015 <comparator@gmx.de>

This file is part of the X13.Home project.
http://X13home.org
http://X13home.net
http://X13home.github.io/

BSD New License
See LICENSE file for license details.
*/

#ifndef _S2CN13_H
#define _S2CN13_H

// Board: S2EC13
// uC: STM32F051C8T6
// PHY1: CC1101
// PHY2: --

// GPIOA
// Pin  Port    CN  Func
//   0  PA0     3
//   1  PA1     4
//   2  PA2     5   _USART2_TX
//   3  PA3     6   _USART2_RX
//   4  PA4     15
//   5  PA5     16
//   6  PA6     17
//   7  PA7     18
//   8  PA8         CC11_IRQ
//   9  PA9     9   _USART1_TX
//  10  PA10    10  _USART1_RX
//  11  PA11    7
//  12  PA12    8
//  13  PA13        SWDIO
//  14  PA14        SWCLK
//  15  PA15
// GPIOB
//  16  PB0     19
//  17  PB1     20
//  18  PB2         LED
//  19  PB3
//  20  PB4
//  21  PB5
//  22  PB6
//  23  PB7
//  24  PB8
//  25  PB9
//  26  PB10    13  SCL2
//  27  PB11    14  SDA2
//  28  PB12        CC11_SEL
//  29  PB13        CC11_SCK
//  30  PB14        CC11_MISO
//  31  PB15        CC11_MOSI


#ifdef __cplusplus
extern "C" {
#endif

// DIO Section
#define EXTDIO_USED                 1
#define EXTDIO_MAXPORT_NR           2
#define HAL_DIO_MAPPING             {17, 16, 7, 6, 5, 4, 27, 26, 10, 9, 12, 11, 3, 2, 1, 0}
// End DIO Section

// PA4-PA7: 4 - 7
// PB0-PB1: 8 - 9
// Analogue Inputs
#define EXTAIN_USED                 1
#define EXTAIN_MAXPORT_NR           6
#define EXTAIN_BASE_2_APIN          {9, 8, 7, 6, 5, 4}
#define EXTAIN_REF                  0x02        // Bit0 - Ext, Bit1 - Vcc, Bit2 - Int1, Bit3 - Int2
// End Analogue Inputs

// UART Section
#define HAL_UART_NUM_PORTS          2
#define HAL_USE_USART1              0           // Mapping to logical port
#define HAL_USE_USART2              1

#define EXTSER_USED                 2
// End UART Section

// TWI Section
#define EXTTWI_USED                 2       // I2C_Bus 1 - I2C1, 2 - I2C2
// End TWI Section

// LEDs
#define LED_On()                    GPIOB->BSRR = GPIO_BSRR_BS_2
#define LED_Off()                   GPIOB->BSRR = GPIO_BSRR_BR_2
#define LED_Init()                  hal_gpio_cfg(GPIOB, GPIO_Pin_2, DIO_MODE_OUT_PP)

// CC11 Section
#define HAL_USE_SPI2                1
#define CC11_USE_SPI                2
#define CC11_NSS_PIN                28                          // PB12
#define CC11_WAIT_LOW_MISO()        while(GPIOB->IDR & GPIO_Pin_14)
#define CC11_SELECT()               GPIOB->BRR = GPIO_Pin_12
#define CC11_RELEASE()              GPIOB->BSRR = GPIO_Pin_12
#define CC11_PHY                    1
#include "PHY/CC1101/cc11_phy.h"
// End CC11 Section

// Object's Dictionary Section
#define OD_MAX_INDEX_LIST           20
#define OD_DEV_UC_TYPE              'S'
#define OD_DEV_UC_SUBTYPE           '2'
#define OD_DEV_PHY1                 'C'
#define OD_DEV_PHY2                 'n'
#define OD_DEV_HW_TYP_H             '1'
#define OD_DEV_HW_TYP_L             '3'

#ifdef __cplusplus
}
#endif

#endif // _S2CN13_H