/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341_tft_driver.h"
#include "ov7670.h"
#include "ov7670_defs.h"
#include "face_detector_builder.h"
#include "integral_image.h"
#include "binary_stage_parser_defs.h"
#include "face_detector.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// RGB565
#define GET_RGB_PIXEL(image, x, y, w) \
  ({ \
    ((image) + ((x) + (y) * (w))); \
  })
#define APPLY_ROUNDING(n) \
  (uint8_t)(n) >> 5
#define FAST_DIVIDE_BY_3(n) \
	((n) * 85 >> 8)
#define SET_PIXEL_COLOR(pixel, r, g, b) \
  *(pixel) = ((r) >> 3) << 11; \
  *(pixel) |= (g >> 2) << 6; \
  *(pixel) |= (b >> 3)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define HOLD_VARIABLE(var) \
	if ((var) == 0) __asm("nop")
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

I2C_HandleTypeDef hi2c2;

SRAM_HandleTypeDef hsram1;

/* USER CODE BEGIN PV */
static ili9341_tft_driver_io_struct tft_io;
extern uint32_t _start_classifiers_load;
extern uint32_t _binary_lbpcascade_frontalface_32_improved_integer_bin_start;
extern uint32_t _binary_lbpcascade_frontalface_32_improved_integer_bin_end;
static uint16_t convertion_buffer[OV7670_IMAGE_WIDTH];
static face_detector_arguments arguments = (face_detector_arguments) {
  .base_scale = 1.f,
  .position_increment = 0.1f,
  .scale_increment = 1.1f,
  .image_size_x = OV7670_IMAGE_WIDTH,
  .image_size_y = OV7670_IMAGE_HEIGHT,
  .min_neighbours = 0U
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_FSMC_Init(void);
static void MX_I2C2_Init(void);
static void MX_DCMI_Init(void);
/* USER CODE BEGIN PFP */
__attribute__((always_inline))
static inline void __initialize_ccm(
  uint32_t *load_addr,
  uint32_t *ccm_begin,
  uint32_t *ccm_end
);

__attribute__((always_inline))
static inline uint16_t decolorize(const uint16_t *pixel);
static void fill_integral_image(
  FILL_LINE_FUNCTOR,
  const uint8_t *const image_source,
  integral_image_size image_size
);
static uint16_t get_rectangle_summarize(
  const lbp_feature_rectangle *const feature_rectangle
);

static void tft_init(void);
static void tft_write_data(uint16_t value);
static void tft_write_reg(uint16_t reg_value);
static uint16_t tft_read_data(uint8_t data_size);
static void tft_enable_backlight(bool is_enabled);
static void draw_faces(uint16_t *const image);
static void draw_face(uint16_t *const image, area area_face);
__attribute__((always_inline))
static inline void draw_big_pixel(
  uint16_t *const image,
  uint8_t x,
  uint8_t y
);

static void configure_display(void);
static void configure_camera(void);
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
  __initialize_ccm(
    &_start_classifiers_load,
    &_binary_lbpcascade_frontalface_32_improved_integer_bin_start,
    &_binary_lbpcascade_frontalface_32_improved_integer_bin_end
  );
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
  MX_DMA_Init();
  MX_FSMC_Init();
  MX_I2C2_Init();
  MX_DCMI_Init();
  /* USER CODE BEGIN 2 */

  // qcif
  integral_image_create(
    (integral_image_size) {
      .width = OV7670_IMAGE_WIDTH,
      .height = OV7670_IMAGE_HEIGHT
    },
    fill_integral_image
  );
  face_detector_builder_create(
    (uint8_t*)&_binary_lbpcascade_frontalface_32_improved_integer_bin_start,
    IMPROVED_BINARY_DATA_STAGES_AMOUNT,
    get_rectangle_summarize
  );

	configure_display();
	configure_camera();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	ov7670_start_capture();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		ov7670_send_captured_frame(&hdcmi);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
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
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
}

/**
  * @brief DCMI Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BC_GPIO_Port, LCD_BC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LCD_BC_Pin */
  GPIO_InitStruct.Pin = LCD_BC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 6;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 6;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */

__attribute__((always_inline))
static inline void __initialize_ccm(
  uint32_t *load_addr,
  uint32_t *ccm_begin,
  uint32_t *ccm_end
)
{
  uint32_t *ccm_ptr = ccm_begin;
  while (ccm_ptr < ccm_end)
    *ccm_ptr++ = *load_addr++;
}

static void fill_integral_image(
  FILL_LINE_FUNCTOR,
  const uint8_t *const image_source,
  integral_image_size image_size
)
{
  uint16_t *image = (uint16_t*)image_source;

  for (uint8_t y = 0; y < image_size.height; y++)
  {
    for (uint8_t x = 0; x < image_size.width; x++)
    {
      convertion_buffer[x] = APPLY_ROUNDING(
        decolorize(GET_RGB_PIXEL(image, x, y, image_size.width))
      );
    }

    fill_line(convertion_buffer, y);
  }
}

__attribute__((always_inline))
static inline uint16_t decolorize(const uint16_t *pixel)
{
  uint8_t r = (*pixel >> 11) << 3; // / 31 * 255 ~= >> 5 << 8
	uint8_t g = ((*pixel >> 6) & 0x1f) << 3; // to 5 digits
	uint8_t b = (*pixel & 0x1f) << 3;
	
	uint8_t grayscale = FAST_DIVIDE_BY_3(r + g + b);

  return grayscale;
}

static uint16_t get_rectangle_summarize(
  const lbp_feature_rectangle *const feature_rectangle
)
{
  integral_image_rectangle_position integral_position = {
    .top_left_corner = {
      .x = feature_rectangle->x,
      .y = feature_rectangle->y
    },
    .bottom_right_corner = {
      .x = feature_rectangle->x + feature_rectangle->width,
      .y = feature_rectangle->y + feature_rectangle->height
    }
  };

  return integral_image_get_rectangle(&integral_position);
}

static void tft_init(void)
{
	HAL_Delay(20);
}

static void tft_write_data(uint16_t value)
{
	*FMC_BANK1_DATA = value;
}

static void tft_write_reg(uint16_t reg_value)
{
	*FMC_BANK1_CMD = reg_value;
}

static uint16_t tft_read_data(uint8_t data_size)
{
	if (data_size > 2)
		return 0;
	
	uint16_t mask = 0xffff >> ((2 - data_size) * 8);
	
	return *(FMC_BANK1_DATA) & mask;
}

static void tft_enable_backlight(bool is_enabled)
{
	HAL_GPIO_WritePin(
		LCD_BC_GPIO_Port,
		LCD_BC_Pin,
		is_enabled ? GPIO_PIN_SET : GPIO_PIN_RESET
	);
}

static void draw_faces(uint16_t *const image)
{
  integral_image_set((uint8_t*)image);
  integral_image_calculate();

  face_detector_detect(&arguments);
  face_detector_result result = face_detector_get_result();

  for (uint8_t i = 0; i < result.faces_amount; i++)
  {
    draw_face(image, result.faces[i]);
  }
}

static void draw_face(uint16_t *const image, area area_face)
{
  for (uint8_t x = 0; x < area_face.size; x += 2)
  {
    draw_big_pixel(image, area_face.x + x, area_face.y);
    draw_big_pixel(image, area_face.x + x, area_face.y + area_face.size);
  }

  for (uint8_t y = 2; y < area_face.size; y += 2)
  {
    draw_big_pixel(image, area_face.x, area_face.y + y);
    draw_big_pixel(image, area_face.x + area_face.size - 1, area_face.y + y);
  }
}

__attribute__((always_inline))
static inline void draw_big_pixel(
  uint16_t *const image,
  uint8_t x,
  uint8_t y
)
{
  if (x + 1 >= OV7670_IMAGE_WIDTH)
    return;
  if (y + 1 >= OV7670_IMAGE_HEIGHT)
    return;

  uint16_t *up_left_pixel = GET_RGB_PIXEL(
    image,
    x,
    y,
    OV7670_IMAGE_WIDTH
  );
  uint16_t *up_right_pixel = GET_RGB_PIXEL(
    image,
    x + 1,
    y,
    OV7670_IMAGE_WIDTH
  );
  uint16_t *down_left_pixel = GET_RGB_PIXEL(
    image,
    x,
    y + 1,
    OV7670_IMAGE_WIDTH
  );
  uint16_t *down_right_pixel = GET_RGB_PIXEL(
    image,
    x + 1,
    y + 1,
    OV7670_IMAGE_WIDTH
  );

  SET_PIXEL_COLOR(up_left_pixel, 255, 0, 0);
  SET_PIXEL_COLOR(up_right_pixel, 255, 0, 0);
  SET_PIXEL_COLOR(down_left_pixel, 255, 0, 0);
  SET_PIXEL_COLOR(down_right_pixel, 255, 0, 0);
}

static void configure_display()
{
	tft_io = (ili9341_tft_driver_io_struct) {
		.io_init = tft_init,
		.io_write_data = tft_write_data,
		.io_write_reg = tft_write_reg,
		.io_read_data = tft_read_data,
		.io_enable_backlight = tft_enable_backlight
	};

	ili9341_tft_driver_status status = ili9341_tft_driver_init(
    &tft_io,
    draw_faces
  );
	if (status)
		Error_Handler();
}

static void configure_camera()
{
	HAL_Delay(10);
	
	ov7670_create(
		&hi2c2,
		(ov7670_window_size) { .width = 172U, .height = 144U }
	);

	HAL_StatusTypeDef status = ov7670_check_link();
	if (status)
		Error_Handler();
	
	status |= ov7670_reset();
	status |= ov7670_send_default_registers();
	status |= ov7670_set_qcif();
	status |= ov7670_set_plck_prescalar(1);
	status |= ov7670_set_rgb_format();
	//status |= ov7670_set_test_pattern();
	
	HAL_Delay(10);
	
	if (status)
		Error_Handler();
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	ov7670_frame_received(hdcmi);
}

void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	(void)hdcmi;
}

void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	(void)hdcmi;
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
	(void)hdcmi;
}

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
