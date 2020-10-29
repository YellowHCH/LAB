

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */




/** @addtogroup TIM_TimeBase
  * @{
  */
/*#################    TIM PART    ###################################*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* TIM handle declaration */

TIM_HandleTypeDef    TimHandle;
static GPIO_InitTypeDef  GPIO_InitStruct0;
static GPIO_InitTypeDef  GPIO_InitStruct1;
static GPIO_InitTypeDef  GPIO_InitStruct2;
static GPIO_InitTypeDef  GPIO_InitStruct3;
static GPIO_InitTypeDef  GPIO_InitStruct4;
static GPIO_InitTypeDef  GPIO_InitStruct5;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;
//#define SECONDS_PER_DAY 24*60*60
#define SECONDS_PER_DAY 4
//#define SECONDS_PER_HOUR 60*60
#define SECONDS_PER_HOUR 3
#define COUNT_ONCE_PER_DAY 		24*60*60/2
#define COUNT_TWICE_PER_DAY 	24*60*60/4
#define COUNT_TWRICE_PER_DAY 	24*60*60/6
//#define HOURS_PER_DAY 24
#define SECONDS_PER_TIM 2
#define SECONDS_PER_TASK 24*60*60
// COUNTS_PRE_TASK = SECONDS_PER_TASK / SECONDS_PER_TIM
#define COUNTS_PRE_TASK 60
#define COUNTS_PRE_TASK_TWO_DAY 24*60*60
#define COUNTS_PRE_TASK_WEEK    7*24*60*60/2
static uint32_t Count_per_task = COUNTS_PRE_TASK;
bool waiting_status = true;
static bool IsFirstTask = false;
static uint32_t waitOneHour = 0;
static uint32_t waitOneHour2 = 0;
///************************///
#define TASK_DAY_MAX	300
double PowerRecord_1[TASK_DAY_MAX] __at(0x10000020);
double PowerRecord_2[TASK_DAY_MAX] __at(0x10002420);
///************************///
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void Error_Handler(void);
static void UART_Error_Handler(void);
static void UART_SEND_Error_Handler(void);
static void UART_Recv_Error_Handler(void);
static void UART_CheckOK_Error_Handler(void);
static void UART_Callback_Error_Handler(void);
static void EXTI15_10_IRQHandler_Config(void);
void Set_Status(void);
void Task_Run(void);
void Task_End(void);
void Init_GPIO_Pins(void);
void Deinit_GPIO_Pins(void);
void Set_Pins(void);
void Reset_Pins(void);
void FlashingFast(void);
void FlashingSlow(void);
void UART_SEND(uint8_t *tx, uint16_t size);

/* Private functions ---------------------------------------------------------*/

/*#################    UART UART    ###################################*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TRANSMITTER_BOARD
//TIM_HandleTypeDef    TimHandle;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady = RESET;
__IO ITStatus UartRecvReady = RESET;
__IO uint32_t UserButtonStatus = 0;  /* set to 1 after User Button interrupt  */

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling **** ";
uint32_t UartSendLen = 0;
/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];
uint32_t UartRecvLen = 0;
char ENDFLAG[] = "\r";

/*#################### WDG PART  #################################*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* none init RAM area*/
uint16_t Date __at(0x10000000);
uint16_t State __at(0x10000008);
/* 20 seconds feed dog */
#define WDGTIME   30

/* IWDG and TIM handlers declaration */
static IWDG_HandleTypeDef   IwdgHandle;
TIM_HandleTypeDef           Input_Handle;
RCC_ClkInitTypeDef          RCC_ClkInitStruct;
RCC_OscInitTypeDef          RCC_OscInitStruct;

uint16_t tmpCC4[2] = {0, 0};
__IO uint32_t uwLsiFreq = 0;
__IO uint32_t uwCaptureNumber = 0;
/* Private function prototypes -----------------------------------------------*/
//void Error_Handler(void);

static uint32_t GetLSIFrequency(void);
static void FeedWDG(void);

void springspring(){
	BSP_LED_Toggle(LED2);
  HAL_Delay(50);
	BSP_LED_Toggle(LED2);
  HAL_Delay(150);

}
/*################ LED ##########################*/
/*################# LEDs ##########################*/
GPIO_InitTypeDef  GPIO_LED0;
GPIO_InitTypeDef  GPIO_LED1;
GPIO_InitTypeDef  GPIO_LED2;
GPIO_InitTypeDef  GPIO_LED3;

void Init_LED_Pins(){
	/* -1- Enable each GPIO Clock (to be able to program the configuration registers) */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* -2- Configure IOs in output push-pull mode to drive PE0 */
  GPIO_LED0.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_LED0.Pull  = GPIO_PULLUP;
  GPIO_LED0.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_LED0.Pin = GPIO_PIN_7;//GPIO_PIN_0
  HAL_GPIO_Init(GPIOE, &GPIO_LED0);//GPIOE
//	__HAL_RCC_GPIOA_CLK_DISABLE();
	
	/*-2- Configure IOs in output push-pull mode to drive PE1*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_LED1.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_LED1.Pull  = GPIO_PULLUP;
  GPIO_LED1.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_LED1.Pin = GPIO_PIN_8;//GPIO_PIN_1
  HAL_GPIO_Init(GPIOE, &GPIO_LED1);//GPIOE
//	__HAL_RCC_GPIOA_CLK_DISABLE();
	
	/*-2- Configure IOs in output push-pull mode to drive PE2*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_LED2.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_LED2.Pull  = GPIO_PULLUP;
  GPIO_LED2.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_LED2.Pin = GPIO_PIN_9;//GPIO_PIN_2
  HAL_GPIO_Init(GPIOE, &GPIO_LED2);//GPIOE
//	__HAL_RCC_GPIOB_CLK_DISABLE();
	
	/*-2- Configure IOs in output push-pull mode to drive PE3*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_LED3.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_LED3.Pull  = GPIO_PULLUP;
  GPIO_LED3.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_LED3.Pin = GPIO_PIN_10;//GPIO_PIN_3
  HAL_GPIO_Init(GPIOE, &GPIO_LED3);//GPIOE

}

void Deinit_LED_Pins(){
	HAL_GPIO_DeInit(GPIOE, 7);
	HAL_GPIO_DeInit(GPIOE, 8);
	HAL_GPIO_DeInit(GPIOE, 9);
	HAL_GPIO_DeInit(GPIOE, 10);
	
}


void Set_LED_Pins(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
}
void Reset_LED_Pins(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
}


/*###################### SYSTEM DESIGN ########################*/
/* Time struct , used to record current time*/
typedef struct {
	uint16_t Day;
	uint16_t Hour;
	uint16_t Minute;
	uint16_t Second;
}CurrentTime;

void UpdateCurrentTime(CurrentTime *time){
	/* time mmust be correct initialized*/
	time->Second += SECONDS_PER_TIM;
	if(time->Second >= 60){
		time->Second = 0;
		time->Minute += 1;
		if(time->Minute >= 60){
			time->Minute = 0;
			time->Hour += 1;
			if(time->Hour >= 24){
				time->Hour = 0;
				time->Day += 1;
			}
		}
	}
}

void InitCurrentTime(CurrentTime *time){
	time->Day = 0;
	time->Hour = 0;
	time->Minute = 0;
	time->Second = 0;
}
CurrentTime CurrentTimeInst __at(0x10000016);

/*################ main program#####################*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/*启动状态*/
	if(State == 0xABCD){
		/*hot reset*/
		++Date;
		UpdateCurrentTime(&CurrentTimeInst);
	}
	else{
		/*absolute reset*/
		State = 0xABCD;
		Date = 0;
		InitCurrentTime(&CurrentTimeInst);
		// init PowerRecord
		memset(PowerRecord_1, 0, sizeof(PowerRecord_1) / sizeof(double));
		memset(PowerRecord_2, 0, sizeof(PowerRecord_2) / sizeof(double));
	}
  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 80 MHz */
  SystemClock_Config();

  /* Configure LED2 */
//  BSP_LED_Init(LED2);
	Init_LED_Pins();
	Reset_LED_Pins();Reset_LED_Pins();Reset_LED_Pins();Reset_LED_Pins();
	Set_LED_Pins();
	Init_GPIO_Pins();
	Reset_Pins();// close pins
	
	/* -2- Configure External line 13 (connected to PC.13 pin) in interrupt mode */
//  EXTI15_10_IRQHandler_Config();

/*######################  Configure the UART peripheral  ########################*/
/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance            = USARTx;

  UartHandle.Init.BaudRate       = 115200; //115200
  UartHandle.Init.WordLength     = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits       = UART_STOPBITS_1;
  UartHandle.Init.Parity         = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode           = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling   = UART_OVERSAMPLING_16;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;




	/* 串口初始化 */
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
  {
    UART_Error_Handler();
  }  
  if(HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    UART_Error_Handler();
  }


  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
      TIM3CLK = PCLK1
      PCLK1 = HCLK
      => TIM3CLK = HCLK = SystemCoreClock
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32l4xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz
	SystemCoreClock = 4MHz
	
	*/
  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000);
//	uwPrescalerValue = 40000 - 1; // 4M / 40000 = 100Hz
	
  /* Set TIMx instance */
  TimHandle.Instance = TIM3;// TIMx

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 10000 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue*SECONDS_PER_TIM - 1; // 24s 
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
	
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }
	

	
	
/*################ configure WDG #########################*/
	/*##-1- Check if the system has resumed from IWDG reset ####################*/
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
    /* IWDGRST flag set: Turn LED2 on */
//    BSP_LED_On(LED2);

//    /* Insert 4s delay */
    HAL_Delay(400);

//    /* Notification done: Turn LED2 off */
//    BSP_LED_Off(LED2);
  }

  /* Clear reset flags in any cases */
  __HAL_RCC_CLEAR_RESET_FLAGS();

  /*##-2- Get the LSI frequency: TIM16 is used to measure the LSI frequency ###*/
  uwLsiFreq = GetLSIFrequency();
  
  /*##-3- Configure & Start the IWDG peripheral #########################################*/
  /* Set counter reload value to obtain 1 sec. IWDG TimeOut.
     IWDG counter clock Frequency = uwLsiFreq
     Set Prescaler to 32 (IWDG_PRESCALER_32)
     Timeout Period = (Reload Counter Value * 32) / uwLsiFreq
     So Set Reload Counter Value = (1 * uwLsiFreq) / 32 */
  IwdgHandle.Instance = IWDG;
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_256;
  IwdgHandle.Init.Reload = (WDGTIME * uwLsiFreq / 256);
  IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

  if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
/****************************** Standby Mode *******************************************************/
// stay in standby mode for 24 hour
while (COUNT_ONCE_PER_DAY > waitOneHour){
        if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, 7) != HAL_OK)
  {
    UART_Recv_Error_Handler();
  }
	while (UartReady != SET)
  {
        FeedWDG();
  }
        // deal with ATRCMD
        char *cmd = (char*)aRxBuffer;
        // ATRLOGD, print log data to uart port
        if(*(cmd+0) == 'A' && *(cmd+1) == 'T' && *(cmd+2) == 'R' && *(cmd+3) == 'L' && *(cmd+4) == 'O' && *(cmd+5) == 'G' && *(cmd+6) == 'D'  )
{
                PrintPowerData();
}
        // ATRCONSW, swith to normal mode
        else if(*(cmd+0) == 'A' && *(cmd+1) == 'T' && *(cmd+2) == 'R' && *(cmd+3) == 'C' && *(cmd+4) == 'O' && *(cmd+5) == 'N' && *(cmd+6) == 'S'){
                break;
}
/****************************** Normal Mode ********************************************************/
// Into normal mode, send atcmd to DSP
// flash twice to indicate into normal mode
#ifdef Debug
	FlashingFast();
	FlashingFast();
#endif
  while (1)
  {
		
		Set_LED_Pins();
		HAL_Delay(50);
		Reset_LED_Pins();
		HAL_Delay(50);
		FeedWDG();
		
		/* 关闭串口，节省功率*/
//		if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
//		{
//			UART_Error_Handler();
//		}  
		
		// waiting for singal , signal was generated by TIM counter
  	waiting_status = true;
		while(waiting_status){
			FeedWDG();
		}
		waiting_status = true;

		// run task		
		/* 拉高继电器控制引脚电平，给控制板和发射板上电 */
		Set_DSP_Relay();
		//Set_TxRx_Relay1();
		//Set_TxRx_Relay2();
		// 等待几秒钟，DSP上电初始化
//		for(int count = 0; count < 5; ++count){
//			FeedWDG();
//			HAL_Delay(1000);
//		}
		Delay_Sec(5);
		/* 打开串口，进行指令传输 */
		if(HAL_UART_Init(&UartHandle) != HAL_OK)
		{
			UART_Error_Handler();
		}
		HAL_Delay(50);// 等串口初始化完成
//		Task_Run();
		TestUart();
		/* waiting for task end*/
		// close task
		/* 拉低继电器电平，主板发射板断电*/
		Delay_Sec(5);// temp task, delay 5 seconds
		Reset_DSP_Relay();
		//Reset_TxRx_Relay1();
		//Reset_TxRx_Relay2();
		
//		Task_End();
		/* Refresh IWDG: reload counter 
		*/
		/* Feed WDG , 10 seconds once*/
		FeedWDG();
		
  }
}
/*#################    TIM PART    ###################################*/

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //BSP_LED_Toggle(LED2);
	++waitOneHour;
	++waitOneHour2;
	UpdateCurrentTime(&CurrentTimeInst);
	if(waitOneHour >= Count_per_task && waitOneHour >= Count_per_task){
		waitOneHour = 0;
		waitOneHour2 = 0;
		waitOneHour = 0;
		waitOneHour2 = 0;
		waitOneHour = 0;
		waitOneHour2 = 0;
		Set_Status();
	}
	
//	FeedWDG();
}

void Delay_Sec(int sec){
	// limit to 100 seconds
	if(sec > 999){sec = 100;}
	for(int count = 0; count < sec; ++count){
			FeedWDG();
			HAL_Delay(1000);
		}
}

/**/
void Set_Status(){
	waiting_status = false;
	waiting_status = false;
	waiting_status = false;
}

void Task_Run(){
	/* set pins*/
	Set_Pins();
	HAL_Delay(100);
	/*1.	Turn on/off DAC/ADC Clock*/
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	Turn_On_DAAD_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	Turn_Off_DAAD_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 2.	Turn on/off DAC Clock */
	Turn_On_DA_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	Turn_Off_DA_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 3.	Turn on/off ADC Clock */
	Turn_On_AD_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	Turn_Off_AD_Clk();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 4.	Signal PIN */
	PingTest();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	char chtmp[] = "hchchchc";
	PingSaveDataToSD(1.23456, 6.54321, 1);HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 5.	Turn on/off Time Gain Control (TGC) */
	Turn_On_TGC();HAL_Delay(10);
	/* Feed WDG , 10 seconds once*/
		FeedWDG();
	 Turn_Off_TGC();HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 6. Set Pulse Interval */
	 SetPulseInterval(3);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 7.	Set Initial Gain (in volts) */
	 SetInitGain(1.2);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 8.	Set End Gain (in volts)  */
	 SetEndGain(2.1);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 9.	List files in SD Card */
	 ListFilesInSDCard();HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 10. Generate CW Pulse */
	 GenCWPulse(200, 10, 1.0);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 11. Generate LFM Pulse */
	 GenLFMPulse(300, 301, 0.9, 12);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 12. Generate LFM2 Pulse */
	 GenLFM2Pulse(400, 1, 2, 1.1, 14);HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
	/* 13. System Reset */
	 SystemReset();HAL_Delay(10);
	 /* Feed WDG , 10 seconds once*/
		FeedWDG();
		/**/
	Set_LED_Pins();
	HAL_Delay(10);
	Reset_LED_Pins();
	HAL_Delay(10);
	Set_LED_Pins();
	HAL_Delay(10);
	Reset_LED_Pins();
	HAL_Delay(10);
	Set_LED_Pins();
	HAL_Delay(10);
	Reset_LED_Pins();
	HAL_Delay(10);
	Set_LED_Pins();
	HAL_Delay(10);
	Reset_LED_Pins();
	HAL_Delay(10);
	
}
void Task_End(){
	Set_LED_Pins();
	HAL_Delay(20);
	Reset_LED_Pins();
	HAL_Delay(20);
	Set_LED_Pins();
	HAL_Delay(20);
	Reset_LED_Pins();
	HAL_Delay(20);
	Reset_Pins();
}

/*####### GPIO_Pins ###############*/
void Init_GPIO_Pins(){
	/* -1- Enable each GPIO Clock (to be able to program the configuration registers) */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* -2- Configure IOs in output push-pull mode to drive PE0 */
//  GPIO_InitStruct0.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct0.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct0.Pull  = GPIO_PULLUP;
  GPIO_InitStruct0.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct0.Pin = GPIO_PIN_0;//GPIO_PIN_0
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct0);//GPIOE
//	__HAL_RCC_GPIOA_CLK_DISABLE();
	
	/*-2- Configure IOs in output push-pull mode to drive PE1*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	GPIO_InitStruct1.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct1.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct1.Pull  = GPIO_PULLUP;
  GPIO_InitStruct1.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct1.Pin = GPIO_PIN_1;//GPIO_PIN_1
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct1);//GPIOE
//	__HAL_RCC_GPIOA_CLK_DISABLE();
	
	/*-3- Configure IOs in output push-pull mode to drive PE2*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	GPIO_InitStruct2.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct2.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct2.Pull  = GPIO_PULLUP;
  GPIO_InitStruct2.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct2.Pin = GPIO_PIN_2;//GPIO_PIN_2
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct2);//GPIOE
//	__HAL_RCC_GPIOB_CLK_DISABLE();
	
	/*-4- Configure IOs in output push-pull mode to drive PE3*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	GPIO_InitStruct3.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct3.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct3.Pull  = GPIO_PULLUP;
  GPIO_InitStruct3.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct3.Pin = GPIO_PIN_3;//GPIO_PIN_3
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct3);//GPIOE
//	__HAL_RCC_GPIOB_CLK_DISABLE();

	/*-2- Configure IOs in output push-pull mode to drive PE3*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	GPIO_InitStruct4.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct4.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct4.Pull  = GPIO_PULLUP;
  GPIO_InitStruct4.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct4.Pin = GPIO_PIN_4;//GPIO_PIN_3
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct4);//GPIOE
	
	/*-2- Configure IOs in output push-pull mode to drive PE3*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	GPIO_InitStruct5.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct5.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct5.Pull  = GPIO_PULLUP;
  GPIO_InitStruct5.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct5.Pin = GPIO_PIN_5;//GPIO_PIN_3
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct5);//GPIOE
}

void Deinit_GPIO_Pins(){
	HAL_GPIO_DeInit(GPIOE, 0);
	HAL_GPIO_DeInit(GPIOE, 1);
	HAL_GPIO_DeInit(GPIOE, 2);
	HAL_GPIO_DeInit(GPIOE, 3);
	HAL_GPIO_DeInit(GPIOE, 4);
	HAL_GPIO_DeInit(GPIOE, 5);
	
}


void Set_Pins(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
}
void Reset_Pins(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* relay control*/
void Set_DSP_Relay(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

void Reset_DSP_Relay(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
}

void Set_TxRx_Relay1(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
}
void Reset_TxRx_Relay1(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
}

void Set_TxRx_Relay2(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}
void Reset_TxRx_Relay2(void){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
}

void FlashingFast(void){
	#ifdef Debug
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
		Set_LED_Pins();
		HAL_Delay(20);
		Reset_LED_Pins();
		HAL_Delay(20);
	#endif
}
void FlashingSlow(void){
	#ifdef Debug
		Set_LED_Pins();
		HAL_Delay(250);
		Reset_LED_Pins();
		HAL_Delay(250);
		Set_LED_Pins();
		HAL_Delay(250);
		Reset_LED_Pins();
		HAL_Delay(250);
		Set_LED_Pins();
		HAL_Delay(250);
		Reset_LED_Pins();
		HAL_Delay(250);
	#endif
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 40
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  Configures EXTI lines 10 to 15 (connected to PC.13 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTI15_10_IRQHandler_Config(void)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* Configure PC.13 pin as input floating */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable and set EXTI lines 10 to 15 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_13)
  {
    /* Toggle LED2 */
    BSP_LED_Toggle(LED2);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while (1)
  {
  }
}


/*#################    UART PART    ###################################*/
void UART_Error_Handler(void){
	while(1){
		HAL_Delay(10);
		BSP_LED_Toggle(LED2);
	}
}

static void UART_SEND_Error_Handler(void){
	while(1){
		HAL_Delay(10);
		BSP_LED_Toggle(LED2);
	}
}
static void UART_Recv_Error_Handler(void){
	while(0){
		HAL_Delay(10);
		BSP_LED_Toggle(LED2);
	}
}

void UART_CheckOK_Error_Handler(void){
	while(0){
		HAL_Delay(10);
		BSP_LED_Toggle(LED2);
	}
}

void UART_Callback_Error_Handler(void){
	while(1){
		HAL_Delay(10);
		BSP_LED_Toggle(LED2);
	}
}
/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of IT Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;

}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
  UartReady = SET;

}

void UART_SEND(uint8_t *tx, uint16_t size){
	strncpy((char*)aTxBuffer, (char*)tx, size);
	char *ptr = (char*)aTxBuffer;
//	for(int idx = 0; idx < size; ++idx){
//		if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)(ptr+idx), 1)!= HAL_OK) //bufSize TXBUFFERSIZE
//		{
//			//Error_Handler(); 
//			UART_SEND_Error_Handler();
//		}
//		HAL_Delay(1);
//	}
	if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)(ptr), size)!= HAL_OK) //bufSize TXBUFFERSIZE
		{
			//Error_Handler(); 
			UART_SEND_Error_Handler();
		}
	while(UartReady != SET){}
			/* Reset transmission flag */
  UartReady = RESET;
	/* Feed WDG , 10 seconds once*/
	FeedWDG();
//	if(HAL_UART_Transmit(&UartHandle, (uint8_t*)aTxBuffer, size, 5000)!= HAL_OK) //bufSize TXBUFFERSIZE
//  {
//    //Error_Handler(); 
//		UART_SEND_Error_Handler();
//  }
}
void UART_RECV(uint16_t size){
	#if 0
        if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, size) != HAL_OK)
  {
    UART_Recv_Error_Handler();
  }
	while (UartReady != SET)
  {
  }

  /* Reset transmission flag */
  UartReady = RESET;
        #endif
	#if 1
	if(HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, size, 500) != HAL_OK)
  {
    //Error_Handler();  
		UART_Recv_Error_Handler();
  }
	#endif
//	/* 约定每次以'\n'作为结束*/
//	uint8_t *recv = aRxBuffer;
//	*recv = '\0';
//	/* set UartRecvLen = 0 before this function */
//	while(*recv != '\n'){
//		if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)recv, 1) != HAL_OK)
//		{
//			//Error_Handler();  
//			UART_Recv_Error_Handler();
//		}
//		while(UartRecvReady != SET){}
//		/* Reset transmission flag */
//		UartRecvReady = RESET;
//		++recv;
//		++UartRecvLen;
//	}
}

void UART_RECV_IT(uint16_t size){
	#if 1
        if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, size) != HAL_OK)
  {
    UART_Recv_Error_Handler();
  }
	while (UartReady != SET)
  {
        FeedWDG();
  }

  /* Reset transmission flag */
  UartReady = RESET;
        #endif
	#if 0
	if(HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, size, 500) != HAL_OK)
  {
    //Error_Handler();  
		UART_Recv_Error_Handler();
  }
	#endif
//	/* 约定每次以'\n'作为结束*/
//	uint8_t *recv = aRxBuffer;
//	*recv = '\0';
//	/* set UartRecvLen = 0 before this function */
//	while(*recv != '\n'){
//		if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)recv, 1) != HAL_OK)
//		{
//			//Error_Handler();  
//			UART_Recv_Error_Handler();
//		}
//		while(UartRecvReady != SET){}
//		/* Reset transmission flag */
//		UartRecvReady = RESET;
//		++recv;
//		++UartRecvLen;
//	}
}

void myItoa(int res, char *des){
	int tmp = res;
	int len = 0;
	while(tmp){
		++len;
		tmp /= 10;
	}
	for(int idx = len - 1; idx >= 0; --idx){
		int bitnum = res - (res / 10)*10;
		res /= 10;
		*(des + idx) = '0' + bitnum;
	}
	*(des + len) = '\0';
}

/* Test Uart Communication Betwen STM32 and Dsp*/
void TestUart(void){
        if(!IsFirstTask){
            IsFirstTask = true;
            // do init job

        }
//	Set_Pins();
	HAL_Delay(100);
//	// test 0
  	ATTest();
  	HAL_Delay(500);
		ATRefresh();
//	// test 1
//	Turn_On_DAAD_Clk();
//	HAL_Delay(500);
//	Turn_Off_DAAD_Clk();
//	HAL_Delay(500);
//	// test 2
//	Turn_On_DA_Clk();
//	HAL_Delay(500);
//	Turn_Off_DA_Clk();
//	HAL_Delay(500);
//	// test 3
//	ATRefresh();Turn_On_AD_Clk();ATRefresh();
//	HAL_Delay(500);
//	Turn_Off_AD_Clk();
//	HAL_Delay(500);
	// test 4
	PingTest();
	HAL_Delay(500);
	ATRefresh();
	PingSaveDataToSD(1.23,3.21, 1);
	ATRefresh();
	HAL_Delay(500);
//	// test 5
//	Turn_On_TGC();
//	HAL_Delay(500);
//	Turn_Off_TGC();
//	HAL_Delay(500);
//	// test 6
//	SetPulseInterval(100);
//	HAL_Delay(500);
//	// test 7
//	SetInitGain(0.8);
//	HAL_Delay(500);
//	// test 8
//	SetEndGain(2.1);
//	HAL_Delay(500);
//	// test 9 not ok
//	ListFilesInSDCard();
//	HAL_Delay(500);
//	// test 10
//	GenCWPulse(200, 1, 1.0);
//	HAL_Delay(500);
//	// test 11
//	GenLFMPulse(120, 150, 0.9, 1);
//	HAL_Delay(500);
//	// test 12
//	GenLFM2Pulse(400, 1, 2, 1.1, 1);
//	HAL_Delay(500);
// test 13
	
//	ATRefresh();ReadLowSpeedADC(2);ATRefresh();
//	HAL_Delay(50);
//	Reset_Pins();
// test 14
//	SystemReset();
//// test 15
//	HAL_Delay(500);
//	SourceSelect(0);
//	HAL_Delay(500);
//	// test 16
//	ATRefresh();
//	PAChannleSelect(0);
//	HAL_Delay(500);
}


/* 0. AT Test*/
// 读空刷新DSP
void ATRefresh(void){
	FeedWDG();
//	uint8_t cmd[] = "AT\r";
//	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	while(1){
		
		if(HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, 1, 5) != HAL_OK)
		{
			//Error_Handler();  
//			UART_Recv_Error_Handler();
			// 如果读取超时，说明读空了，则跳出while
			break;
		}
	}
	HAL_Delay(5);
}

void ATTest(void){
	uint8_t cmd[] = "ATT\r";
//	uint8_t cmd[] = "ATPING\r";
//	uint8_t cmd[] = "ATCODACLK=1\r";
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
		UART_CheckOK_Error_Handler(); 
	#ifdef Debug
		FlashingSlow();
	FlashingFast();
	#endif
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 1.	Turn on/off DAC/ADC Clock */
void Turn_On_DAAD_Clk(){
	uint8_t cmd[] = "ATCODACLK=1\r";
//	strcat((char*)cmd, "\r\n");
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));
//	printf("%s\n", cmd);

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

void Turn_Off_DAAD_Clk(){
	uint8_t cmd[] = "ATCODACLK=0\r";
//	strcat((char*)cmd, "\r\n");
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 2.	Turn on/off DAC Clock */
void Turn_On_DA_Clk(){
	uint8_t cmd[] = "ATDACLK=1\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
void Turn_Off_DA_Clk(){
	uint8_t cmd[] = "ATDACLK=0\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 3.	Turn on/off ADC Clock */
void Turn_On_AD_Clk(){
	uint8_t cmd[] = "ATADCLK=1\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
void Turn_Off_AD_Clk(){
	uint8_t cmd[] = "ATADCLK=0\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 4.	Signal PIN */
void PingTest(){
	uint8_t cmd[] = "ATPING\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));
//	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	//UART_RECV(7);
	HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, 7, 1000);
	char *ch = (char*)aRxBuffer;
	
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/*
* need more than 20s, fuck
* choice:
* 1-> CW
* 2-> LFM
* 3-> PALFM
* others-> CW
*/
void PingSaveDataToSD(double ChannelOneVolt, double ChannelTwoVolt, uint8_t choice){
	/* name == "d0000001"*/
	uint8_t cmd[80] = "ATPING=\"d0000001.bin\"";
        switch(choice){
        case 1:
                cmd[8] = 'C';
                break;
        case 2:
                cmd[8] = 'L';
                break;
        case 3:
                cmd[8] = 'P';
                break;
        default: break;
        }
	char tmp[10] = "";
	sprintf(tmp, "%07hu", Date);
	++Date;
	strncpy((char*)(cmd+9), tmp, strlen(tmp));
	char tmpDay[10] = "";
	char tmpHour[10] = "";
	char tmpMin[10] = "";
	char tmpSec[10] = "";
	sprintf(tmpDay, "%hu", CurrentTimeInst.Day);
	sprintf(tmpHour, "%hu", CurrentTimeInst.Hour);
	sprintf(tmpMin, "%hu", CurrentTimeInst.Minute);
	sprintf(tmpSec, "%hu", CurrentTimeInst.Second);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, "Day:");
	strcat((char*)cmd, tmpDay);
	strcat((char*)cmd, "Hour:");
	strcat((char*)cmd, tmpHour);
	strcat((char*)cmd, "Min:");
	strcat((char*)cmd, tmpMin);
	strcat((char*)cmd, "Sec:");
	strcat((char*)cmd, tmpSec);
//	strcat((char*)cmd, ",");
	char volt1[10] = "";
	char volt2[10] = "";
	sprintf(volt1, "%.6f", ChannelOneVolt);
	sprintf(volt2, "%.6f", ChannelTwoVolt);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, volt1);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, volt2);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

// SD writing speed is slow, need to wait 22 seconds
/*
        it's hard to decide how long to wait, wait for 23s may not sutiable
*/
	//Delay_Sec(23);
	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, RXBUFFERSIZE);
	//UART_RECV(7);
        // busy wait 30s
	HAL_UART_Receive(&UartHandle, (uint8_t *)aRxBuffer, 7, 30*1000);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 5.	Turn on/off Time Gain Control (TGC) */
void Turn_On_TGC(){
	uint8_t cmd[] = "ATTGC=1\r";

	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
void Turn_Off_TGC(){
	uint8_t cmd[] = "ATTGC=0\r";
	
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 6. Set Pulse Interval */
void SetPulseInterval(int interval){
	uint8_t cmd[20] = "ATPUSINT=";
	uint8_t tmp[10] = "";
	myItoa(interval, (char*)tmp);
	strcat((char*)cmd, (char*)tmp);

	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
//	UART_SEND(cmd, strlen((char*)cmd));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 7.	Set Initial Gain (in volts) */
void SetInitGain(double volts){
	char tmp[10] = "";
	/*sprintf大量占用栈内存，易造成内存溢出，后面可以优化此处*/
	sprintf(tmp, "%f", volts);
	uint8_t cmd[20] = "ATIGAIN=";
	strcat((char*)cmd, (char*)tmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 8.	Set End Gain (in volts)  */
void SetEndGain(double volts){
	char tmp[10] = "";
	/*sprintf大量占用栈内存，易造成内存溢出，后面可以优化此处*/
	sprintf(tmp, "%f", volts);
	uint8_t cmd[20] = "ATEGAIN=";
	strcat((char*)cmd, (char*)tmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	///UART_RECV(6);
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 9.	List files in SD Card */
void ListFilesInSDCard(){
	uint8_t cmd[] = "ATSDLS\r";
	//strcat((char*)cmd, ENDFLAG);
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingFast();
		//FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 10. Generate CW Pulse */
void GenCWPulse(uint16_t fre, uint16_t cycle, double gain){
	char freTmp[10] = "";
	char cycTmp[10] = "";
	char gainTmp[10] = "";
	sprintf(freTmp, "%hu", fre);
	sprintf(cycTmp, "%hu", cycle);
	sprintf(gainTmp, "%f", gain);
	
	uint8_t cmd[40] = "ATGCW=";
	strcat((char*)cmd, freTmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, cycTmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, gainTmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 11. Generate LFM Pulse */
void GenLFMPulse(uint16_t f0, uint16_t f1, double gain, uint16_t Tsing){
	char f0Tmp[10] = "";
	char TsingTmp[10] = "";
	char f1Tmp[10] = "";
	char gainTmp[10] = "";
	//uint8_t TsingTmp[10] = "";
	sprintf(f0Tmp, "%hu", f0);
	sprintf(f1Tmp, "%hu", f1);
	sprintf(gainTmp, "%f", gain);
	sprintf(TsingTmp, "%hu", Tsing);
	
	uint8_t cmd[80] = "ATGLFM=";
	strcat((char*)cmd, f0Tmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, f1Tmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, gainTmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, TsingTmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* 12. Generate LFM2 Pulse */
void GenLFM2Pulse(uint16_t fc, uint16_t f0, uint16_t f1, double gain, uint16_t Tsing){
	char fcTmp[10] = "";
	char f0Tmp[10] = "";
	char f1Tmp[10] = "";
	char gainTmp[10] = "";
	char TsingTmp[10] = "";
	sprintf(fcTmp, "%hu", fc);
	sprintf(f0Tmp, "%hu", f0);
	sprintf(f1Tmp, "%hu", f1);
	sprintf(gainTmp, "%f", gain);
	sprintf(TsingTmp, "%hu", Tsing);
	
	uint8_t cmd[40] = "ATGLFM2=";
	strcat((char*)cmd, fcTmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, f0Tmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, f1Tmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, gainTmp);
	strcat((char*)cmd, ",");
	strcat((char*)cmd, TsingTmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 13. Read Low Speed ADC */
/* this function is 难搞 */
double ReadLowSpeedADC(uint16_t channel){
	char tmp[1] = "";
	sprintf(tmp, "%01hu", channel);
	uint8_t cmd[20] = "ATLSADC=";
	strcat((char*)cmd, tmp);
	strcat((char*)cmd, ENDFLAG);
	// send cmd
//	UART_SEND(cmd, (COUNTOF(cmd) - 1));
	UART_SEND(cmd, strlen((char*)cmd));

	// wait for ADC , need to check how many bytes
	memset(aRxBuffer, 0, 17);
	UART_RECV(17); // size = "\r\n" + buffer[0:7] + "\r\nOK\r\n" + ">" = 17
	char *ch = (char*)(aRxBuffer);
	if(*ch != '\r' || *(ch+1) != '\n' || *(ch+10) != '\r' || *(ch+11) != '\n' || *(ch+12) != 'O' || *(ch+13) != 'K' || *(ch+14) != '\r' || *(ch+15) != '\n' || *(ch+16) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler(); 
	}
	else{
		#ifdef Debug
		FlashingFast();
		#endif
	}
	double power = 0.0;
	char valStr[8] = "";
	strncpy(valStr, ch+2, 8);
	power = atof(valStr);
        // startge for power manage
        #if 0
        if(power < 0.5){
                Count_per_task = COUNTS_PRE_TASK_WEEK;
        }
        else if(power < 1){
                Count_per_task = COUNTS_PRE_TASK_TWO_DAY;
        }
        else{
                ;
        }
        #endif
	// record to noninit ram
				switch(channel){
					case 2:
						PowerRecord_1[Date % TASK_DAY_MAX] = power;
						break;
					case 3:
						PowerRecord_2[Date % TASK_DAY_MAX] = power;
						break;
					default: break;
				}
	return power;
}

/* 14. System Reset */
void SystemReset(){
	uint8_t cmd[] = "ATRESET\r";
//	strcat((char*)cmd, "\r\n");
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 15. Source Select
 * ATSRSEL=0: select primary channel
 * ATSRSEL=1: select sencdory channel
 * */
void SourceSelect(uint8_t choice){
	uint8_t cmd[] = "ATSRSEL=0\r";
	uint8_t cmt[20] = "ATSRSEL=0\r";
//	if( choice == 1){
//		cmd[8] = (uint8_t)('1');
//	}
	switch(choice){
        case 0:
                break;
        case 1:
                cmd[8] = '1';
                break;
        default: return;
        }
//	strcat((char*)cmd, "\r\n");
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}

/* 16. PA channle select
 * choice:
 * 0: turn off both channel
 * 1: turn channel 1
 * 2: turn channel 2
 * 3: turn both PA channel
 *
 * */
void PAChannleSelect(uint8_t choice){
	uint8_t cmd[] = "ATPACSEL=0\r";
        switch(choice){
        case 0:
                break;
        case 1:
                cmd[9] = '1';
                break;
        case 2:
                cmd[9] = '2';
                break;
        case 3:
                cmd[9] = '3';
                break;
        default: return;
        }
//	strcat((char*)cmd, "\r\n");
	// send cmd
	UART_SEND(cmd, (COUNTOF(cmd) - 1));

	// wait for OK
	UartRecvLen = 0;
	memset(aRxBuffer, 0, 7);
	UART_RECV(7);
	char *ch = (char*)aRxBuffer;
	if(*(ch+0) != '\r' || *(ch+1) != '\n' || *(ch+2) != 'O' || *(ch+3) != 'K' || *(ch+4) != '\r' || *(ch+5) != '\n' || *(ch+6) != '>'){
	#ifdef Debug
		FlashingSlow();
	#endif
		UART_CheckOK_Error_Handler();
	}
	else{
	#ifdef Debug
		FlashingFast();
	#endif
	}
}
/* ########## */
void PrintPowerData(){
	//sprintf(gainTmp, "%f", gain);
	uint8_t str[100] = "channel-2\n";
	UART_SEND(str, (COUNTOF(str) - 1));
	memset(str, 0, sizeof(str) / sizeof(uint8_t));
	double power = 0;
	char s_power[10] = {0};
	FlashingFast();
	for(int idx = 0; idx < TASK_DAY_MAX; ++idx){
		power = PowerRecord_1[idx];
		sprintf(s_power, "%f", power);
		sprintf((char*)str, "%d: ", idx);
		strcat((char*)str, s_power);
		strcat((char*)str, "\n");
		UART_SEND(str, (COUNTOF(str) - 1));
		memset(str, 0, sizeof(str) / sizeof(uint8_t));
		FeedWDG();
	}
	
	sprintf((char*)str, "\nchannel-3\n");
	UART_SEND(str, (COUNTOF(str) - 1));
	memset(str, 0, sizeof(str) / sizeof(uint8_t));
	memset(s_power, 0, sizeof(s_power));
	FlashingFast();
	for(int idx = 0; idx < TASK_DAY_MAX; ++idx){
		power = PowerRecord_2[idx];
		sprintf(s_power, "%f", power);
		sprintf((char*)str, "%d: ", idx);
		strcat((char*)str, s_power);
		strcat((char*)str, "\n");
		UART_SEND(str, (COUNTOF(str) - 1));
		memset(str, 0, sizeof(str) / sizeof(uint8_t));
		FeedWDG();
	}
	
}



/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  //Error_Handler();
	UART_Callback_Error_Handler();
}

/*#################  WDG PART #########################*/
/**
  * @brief  Configures TIM16 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
static void FeedWDG(void){
	if(HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
    {
      /* Refresh Error */
      Error_Handler();
    }
}


static uint32_t GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;


  /* Configure the TIM peripheral *********************************************/ 
  /* Set TIMx instance */  
  Input_Handle.Instance = TIM16;
  
  /* TIM16 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM16 CH1.
     The Rising edge is used as active edge.
     The TIM16 CCR1 is used to compute the frequency value. 
  ------------------------------------------------------------ */
  Input_Handle.Init.Prescaler         = 0;
  Input_Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  Input_Handle.Init.Period            = 0xFFFF;
  Input_Handle.Init.ClockDivision     = 0;
  if(HAL_TIM_IC_Init(&Input_Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Connect internally the TIM16_CH1 Input Capture to the LSI clock output */
  HAL_TIMEx_RemapConfig(&Input_Handle, TIM_TIM16_TI1_LSI);
  
  /* Configure the Input Capture of channel 1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&Input_Handle, &TIMInput_Config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&Input_Handle, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the TIM16 get 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable TIM16 CC1 Interrupt Request */
  HAL_TIM_IC_Stop_IT(&Input_Handle, TIM_CHANNEL_1);
  
  /* Deinitialize the TIM16 peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&Input_Handle);

  return uwLsiFreq;
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  uint32_t lsiperiod = 0;

  /* Get the Input Capture value */
  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&Input_Handle, TIM_CHANNEL_1);

  if (uwCaptureNumber >= 2)
  {
    /* Compute the period length */
    lsiperiod = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);

    /* Frequency computation */ 
    uwLsiFreq = (uint32_t) SystemCoreClock / lsiperiod;
    uwLsiFreq *= 8;
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



