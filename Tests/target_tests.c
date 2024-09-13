#include "main.h"
#include "string.h"
#include <stddef.h>
#include "unity_config.h"
#include "unity_fixture.h"

// Defines -------------------------------------------------------------------

enum {
  DEBUG_TIMEOUT = 100U
};

// Static variables ----------------------------------------------------------

UART_HandleTypeDef huart2;

// Static functions ----------------------------------------------------------

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void run_all_tests(void);

// Specific functions for tests ----------------------------------------------

void unity_config_put_c(uint8_t c);
int _kill(int, int);
int _getpid();
void _exit(int);

// Main code -----------------------------------------------------------------

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  return UnityMain(0, NULL, run_all_tests);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

static void run_all_tests()
{
	RUN_TEST_GROUP(test_module);
}

void unity_config_put_c(uint8_t c)
{
  (void) HAL_UART_Transmit(&huart2, &c, sizeof(c), DEBUG_TIMEOUT);
}

int _kill(int a, int b)
{
  UNUSED(a);
  UNUSED(b);

  return -1;
}
 
int _getpid()
{
  return 1;
}
 
void _exit(int a)
{
  UNUSED(a);

  while(1)
	{			
	}
}

void Error_Handler()
{
  __disable_irq();
  while (1)
  {
  }
}

