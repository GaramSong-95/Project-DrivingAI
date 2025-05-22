/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
SPI_HandleTypeDef hspi1;
#define MCP2515_CS_PORT GPIOA
#define MCP2515_CS_PIN  GPIO_PIN_4
#define MCP2515_CS_LOW() HAL_GPIO_WritePin(MCP2515_CS_PORT, MCP2515_CS_PIN, GPIO_PIN_RESET)
#define MCP2515_CS_HIGH() HAL_GPIO_WritePin(MCP2515_CS_PORT, MCP2515_CS_PIN, GPIO_PIN_SET)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  mcp2515_init();


  char start[] = "start\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)start, strlen(start), 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*부저파트
	// 부저 ON
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500); // 듀티비 50%
	HAL_Delay(1000);
	// 부저 OFF
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // 듀티비 0%
	HAL_Delay(1000);
	*/
	mcp2515_receive_message(); // 수신 테스트
	//HAL_Delay(2000);
	//mcp2515_send_message(); // 송신테스트
	//HAL_Delay(2000);
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 159;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  // GPIOB 클럭 활성화
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



  // PB5를 출력으로 설정 (MODER 레지스터: 2비트당 1핀)
  GPIOB->MODER &= ~(0b11 << (5 * 2));  // 초기화
  GPIOB->MODER |=  (0b01 << (5 * 2));  // 01: General purpose output

  // 출력 타입 설정 (푸시풀)
  GPIOB->OTYPER &= ~(1 << 5);  // 0: Push-pull

  // 출력 속도 설정 (선택사항: High speed로 설정 가능)
  GPIOB->OSPEEDR |= (0b11 << (5 * 2));  // 11: High speed

  // 풀업/다운 설정 안 함 (No pull-up, pull-down)
  GPIOB->PUPDR &= ~(0b11 << (5 * 2));

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//SPI 송수신 함수
uint8_t SPI_WriteRead(uint8_t data)
{
	uint8_t received = 0;
		HAL_SPI_TransmitReceive(&hspi1, &data, &received, 1, HAL_MAX_DELAY);
		return received;
}

//MCP2515 레지스터 쓰기/읽기 함수(CAN 모듈)
void mcp2515_write_register(uint8_t address, uint8_t data)
{
	MCP2515_CS_LOW(); //SPI 슬레이브 시작
	SPI_WriteRead(0x02); //Write 명령
	SPI_WriteRead(address); // 쓰고 싶은 MCP2515 내부 주소
	SPI_WriteRead(data); //그 주소에 쓸 데이터
	MCP2515_CS_HIGH(); //SPI 종료
}

uint8_t mcp2515_read_register(uint8_t address)
{
	uint8_t value;
	MCP2515_CS_LOW();
	SPI_WriteRead(0x03); // Read command
	HAL_Delay(1); // ← 추가
	SPI_WriteRead(address);
	HAL_Delay(1); // ← 추가
	value = SPI_WriteRead(0x00);
	MCP2515_CS_HIGH();
	return value;
}

/************CP2515 초기화 함수****************/
void mcp2515_reset()
{
	MCP2515_CS_LOW();
	SPI_WriteRead(0xC0); //Reset command
	MCP2515_CS_HIGH();
	HAL_Delay(10); //리셋 후 안정을 위해 대기
}

void mcp2515_init()
{
	mcp2515_reset();

	//CNF1, CNF2, CNF3 설정 500kbps 기준 (8Mhz 기준)
	mcp2515_write_register(0x2A, 0x00);  // CNF1: SJW = 1, BRP = 0
	mcp2515_write_register(0x29, 0x91);  // CNF2: BTLMODE=1, PHSEG1=2, PRSEG=1
	mcp2515_write_register(0x28, 0x01);  // CNF3: PHSEG2=3

	// Normal Mode 진입
	mcp2515_write_register(0x0F, 0x00); // CANCTRL register -> Normal mode
	HAL_Delay(10);
	uint8_t mode = mcp2515_read_register(0x0E) & 0xE0; // CANSTAT 상위 3비트로 현재 모드 확인
	if (mode != 0x00)
	{
		char err[] = "MCP2515 NOT in Normal Mode\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), 100);
	}
	// 필터/마스크 초기화
	mcp2515_write_register(0x00, 0x00); // RXM0SIDH
	mcp2515_write_register(0x01, 0x00); // RXM0SIDL
	mcp2515_write_register(0x04, 0x00); // RXM1SIDH
	mcp2515_write_register(0x05, 0x00); // RXM1SIDL

	// 필터 무시 모드 설정
	mcp2515_write_register(0x60, 0x64);  // RXB0CTRL: RXM=11, BUKT=1
	mcp2515_write_register(0x70, 0x60);  // RXB1CTRL: RXM=11



}
/******************************************/

/***************CAN 송신 함수***************/
void mcp2515_send_message()
{
	// ID: 0x123
	uint16_t id = 0x123;
    uint8_t sidh = (id >> 3) & 0xFF;
    uint8_t sidl = (id & 0x07) << 5;
    sidl &= ~(1 << 3); // IDE 비트 반드시 0으로!

	mcp2515_write_register(0x30, sidh); // TXB0SIDH
	mcp2515_write_register(0x31, sidl); // TXB0SIDL

	// DLC(data length code): 4바이트
	mcp2515_write_register(0x35, 0x04); // TXBODLC

	// Data payload: 0xDE, 0xAD, 0xBE, 0xEF
	mcp2515_write_register(0x36, 0xDE);
	mcp2515_write_register(0x37, 0xAD);
	mcp2515_write_register(0x38, 0xBE);
	mcp2515_write_register(0x39, 0xEF);

	// 전송 요청 전 CANINTF의 TX0IF 플래그 클리어
	mcp2515_write_register(0x2C, 0x00);

	// 요청 전송
	MCP2515_CS_LOW();
	SPI_WriteRead(0x81); // RTS TXB0
	MCP2515_CS_HIGH();

	// 전송 성공 확인
	HAL_Delay(1); // 아주 짧은 시간 대기(TX처리 시간)
	uint8_t canintf = mcp2515_read_register(0x2C);
	if(canintf & (0x04 | 0x02 | 0x01)) // TX0IF bit
	{
		char msg[] = "CAN TX Done\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
	}
	else
	{
		char err[] = "TX Failed\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), 100);
	}

}
/******************************************/

/***************CAN 수신 함수***************/
void mcp2515_receive_message()
{

	uint8_t canintf = mcp2515_read_register(0x2C);
		uint8_t rxb_addr;
		if (canintf & 0x01) rxb_addr = 0x61;
		else if (canintf & 0x02) rxb_addr = 0x71;
		else return;

		uint8_t sidH = mcp2515_read_register(rxb_addr);       // SIDH
		uint8_t sidL = mcp2515_read_register(rxb_addr + 1);   // SIDL
		uint8_t eid8 = mcp2515_read_register(rxb_addr + 2);   // EID8
		uint8_t eid0 = mcp2515_read_register(rxb_addr + 3);   // EID0
		uint8_t dlc  = mcp2515_read_register(rxb_addr + 4) & 0x0F;

		uint8_t ide = (sidL >> 3) & 0x01;
		uint32_t id;
		if (ide == 0) {
		    // 표준 ID
		    id = ((sidH << 3) | (sidL >> 5)) & 0x7FF;
		} else {
		    // 확장 ID
		    id = ((uint32_t)(sidH) << 21)
		       | ((uint32_t)(sidL & 0xE0) << 13)
		       | ((uint32_t)(sidL & 0x03) << 16)
		       | ((uint32_t)(eid8) << 8)
		       | (eid0);
		}
		uint8_t data[8] = {0};
		for (int i = 0; i < dlc; i++) {
			data[i] = mcp2515_read_register(rxb_addr + 5 + i);
		}

		if(id == 0x123 && (data[0] == 0x01 || data[0] == 0x02))
		{
			HAL_UART_Transmit(&huart2, (uint8_t*)"BUZZER ON!\r\n", 12, 100);
			// 부저 PWM 시작
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 50); // 듀티비 5%
			GPIOB->ODR |= (1 << 5);
			HAL_Delay(100);
			// 부저 OFF
			GPIOB->ODR &= ~(1 << 5);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		}

		// 출력
		char buf[128];
		int offset = 0;
		offset += sprintf(buf + offset, "CAN RX ID=0x%08X, DLC=%d, Data=", id, dlc);
		for (int i = 0; i < dlc; i++) {
		    offset += sprintf(buf + offset, "%02X ", data[i]);
		}
		offset += sprintf(buf + offset, "\r\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, offset, 100);

		sprintf(buf, "SIDH = 0x%02X, SIDL = 0x%02X, EID8 = 0x%02X, EID0 = 0x%02X, DLC = %d\r\n",
		        sidH, sidL, eid8, eid0, dlc);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);

		sprintf(buf, "CAN RX ID = 0x%08X, Data = ", id);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);

		for (int i = 0; i < dlc; i++) {
			sprintf(buf, "%02X ", data[i]);
			HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
		}
		HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);

		// 클리어
		mcp2515_write_register(0x2C, canintf & ~(0x03));
}
/******************************************/
/* USER CODE END 4 */

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
