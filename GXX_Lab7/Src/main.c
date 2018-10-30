
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "stdio.h"

#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_gyro.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
osThreadId defaultTaskHandle;

int32_t counter = 0;
int lock = 0;
osSemaphoreId semaphore_id;
char buffer[100];

int sensorFlag = 0;
int timer = 0;
int btnFlag = 0;
int threadFlag = 0;
int longPressed = 0;
int sensor_thread_killed=0;

int tim_flag = 0;

/* USER CODE BEGIN PV */

osThreadId uartTaskHandle;
osThreadId sensorsTaskHandle;
osThreadId buttonTaskHandle;

TIM_HandleTypeDef htim2;

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef handle;
static void MX_GPIO_Init(void);
void UART_init(void);

static void MX_TIM2_Init(void);

int fputc(int ch, FILE *f) {
  while (HAL_OK != HAL_UART_Transmit(&handle, (uint8_t *) &ch, 1, 30000));
  return ch;
}
int fgetc(FILE *f) {
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&handle, (uint8_t *)&ch, 1, 30000));
  return ch;
}

void UART_init() {	
	GPIO_InitTypeDef gpio_init;
	
	handle.Instance = USART1;
	handle.Init.BaudRate = 115200;
  handle.Init.WordLength = UART_WORDLENGTH_8B;
  handle.Init.StopBits = UART_STOPBITS_1;
  handle.Init.Parity = UART_PARITY_NONE;
  handle.Init.Mode = UART_MODE_TX_RX;
  handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  handle.Init.OverSampling = UART_OVERSAMPLING_16;
  handle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();
	
  gpio_init.Pin = GPIO_PIN_6;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOB, &gpio_init);
	
	gpio_init.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &gpio_init);
	
	HAL_UART_Init(&handle);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);

void StartSensorTask(void const * argument);
void StartButtonTask(void const * argument);
void StartUartTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
//	MX_GPIO_Init();
//	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
	//int lock = 0;
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	
	osSemaphoreDef(SemaphoreIsr);
	semaphore_id = osSemaphoreCreate(osSemaphore(SemaphoreIsr), counter);

  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	
//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
//	
//	// Create UART Thread
//	osThreadDef(uartThread, StartUartTask, osPriorityNormal, 0, 128);
//	uartTaskHandle = osThreadCreate(osThread(uartThread),NULL);
//	
//	// Create Sensors thread
	osThreadDef(sensorThread, StartSensorTask, osPriorityNormal, 0, 128);
	sensorsTaskHandle = osThreadCreate(osThread(sensorThread), NULL);
//	
//	// Create Button Thread
	osThreadDef(buttonThread, StartButtonTask, osPriorityNormal, 0, 128);
	buttonTaskHandle = osThreadCreate(osThread(buttonThread),NULL);
//	
//  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
		osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */
		

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
 
    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
//void StartDefaultTask(void const * argument)
//{

//  /* USER CODE BEGIN 5 */
//	UART_init();
//	BSP_ACCELERO_Init();
//	int16_t XYZ[3];
//	int buttonState;
//  /* Infinite loop */
//  for(;;)
//  {
//		osDelay(100);
//		BSP_ACCELERO_AccGetXYZ(XYZ);
//		printf("X = %d, Y = %d, Z = %d\n\r", XYZ[0], XYZ[1], XYZ[2]);
//	
//		int buttonState = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
//		
//		if(buttonState == 0){
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
//		} else {
//			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
//		}
//  }
  /* USER CODE END 5 */ 
//}

void StartSensorTask(void const * argument){
	
	UART_init();
	BSP_ACCELERO_Init();
	BSP_HSENSOR_Init();
	BSP_GYRO_Init();
	BSP_TSENSOR_Init();
	BSP_MAGNETO_Init();
	BSP_PSENSOR_Init();
	
	
	
	// Values for reading
	int16_t XYZ_Accel[3];
	float XYZ_Gyro[3];
	float humidity;
	float temp;
	int16_t XYZ_Mag[3];
	float pressure;
	
	// Create UART Printing thread


		
		for(;;){
			
			if(lock == 0) {
				lock =1;
				if(counter == 0){
					osDelay(200);
					temp = BSP_TSENSOR_ReadTemp();
					printf("Temperature: %f\n\r", temp);
					
				}
				else if(counter == 1){
					osDelay(200);
					humidity = BSP_HSENSOR_ReadHumidity();
					printf("Humidity: %f\n\r", humidity);
				}
				else if(counter == 2) {
					osDelay(200);
					BSP_MAGNETO_GetXYZ(XYZ_Mag);
					printf("Magnetometer: X= %d, Y= %d, X= %d\n\r",XYZ_Mag[0],XYZ_Mag[1],XYZ_Mag[2]);
				}
				else if(counter == 3){
					osDelay(200);
					pressure = BSP_PSENSOR_ReadPressure();
					printf("Pressure: %f\n\r", pressure);	
				}else if(counter == 4){
					osDelay(200);
					BSP_ACCELERO_AccGetXYZ(XYZ_Accel);
					printf("Accelerometer: X = %d, Y = %d, Z = %d\n\r", XYZ_Accel[0], XYZ_Accel[1], XYZ_Accel[2]);
				}else if(counter == 5){
					osDelay(200);
					BSP_GYRO_GetXYZ(XYZ_Gyro);
					printf("Gyro: X = %f, Y = %f, Z = %f\n\r", XYZ_Gyro[0], XYZ_Gyro[1], XYZ_Gyro[2]);
				}
				lock = 0;
			}
}
	
}

void StartButtonTask(void const * argument){
	int initialized = 0;
	
	MX_GPIO_Init();
	//MX_TIM1_Init();
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
	
	for(;;){
		timer = 0;
		int buttonState = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);
		while(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)){
			if(tim_flag == 1){
				// Using systic timer at 80kHz
				tim_flag = 0;
				timer++;
			}
		}
		if(timer > 1500){
			// turn sensor thread off
			osThreadTerminate(sensorsTaskHandle);
			sensor_thread_killed=1;
		}else{
			if(buttonState == 0){
				if(sensor_thread_killed==1){
					sensor_thread_killed=0;
					osThreadDef(sensorThread, StartSensorTask, osPriorityNormal, 0, 128);
					sensorsTaskHandle = osThreadCreate(osThread(sensorThread), NULL);
				}
				lock=1;
				osDelay(150);
				counter++;
				lock=0;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
				if(counter == 6){
					counter = 0;
				}
			}else{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			}
		}		
	}
}

void StartUartTask(void const * argument){
	
	while(1) {
		int uart_lock = 1;
		HAL_UART_Transmit(&handle, (uint8_t *) &buffer[0], strlen(buffer), 30000);
		//HAL_UART_Transmit(&handle, (uint8_t *) "\n", 1, 30000);
		uart_lock = 0;
		//printf(buffer,strlen(buffer));
		osDelay(100);
	}
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */

static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/**
  * @}
  */

/**
  * @}
  */
	


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
