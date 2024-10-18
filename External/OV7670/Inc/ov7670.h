#ifndef OV7670_H
#define OV7670_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct
{
	uint16_t width;
	uint16_t height;
} ov7670_window_size;

typedef struct
{
	uint8_t cb;
	uint8_t y0;
	uint8_t cr;
	uint8_t y1;
} ov7670_two_pixels;

void ov7670_create(
	I2C_HandleTypeDef *const i2c,
	const ov7670_window_size window
);
void ov7670_destroy(void);
HAL_StatusTypeDef ov7670_check_link(void);
HAL_StatusTypeDef ov7670_reset(void);
HAL_StatusTypeDef ov7670_start_capture(void);
void ov7670_send_captured_frame(DCMI_HandleTypeDef *hdcmi);
void ov7670_frame_received(DCMI_HandleTypeDef *hdcmi);
HAL_StatusTypeDef ov7670_set_test_pattern(void);
HAL_StatusTypeDef ov7670_set_rgb_format(void);
HAL_StatusTypeDef ov7670_send_default_registers(void);
uint32_t *ov7670_get_captured_data(void);
HAL_StatusTypeDef ov7670_set_plck_prescalar(uint8_t pre_scalar);
HAL_StatusTypeDef ov7670_set_qcif(void);

#endif
