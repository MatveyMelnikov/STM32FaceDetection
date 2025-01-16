#include "ov7670.h"
#include <stdbool.h>
#include <string.h>
#include "ov7670_defs.h"
#include "ili9341_tft_driver.h"
#include "main.h"

// Static variables ----------------------------------------------------------

static I2C_HandleTypeDef *i2c_io;
static uint16_t pixel_buffer[PIXEL_BUFFER_SIZE];
extern DCMI_HandleTypeDef hdcmi;
static volatile bool is_frame_captured = false;
static ili9341_draw_area area;

// Static functions prototypes -----------------------------------------------

static HAL_StatusTypeDef ov7670_set_fast_auto_exposure_and_gain_control(void);
static HAL_StatusTypeDef ov7670_set_edge_enhancement_factor(void);
static HAL_StatusTypeDef ov7670_set_uv_auto_adjustment(void);
static HAL_StatusTypeDef ov7670_enable_color_image(void);
__attribute__((always_inline))
static inline 
HAL_StatusTypeDef ov7670_add_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	const uint8_t data
);
__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_read_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	uint8_t *const data
);
__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_write_addr(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr
);
__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_write_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	const uint8_t data
);

// Functions implementations -------------------------------------------------

void ov7670_create(
	I2C_HandleTypeDef *const i2c,
	const ov7670_window_size window
)
{
	i2c_io = i2c;
	area = (ili9341_draw_area) {
		.start = 0,
		.end = window.width - 1,
		.top = 0,
		.bottom = window.height - 1
	};
}

void ov7670_destroy(void)
{
	i2c_io = NULL;
}

HAL_StatusTypeDef ov7670_check_link()
{
	uint8_t received_data = 0x0;

	HAL_StatusTypeDef status = ov7670_read_byte(i2c_io, PID, &received_data);
	status |= (received_data == 0x76) ? HAL_OK : HAL_ERROR;
	
	return status;
}

HAL_StatusTypeDef ov7670_reset()
{
	// 7 bit - reset
	HAL_StatusTypeDef status = ov7670_write_byte(i2c_io, COM_7, 0x80);
	HAL_Delay(1); // Datasheet pg. 6
	
	return status;
}

HAL_StatusTypeDef ov7670_start_capture()
{
	is_frame_captured = false;
	
	// The size is specified in uint32_t
	HAL_DCMI_Start_DMA(
		&hdcmi,
		DCMI_MODE_CONTINUOUS,
		(uint32_t)pixel_buffer,
		PIXEL_BUFFER_SIZE / 2U
	);
	
	return HAL_OK;
}

void ov7670_frame_received(DCMI_HandleTypeDef *hdcmi)
{
	is_frame_captured = true;
}

void ov7670_send_captured_frame(DCMI_HandleTypeDef *hdcmi)
{
	if (!is_frame_captured)
		return;

	ili9341_tft_driver_draw_image(&area, (uint16_t*)pixel_buffer);
	
	is_frame_captured = false;
}

// Enable 8-bar color bar
// TEST PATTERN - datasheet pg. 21
HAL_StatusTypeDef ov7670_set_test_pattern()
{
	HAL_StatusTypeDef status = ov7670_add_byte(i2c_io, SCALING_YSC, 0x80);
	
	return status;
}

// VGA processed bayer rgb mode (565)
// Implementation Guide pg. 8
HAL_StatusTypeDef ov7670_set_rgb_format()
{
	HAL_StatusTypeDef status = ov7670_add_byte(i2c_io, COM_7, 0x4);
	status |= ov7670_add_byte(i2c_io, COM_15, 0x10);
	
	return status;
}

HAL_StatusTypeDef ov7670_send_default_registers()
{
	HAL_StatusTypeDef status = ov7670_set_fast_auto_exposure_and_gain_control();
	status |= ov7670_set_edge_enhancement_factor();
	status |= ov7670_set_uv_auto_adjustment();
	status |= ov7670_enable_color_image();
	
	return status;
}

static HAL_StatusTypeDef ov7670_set_fast_auto_exposure_and_gain_control()
{
	return ov7670_write_byte(i2c_io, COM_8, 0x81);
}

static HAL_StatusTypeDef ov7670_set_edge_enhancement_factor()
{
	return ov7670_write_byte(i2c_io, EDGE, 0x01);
}

static HAL_StatusTypeDef ov7670_set_uv_auto_adjustment()
{
	return ov7670_write_byte(i2c_io, COM_13, 0xd9); 
}

static HAL_StatusTypeDef ov7670_enable_color_image()
{
	return ov7670_write_byte(i2c_io, 0xb0, 0x84); // Undocumented register
}

uint32_t *ov7670_get_captured_data()
{
	return (uint32_t*)pixel_buffer;
}

HAL_StatusTypeDef ov7670_set_plck_prescalar(uint8_t pre_scalar)
{
	return ov7670_write_byte(i2c_io, CLKRC, pre_scalar & 0x1f);
}

HAL_StatusTypeDef ov7670_set_qcif()
{
	HAL_StatusTypeDef status = ov7670_write_byte(i2c_io, COM_3, 0x8);
	status |= ov7670_write_byte(i2c_io, COM_7, 0x8);
	
	return status;
}

__attribute__((always_inline))
static inline 
HAL_StatusTypeDef ov7670_add_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	const uint8_t data
)
{
	uint8_t received_data = 0;
	HAL_StatusTypeDef status = ov7670_read_byte(i2c, sub_addr, &received_data);
	if (status)
		return status;
	
	return ov7670_write_byte(i2c, sub_addr, received_data | data);
}

__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_read_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	uint8_t *const data
)
{
	HAL_StatusTypeDef status = ov7670_write_addr(i2c, sub_addr);
	status |= HAL_I2C_Master_Receive(
		i2c,
		READ_ADDR,
		data,
		sizeof(data),
		OV7670_TIMEOUT
	);
		
	return status;
}

__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_write_addr(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr
)
{
	return HAL_I2C_Master_Transmit(
		i2c,
		WRITE_ADDR,
		(uint8_t*)&sub_addr,
		sizeof(sub_addr),
		OV7670_TIMEOUT
	);
}

__attribute__((always_inline))
static inline HAL_StatusTypeDef ov7670_write_byte(
	I2C_HandleTypeDef *i2c,
	const uint8_t sub_addr,
	const uint8_t data
)
{
	uint8_t sent_data[] = { sub_addr, data };
	
	return HAL_I2C_Master_Transmit(
		i2c,
		WRITE_ADDR,
		sent_data,
		sizeof(uint8_t) * 2,
		OV7670_TIMEOUT
	);
}
