#include "main.h"
#include "stm32l4xx_hal.h"

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

uint8_t sensorTemp;
//int sensorTemp;
int tens;
int ones;
int realTemp;
int t;
uint32_t Reload;
int flag;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_adc_init(void);

// Declare UART_Print_String() function
int UART_Print_String(UART_HandleTypeDef * uart_pointer, char * array_ptr, int no_of_items);
int UART_Print_String_DMA(UART_HandleTypeDef * uart_pointer, char * array, int no_of_items);
void uart_dma_init();

int main(void){
	char holder;
	char holder2;
	char array[30];
	char ch[5] = {'j','o','b','s','\n'};
	char temp[18]= {'T','e','m','p','e','r','a','t','u','r','e',':',' ','0','0','C','\r','\n'};
	int counter = 0;


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
	// initialize ADC
	MX_adc_init();
	// initialize DMA
	//HAL_DMA_Init(&hdma_usart1_tx);
	//_HAL_RCC_DMA1_CLK_ENABLE();
	uart_dma_init();
	// Initialize ADC

  /* Infinite loop */
  while (1){
		
		// dma
		//HAL_DMA_Start(&hdma_usart1_tx, sensorTemp, (uint32_t)&array, 30);
		//HAL_Delay(100);
		
		// using interrupt
		if(flag == 1) {
			HAL_ADC_Start(&hadc1);
			HAL_DMA_Start(&hdma_usart1_tx, sensorTemp, (uint32_t)&array, 30);
			flag = 0;
			
		// end of interrupt code
			
			// look for temperature vals
			if(HAL_ADC_PollForConversion(&hadc1, 3000) == HAL_OK){
				//HAL_ADC_ConvCpltCallback(&hadc1);
				sensorTemp = (uint8_t)HAL_ADC_GetValue(&hadc1);
				//HAL_ADC_Stop(&hadc1);
				
				//HAL_UART_Transmit(&huart1, (uint8_t *)&ch[0], 5, 30000);
				//sensorTemp = sensorTemp *(100/256);
				sensorTemp = __HAL_ADC_CALC_TEMPERATURE(3300,sensorTemp, ADC_RESOLUTION_10B);
				//sensorTemp -= 20; //for calibration
				//realTemp =(sensorTemp - 760)/(2.5 + 25);
				
				//tens = (realTemp/10);
				tens = (sensorTemp/10);
				ones = sensorTemp-(tens*10);
				//ones = (realTemp % 10);
				
				//holder = tens + '0';
				//holder2 = ones + '0';
				
				temp[13] = tens + '0';
				temp[14] = ones + '0';
				
				if (counter < 30){
					array[counter] = tens + '0';
					counter++;
					array[counter] = ones + '0';
					counter++;
					array[counter] = '\n';
					counter++;
				}
				if (counter == 30) {
					UART_Print_String_DMA(&huart1, &array[0], 30);

					// HAL_UART_Transmit_DMA(&huart1, (uint8_t *) array, 30);
					counter = 0;
				}
				//UART_Print_String(&huart1, &temp[0], 18);
				
				//HAL_UART_Transmit(&huart1, (uint8_t *)&temp[0], 18, 3000);
			
			}
		}
  }
}

/**
Function used to send characters over the UART channel. Uses the HAL_UART_Transmit() function.
**/
int UART_Print_String(UART_HandleTypeDef * uart_pointer, char * array_ptr, int no_of_items){
	
	// Call the function to print to UART
	HAL_UART_Transmit(uart_pointer, (uint8_t *)array_ptr, no_of_items, 30000);
	
	if(uart_pointer->gState != 0xE0U) {
		return 1;
	}
	return 0;
}

int UART_Print_String_DMA(UART_HandleTypeDef * uart_pointer, char * array, int no_of_items){
	
	// Call the function to print to UART
	HAL_UART_Transmit_DMA(uart_pointer, (uint8_t *) array, 30);
	__HAL_UART_ENABLE_IT(uart_pointer, UART_IT_TXE) ;
	
	if(uart_pointer->gState != 0xE0U) {
		return 1;
	}
	return 0;
}

static void MX_adc_init(){
	ADC_ChannelConfTypeDef sConfig;

	hadc1.Instance = ADC1;
	//hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	// set resolution to 10 bits 
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
	// LSB is the right most bit
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;

	if(HAL_ADC_Init(&hadc1) != HAL_OK){
    _Error_Handler(__FILE__, __LINE__);
  }
	
 sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
 sConfig.Rank = 1;
 sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
 sConfig.Offset = 0;
	
  if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 32;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
	
	// argument you pass in, is an integer the systick counter counts to
	// HALL_RCC_GetHCLKFreq() = 80MHz (max frequency)
	// clocktime 
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/10);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void uart_dma_init()
{
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();

  /* Peripheral DMA init*/
  //hdma_usart1_tx.Instance = DMA1_Stream6;
  //hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
  hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart1_tx.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart1_tx.Init.Mode = DMA_NORMAL;
  hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
  //hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_usart1_tx);

  __HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_ADC_CLK_ENABLE();
}


void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
}
#endif /* USE_FULL_ASSERT */
