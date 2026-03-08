#include "main.h"

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);


// System state
typedef enum {
    STATE_TX,
    STATE_WAIT_RESPONSE
} State_t;

volatile uint8_t nrf_event = 0; // 1: RX_DR, 2: TX_DS, 3: MAX_RT
uint8_t tx_buf[32] = "Ping";
uint8_t rx_buf[32];

NRF24_t nrf = {
    .hspi = &hspi1,
    .CE_Port = NRF_CE_GPIO_Port,
	.CE_Pin = NRF_CE_Pin,
    .CSN_Port = NRF_CS_GPIO_Port,
	.CSN_Pin = NRF_CS_Pin
};

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  RetargetInit(&huart1);
  NRF24_Init(&nrf);

  if (NRF24_Check(&nrf))
  {
	  uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	  NRF24_SetAddress(&nrf, addr);
      NRF24_Mode_RX(&nrf);
      printf("NRF OK\r\n");
  }

  else  printf("NRF CHECK ERROR\r\n");


  State_t current_state = STATE_TX;

  while (1)
  {
      switch (current_state)
      {
              case STATE_TX:
              NRF24_Mode_TX(&nrf);
              nrf_event = 0;
              NRF24_Transmit(&nrf, tx_buf);
              while(nrf_event == 0);

              if (nrf_event == 2) // sent successfully (TX_DS)
              {
                  printf("Sent OK\r\n");
                  current_state = STATE_WAIT_RESPONSE;
              }
              else // Error or timeout
              {
                  printf("TX Error\r\n");
                  current_state = STATE_TX;
                  HAL_Delay(100);
              }
              nrf_event = 0;
              break;

              case STATE_WAIT_RESPONSE:
              NRF24_Mode_RX(&nrf); // Switching on reception
              nrf_event = 0;
              uint32_t rx_timer = HAL_GetTick();
              while(nrf_event == 0 && (HAL_GetTick() - rx_timer < 200));

              if (nrf_event == 1) // Data received!
              {
                  NRF24_ReadPayload(&nrf, rx_buf, 32);
                  printf("Received: %s\r\n", (char*)rx_buf);

              }
              else  printf("RX Timeout\r\n");
              current_state = STATE_TX;
              break;
      }
  }

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == NRF_IRQ_Pin)
    {
    	uint8_t status = NRF24_ReadReg(&nrf, NRF24_REG_STATUS);
    	NRF24_WriteReg(&nrf, NRF24_REG_STATUS, status & 0x70); // Resetting all flags

    	if (status & (1 << 6)) nrf_event = 1; // RX
    	if (status & (1 << 5)) nrf_event = 2; // TX
    	if (status & (1 << 4)) // MAX_RT
    	{
    		nrf_event = 3;
    		uint8_t cmd = NRF24_CMD_FLUSH_TX;
    		CSN_Select(&nrf);
    		HAL_SPI_Transmit(nrf.hspi, &cmd, 1, 10);
    		CSN_Unselect(&nrf);
    	}

    }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}


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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NRF_CE_Pin|NRF_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NRF_CE_Pin NRF_CS_Pin */
  GPIO_InitStruct.Pin = NRF_CE_Pin|NRF_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : NRF_IRQ_Pin */
  GPIO_InitStruct.Pin = NRF_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(NRF_IRQ_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


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

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */
