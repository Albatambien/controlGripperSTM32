/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* Definitions for gripperTask */
osThreadId_t gripperTaskHandle;
const osThreadAttr_t gripperTask_attributes = {
  .name = "gripperTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for artTask */
osThreadId_t artTaskHandle;
const osThreadAttr_t artTask_attributes = {
  .name = "artTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for stopMotors */
osThreadId_t stopMotorsHandle;
const osThreadAttr_t stopMotors_attributes = {
  .name = "stopMotors",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for receiveJetson */
osThreadId_t receiveJetsonHandle;
const osThreadAttr_t receiveJetson_attributes = {
  .name = "receiveJetson",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for emStop */
osThreadId_t emStopHandle;
const osThreadAttr_t emStop_attributes = {
  .name = "emStop",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for checkButton */
osThreadId_t checkButtonHandle;
const osThreadAttr_t checkButton_attributes = {
  .name = "checkButton",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for transmitJetson */
osThreadId_t transmitJetsonHandle;
const osThreadAttr_t transmitJetson_attributes = {
  .name = "transmitJetson",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */
int gripper = 0, art = 0;
int gripperOpen = 75, gripperClose = 127, artUp = 197, artDown = 83;
int comm[2];
int gripperFlag = 0, artFlag = 0, stopFlag = 0;
int gripperOK = 0, artOK = 0, stopOK = 0, objectHeld = 0;
int okFlags[4];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void StartGripperTask(void *argument);
void StartArtTask(void *argument);
void StopMotorMov(void *argument);
void StartReceiveJetson(void *argument);
void StartEmStop(void *argument);
void StartCheckButton(void *argument);
void StartTransmitJEtson(void *argument);

/* USER CODE BEGIN PFP */
void read(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of gripperTask */
  gripperTaskHandle = osThreadNew(StartGripperTask, NULL, &gripperTask_attributes);

  /* creation of artTask */
  artTaskHandle = osThreadNew(StartArtTask, NULL, &artTask_attributes);

  /* creation of stopMotors */
  stopMotorsHandle = osThreadNew(StopMotorMov, NULL, &stopMotors_attributes);

  /* creation of receiveJetson */
  receiveJetsonHandle = osThreadNew(StartReceiveJetson, NULL, &receiveJetson_attributes);

  /* creation of emStop */
  emStopHandle = osThreadNew(StartEmStop, NULL, &emStop_attributes);

  /* creation of checkButton */
  checkButtonHandle = osThreadNew(StartCheckButton, NULL, &checkButton_attributes);

  /* creation of transmitJetson */
  transmitJetsonHandle = osThreadNew(StartTransmitJEtson, NULL, &transmitJetson_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 64;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2499;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : Button_Pin */
  GPIO_InitStruct.Pin = Button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Button_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void read() {
    comm[0] = 1;
    comm[1] = 2;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartGripperTask */
/**
  * @brief  Function implementing the gripperTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartGripperTask */
void StartGripperTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if(gripperFlag == 1)
	  {
		  TIM2->CCR1 = gripperOpen;
		  osDelay(50);
		  gripperFlag = 0;
  		  gripperOK = 1;
	  }
	  else if (gripperFlag == 2)
	  {
		  TIM2->CCR1 = gripperClose;
		  osDelay(2000);
		  gripperFlag = 0;
  		  gripperOK = 1;
	  }
    osDelay(100);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartArtTask */
/**
* @brief Function implementing the artTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartArtTask */
void StartArtTask(void *argument)
{
  /* USER CODE BEGIN StartArtTask */
  /* Infinite loop */
  for(;;)
  {
	  if(artFlag == 1)
	  	  {
	  		  TIM2->CCR2 = artUp;
	  		  osDelay(50);
	  		  artFlag = 0;
	  		  artOK = 1;
	  	  }
	 else if (artFlag == 2)
	  	  {
	  		  TIM2->CCR2 = artDown;
	  		  osDelay(50);
	  		  artFlag = 0;
	  		  artOK = 1;
	  	  }
	 osDelay(1);
  }
  /* USER CODE END StartArtTask */
}

/* USER CODE BEGIN Header_StopMotorMov */
/**
* @brief Function implementing the stopMotors thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StopMotorMov */
void StopMotorMov(void *argument)
{
  /* USER CODE BEGIN StopMotorMov */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StopMotorMov */
}

/* USER CODE BEGIN Header_StartReceiveJetson */
/**
* @brief Function implementing the receiveJetson thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReceiveJetson */
void StartReceiveJetson(void *argument)
{
  /* USER CODE BEGIN StartReceiveJetson */
  /* Infinite loop */
  for(;;)
  {
	 read();
	 switch (comm[0]){
	 	 case 1:
	 		 if (comm[1]==1)
				{
					gripperFlag = 1;
				}
			 else if (comm[1]==2)
				{
					gripperFlag = 2;
				}
			 else
				{
					gripperFlag = 0;
				}
			 break;
		 case 2:
			 if (comm[1]==1)
				{
					artFlag = 1;
				}
			 else if (comm[1]==2)
				{
					artFlag = 2;
				}
			 else
				{
					artFlag = 0;
				}
			 break;
		 case 3:
				stopFlag = 1;
				break;
		default:
				gripperFlag = 0;
				artFlag = 0;
				stopFlag = 0;
	            break;
		  }
	    osDelay(20000);
  }
  /* USER CODE END StartReceiveJetson */
}

/* USER CODE BEGIN Header_StartEmStop */
/**
* @brief Function implementing the emStop thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEmStop */
void StartEmStop(void *argument)
{
  /* USER CODE BEGIN StartEmStop */
  /* Infinite loop */
  for(;;)
  {
	  if(stopFlag == 1){
		  gripperFlag = 0;
		  artFlag = 0;
		  stopFlag = 0;
	  }
    osDelay(1);
  }
  /* USER CODE END StartEmStop */
}

/* USER CODE BEGIN Header_StartCheckButton */
/**
* @brief Function implementing the checkButton thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCheckButton */
void StartCheckButton(void *argument)
{
  /* USER CODE BEGIN StartCheckButton */
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET){
	 		  objectHeld = 0;
	 	  }
	  else{
	 		  objectHeld = 1;
	 	  }
	  osDelay(1);
  }
  /* USER CODE END StartCheckButton */
}

/* USER CODE BEGIN Header_StartTransmitJEtson */
/**
* @brief Function implementing the transmitJetson thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTransmitJEtson */
void StartTransmitJEtson(void *argument)
{
  /* USER CODE BEGIN StartTransmitJEtson */
  /* Infinite loop */
  for(;;)
  {
	  if (gripperOK == 1)
	  {
		  okFlags[0]= gripperOK;
		  gripperOK = 0;
	  }
	  else
	  {
		  okFlags[0]= 0;
	  }
	  if (artOK == 1)
	  {
	  	  okFlags[1]= artOK;
	  	  artOK = 0;
	  }
	  else
	  {
		  okFlags[1]= 0;
	  }
	  if (objectHeld == 1)
	  {
	  	  okFlags[2]= objectHeld;
	  }
	  else
	  {
		  okFlags[2]= 0;
	  }
	  if (stopOK == 1)
	  {
	  	  okFlags[3]= stopOK;
	  	  stopOK = 0;
	  }
	  else
	  {
	  	  okFlags[3]= 0;
	  }
    osDelay(20100);
  }
  /* USER CODE END StartTransmitJEtson */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
