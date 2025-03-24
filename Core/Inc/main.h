/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

#include "stm32f3xx_ll_dma.h"
#include "stm32f3xx_ll_spi.h"
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_pwr.h"
#include "stm32f3xx_ll_gpio.h"

#include "stm32f3xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM1_LED_Pin GPIO_PIN_13
#define TIM1_LED_GPIO_Port GPIOC
#define USART2_TX_ESP_Pin GPIO_PIN_2
#define USART2_TX_ESP_GPIO_Port GPIOA
#define USART2_RX_ESP_Pin GPIO_PIN_3
#define USART2_RX_ESP_GPIO_Port GPIOA
#define GPIO_BME280_NCS_Pin GPIO_PIN_12
#define GPIO_BME280_NCS_GPIO_Port GPIOB
#define SPI2_SCK_BME280_Pin GPIO_PIN_13
#define SPI2_SCK_BME280_GPIO_Port GPIOB
#define SPI2_MISO_BME280_Pin GPIO_PIN_14
#define SPI2_MISO_BME280_GPIO_Port GPIOB
#define SPI2_MOSI_BME280_Pin GPIO_PIN_15
#define SPI2_MOSI_BME280_GPIO_Port GPIOB
#define GPIO_USB_PU_Pin GPIO_PIN_10
#define GPIO_USB_PU_GPIO_Port GPIOA
#define GPIO_FLASH_NCS_Pin GPIO_PIN_15
#define GPIO_FLASH_NCS_GPIO_Port GPIOA
#define SPI1_SCK_FLASH_Pin GPIO_PIN_3
#define SPI1_SCK_FLASH_GPIO_Port GPIOB
#define SPI1_MISO_FLASH_Pin GPIO_PIN_4
#define SPI1_MISO_FLASH_GPIO_Port GPIOB
#define SPI1_MOSI_FLASH_Pin GPIO_PIN_5
#define SPI1_MOSI_FLASH_GPIO_Port GPIOB
#define GPIO_ESP_NPD_EN_Pin GPIO_PIN_8
#define GPIO_ESP_NPD_EN_GPIO_Port GPIOB
#define GPIO_ESP_FLASH_Pin GPIO_PIN_9
#define GPIO_ESP_FLASH_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
