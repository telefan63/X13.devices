#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#if (defined STM32F0)
#include "stm32f0xx.h"
#else
#error unknown uC Family
#endif  //  uC Familie

void halEnterCritical(void);
void halLeaveCritical(void);
#define ENTER_CRITICAL_SECTION      halEnterCritical
#define LEAVE_CRITICAL_SECTION      halLeaveCritical

// Hardware specific options
#define portBYTE_ALIGNMENT          8
#define configTOTAL_HEAP_SIZE       2048

//////////////////////////////////////////////////////////////
// DIO/GPIO Section
#define DIO_PORT_POS                4
#define DIO_PORT_MASK               0x0F
#define DIO_PORT_TYPE               uint16_t

// GPIO compatibility
#define GPIO_Pin_0                 ((uint16_t)0x0001)  /*!< Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /*!< Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /*!< Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /*!< Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /*!< Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /*!< Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /*!< Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /*!< Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /*!< Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /*!< Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /*!< Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /*!< Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /*!< Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /*!< Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /*!< Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /*!< Pin 15 selected */

// DIO Modes
// 11-8 bits:   AF number
//  6-5 bits:   Low / Medium / Fast / High Speed
//  4-3 bits:   Input / Output / AF / Analog
//  2 bit:      Push-Pull / Open Drain
//  0-1 bits:   Float / PullUp / PullDown

#define DIO_AF_OFFS                 8

#define DIO_MODE_IN_FLOAT           0x00
#define DIO_MODE_IN_PU              0x01
#define DIO_MODE_IN_PD              0x02
#define DIO_MODE_OUT_PP             0x08
//#define DIO_MODE_OUT_OD             0x0C
#define DIO_MODE_OUT_PP_HS          0x68    // Output, push-pull, high speed
#define DIO_MODE_AF_PP              0x10
//#define DIO_MODE_AF_PU              0x11
//#define DIO_MODE_AF_PD              0x12
#define DIO_MODE_AF_OD              0x14
#define DIO_MODE_AF_PP_HS           0x70    // Alternative function, Push/pull, high speed
#define DIO_MODE_AIN                0x18

void hal_gpio_set(GPIO_TypeDef * GPIOx, uint16_t Mask);
void hal_gpio_reset(GPIO_TypeDef * GPIOx, uint16_t Mask);

void hal_dio_gpio_cfg(GPIO_TypeDef * GPIOx, uint16_t Mask, uint16_t Mode);

// DIO Section
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// SPI Section
#define HAL_SPI_MODE_0              0
#define HAL_SPI_MODE_1              1
#define HAL_SPI_MODE_2              2
#define HAL_SPI_MODE_3              3
#define HAL_SPI_MSB                 0
#define HAL_SPI_LSB                 4
#define HAL_SPI_8B                  0
#define HAL_SPI_16B                 8

void hal_spi_cfg(uint8_t port, uint8_t mode, uint32_t speed);
uint8_t hal_spi_exch8(uint8_t port, uint8_t data);
uint16_t hal_spi_exch16(uint8_t port, uint16_t data);
// SPI Section
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// IRQ subroutine
void SysTick_Handler(void);

//////////////////////////////////////////////////////////////
// HAL API
void INIT_SYSTEM(void);
void StartSheduler(void);

void eeprom_init_hw(void);
void eeprom_read(uint8_t *pBuf, uint32_t Addr, uint32_t Len);
void eeprom_write(uint8_t *pBuf, uint32_t Addr, uint32_t Len);

void _delay_ms(uint16_t ms);
void _delay_us(uint16_t us);

uint16_t hal_get_ms(void);
uint32_t hal_get_sec(void);
#define  hal_reboot     NVIC_SystemReset
uint16_t hal_RNG(void);

#ifdef __cplusplus
}
#endif

#endif  //  __HAL_H
