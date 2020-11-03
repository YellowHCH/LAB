/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/*#################    TIM PART    ###################################*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/*###############  DEBUG MODE  #####################*/
/* 在debug版本中定义 Debug用于调试，在release版本中注释掉 */
#define Debug


/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_nucleo.h"

#include "stdbool.h"
//#include "ATCMD.h"
//#include "stm32l4xx_hal_uart.h"


#include "string.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler

/* Exported functions ------------------------------------------------------- */

/* Private functions ---------------------------------------------------------*/
/*#########################  UART PART  ###########################################*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
/*############ USART1 PB6 == Tx;  PB7 = Rx ##########################*/
//#define USARTx                           USART1
//#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
//#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
//#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

//#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
//#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

///* Definition for USARTx Pins */
//#define USARTx_TX_PIN                    GPIO_PIN_6
//#define USARTx_TX_GPIO_PORT              GPIOB
//#define USARTx_TX_AF                     GPIO_AF7_USART1
//#define USARTx_RX_PIN                    GPIO_PIN_7
//#define USARTx_RX_GPIO_PORT              GPIOB
//#define USARTx_RX_AF                     GPIO_AF7_USART1

///* Definition for USARTx's NVIC */
//#define USARTx_IRQn                      USART1_IRQn
//#define USARTx_IRQHandler                USART1_IRQHandler

/*###################################*/
#define USARTx                           UART4
#define USARTx_CLK_ENABLE()              __HAL_RCC_UART4_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_UART4_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_UART4_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_0
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF8_UART4
#define USARTx_RX_PIN                    GPIO_PIN_1
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF8_UART4

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      UART4_IRQn
#define USARTx_IRQHandler                UART4_IRQHandler

/*#######################   #############################*/
//#define USARTx                           USART3
//#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()
//#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
//#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

//#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
//#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

///* Definition for USARTx Pins */
//#define USARTx_TX_PIN                    GPIO_PIN_4
//#define USARTx_TX_GPIO_PORT              GPIOC
//#define USARTx_TX_AF                     GPIO_AF7_USART3
//#define USARTx_RX_PIN                    GPIO_PIN_5
//#define USARTx_RX_GPIO_PORT              GPIOC
//#define USARTx_RX_AF                     GPIO_AF7_USART3


/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */
void SystemClock_Config(void);
extern void Error_Handler(void);//msp里面用到了
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void UART_SEND(uint8_t *tx, uint16_t size);
void UART_RECV(uint16_t size);
void UART_RECV_IT(uint16_t size);
/* transfer int to str */
void myItoa(int res, char *des);
/*0. ATT */
void ATRefresh(void);
void ATTest(void);
void ATMODE(int *ans);
/*1.	Turn on/off DAC/ADC Clock*/
void Turn_On_DAAD_Clk(void);
void Turn_Off_DAAD_Clk(void);
/* 2.	Turn on/off DAC Clock */
void Turn_On_DA_Clk(void);
void Turn_Off_DA_Clk(void);
/* 3.	Turn on/off ADC Clock */
void Turn_On_AD_Clk(void);
void Turn_Off_AD_Clk(void);
/* 4.	Signal PIN */
void PingTest(void);
void PingSaveDataToSD(double ChannelOneVolt, double ChannelTwoVolt, uint8_t choice);
/* 5.	Turn on/off Time Gain Control (TGC) */
void Turn_On_TGC(void);
void Turn_Off_TGC(void);
/* 6. Set Pulse Interval */
void SetPulseInterval(int interval);
/* 7.	Set Initial Gain (in volts) */
void SetInitGain(double volts);
/* 8.	Set End Gain (in volts)  */
void SetEndGain(double volts);
/* 9.	List files in SD Card */
void ListFilesInSDCard(void);
/* 10. Generate CW Pulse */
void GenCWPulse(uint16_t fre, uint16_t cycle, double gain);
/* 11. Generate LFM Pulse */
void GenLFMPulse(uint16_t f0, uint16_t f1, double gain, uint16_t Tsing);
/* 12. Generate LFM2 Pulse */
void GenLFM2Pulse(uint16_t fc, uint16_t f0, uint16_t f1, double gain, uint16_t Tsing);
/* 13. Read Low Speed ADC*/
double ReadLowSpeedADC(uint16_t channel);
/* 14. System Reset */
void SystemReset(void);
/* 15. Source Select
 * ATSR=0: select primary channel
 * ATSR=1: select sencdory channel
 * */
void SourceSelect(uint8_t choice);
/* 16. PA channle select
 * choice:
 * 0: turn off both channel
 * 1: turn channel 1
 * 2: turn channel 2
 * 3: turn both PA channel
 *
 * */
void PAChannleSelect(uint8_t choice);
/* 17. ATMSG='MESSAGE TO BE DISPLAY'*/
void ATMSG(uint8_t *msg);
void ATMSGLog(uint8_t *msg, uint16_t size);
/* For Special Condition*/
void PrintPowerData(void);
void Test_PrintLogInfo(void);
/* Test Uart Communication Betwen STM32 and Dsp*/
void TestUart(void);

/*############ relay control ##################*/
/* DSP_FPGA_MAIN_BOARD CONTROL
	 pin : PE0
*/
void Set_DSP_Relay(void);
void Reset_DSP_Relay(void);
/* Tx_Rx_BOARD CONTROL
	 pin : PE1 & PE2
*/
void Set_TxRx_Relay1(void);
void Reset_TxRx_Relay1(void);
void Set_TxRx_Relay2(void);
void Reset_TxRx_Relay2(void);
void Delay_Sec(int sec);
/*################## WDG PART #########################*/
#ifndef __at
#define __at(_addr) __attribute__ ((at(_addr)))
 
#endif
 
#ifndef __section
#define __section(_name) __attribute__ ((section(_name)))
 
#endif



#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
