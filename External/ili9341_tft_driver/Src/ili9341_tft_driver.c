#include "ili9341_tft_driver.h"
#include "ili9341_tft_driver_defs.h"
#include <stddef.h>
#include <stdbool.h>

// Defines -------------------------------------------------------------------

#define FAST_DIVIDE_BY_3(n) \
	((n) * 85 >> 8)

// Static variables ----------------------------------------------------------

static ili9341_tft_driver_io_struct *io_struct = NULL;
static void (*draw_faces)(uint16_t *const image);

// Static functions prototypes -----------------------------------------------

static ili9341_tft_driver_status ili9341_check_link(void);
static void ili9341_soft_reset(void);
static void ili9341_send_default_registers(void);
static void ili9341_display_on(void);
static bool ili9341_is_not_valid_coordinates(ili9341_draw_area *const area);
static ili9341_tft_driver_status ili9341_write_x_coordinates(uint16_t x0, uint16_t x1);
static ili9341_tft_driver_status ili9341_write_y_coordinates(uint16_t y0, uint16_t y1);
// __attribute__((always_inline))
// static inline uint16_t decolorize(const uint16_t *pixel);
static uint32_t ili9341_calculate_pixels_amount(ili9341_draw_area *const area);
static ili9341_draw_area ili9341_get_max_area(void);
__attribute__((always_inline))
static inline bool is_not_safe_call(void *const function_ptr);
static ili9341_tft_driver_status io_init(void);
__attribute__((always_inline))
static inline ili9341_tft_driver_status io_write_data(uint16_t data);
__attribute__((always_inline))
static inline ili9341_tft_driver_status io_write_reg(uint16_t reg_value);
__attribute__((always_inline))
static inline ili9341_tft_driver_status io_read_data(
	uint8_t data_size,
	uint16_t *const value
);
static ili9341_tft_driver_status io_delay(uint32_t delay);
static ili9341_tft_driver_status io_dummy_read(uint8_t amount);
static ili9341_tft_driver_status io_enable_backlight(bool is_enabled);

// Implementations -----------------------------------------------------------

ili9341_tft_driver_status ili9341_tft_driver_init(
	ili9341_tft_driver_io_struct *const io,
	void (*draw_faces_functor)(uint16_t *const image)
)
{
	io_struct = io;
	
	io_enable_backlight(false);
	
	ili9341_tft_driver_status status = io_init();
	if (status)
		return status;
	
	status = ili9341_check_link();
	if (status)
		return status;

	ili9341_soft_reset();
	ili9341_send_default_registers();
	
	ili9341_display_on();
	ili9341_tft_driver_clear();
	io_enable_backlight(true);

	draw_faces = draw_faces_functor;
	
	return ILI9341_OK;
}

static ili9341_tft_driver_status ili9341_check_link()
{
	uint16_t id = 0;
	
	ili9341_tft_driver_status status = ili9341_tft_driver_get_id(&id);
	if (status || id != ILI9341_ID)
		return ILI9341_ERROR;
	return status;
}

static void ili9341_soft_reset()
{
	io_write_reg(ILI9341_CMD_RESET);
  	io_delay(10);
}

static void ili9341_send_default_registers()
{
	io_write_reg(ILI9341_CMD_POWER_A);
	io_write_data(0x39);
	io_write_data(0x2C);
	io_write_data(0x00);
	io_write_data(0x34);
	io_write_data(0x02);
	io_write_reg(ILI9341_CMD_POWER_B);
	io_write_data(0x00);
	io_write_data(0xC1);
	io_write_data(0x30);
	io_write_reg(ILI9341_CMD_DTCA);
	io_write_data(0x85);
	io_write_data(0x00);
	io_write_data(0x78);
	io_write_reg(ILI9341_CMD_DTCB);
	io_write_data(0x00);
	io_write_data(0x00);
	io_write_reg(ILI9341_CMD_POWER_SEQ);
	io_write_data(0x64);
	io_write_data(0x03);
	io_write_data(0x12);
	io_write_data(0x81);
	io_write_reg(ILI9341_CMD_PRC);
	io_write_data(0x20);
	io_write_reg(ILI9341_CMD_POWER1);
	io_write_data(0x23);
	io_write_reg(ILI9341_CMD_POWER2);
	io_write_data(0x10);
	io_write_reg(ILI9341_CMD_VCOM1);
	io_write_data(0x3E);
	io_write_data(0x28);
	io_write_reg(ILI9341_CMD_VCOM2);
	io_write_data(0x86);
	io_write_reg(ILI9341_CMD_MAC);
	io_write_data(0xE8);
	io_write_reg(ILI9341_CMD_PIXEL_FORMAT);
	io_write_data(0x55); // 16-bit Parallel RGB Interface
	io_write_reg(ILI9341_CMD_FRC); // (frame control in normal mode)
	io_write_data(0x00);
	io_write_data(0x18);
	io_write_reg(ILI9341_CMD_DFC);
	io_write_data(0x08);
	io_write_data(0x82);
	io_write_data(0x27);
	io_write_reg(0xF2);
	io_write_data(0x00);
	io_write_reg(ILI9341_CMD_SLEEP_OUT);
	
	io_delay(10);
}

static void ili9341_display_on()
{
	io_write_reg(ILI9341_CMD_DISPLAY_ON);
}

void ili9341_tft_driver_destroy()
{
	io_struct = NULL;
}

ili9341_tft_driver_status ili9341_tft_driver_get_id(uint16_t *const value)
{
	uint16_t result = 0;
	
	ili9341_tft_driver_status status = io_write_reg(ILI9341_CMD_ID);
	status |= io_dummy_read(2);
	
	status |= io_read_data(2, &result);
	*value = result << 8;
	
	status |= io_read_data(2, &result);
	*value |= result;
	
	return status;
}

ili9341_tft_driver_status ili9341_tft_driver_set_area(
	ili9341_draw_area *const area
)
{
	if (ili9341_is_not_valid_coordinates(area))
    return ILI9341_ERROR;
	
	ili9341_tft_driver_status status = ili9341_write_x_coordinates(
		area->start, area->end
	);
	status |= ili9341_write_y_coordinates(
		area->top, area->bottom
	);
	 
	return status;
}

static bool ili9341_is_not_valid_coordinates(ili9341_draw_area *const area)
{
	return area->start > area->end ||
		area->end > ILI9341_HRES ||
		area->top > area->bottom ||
		area->bottom > ILI9341_VRES;
}

static ili9341_tft_driver_status ili9341_write_x_coordinates(uint16_t x0, uint16_t x1)
{
	ili9341_tft_driver_status status = io_write_reg(ILI9341_CMD_SETX);
  status |= io_write_data(x0 >> 8);
  status |= io_write_data(x0 & 0xFF);
  status |= io_write_data(x1 >> 8);
  status |= io_write_data(x1 & 0xFF);
	
	return status;
}

static ili9341_tft_driver_status ili9341_write_y_coordinates(uint16_t y0, uint16_t y1)
{
	ili9341_tft_driver_status status = io_write_reg(ILI9341_CMD_SETY);
  status |= io_write_data(y0 >> 8);
  status |= io_write_data(y0 & 0xFF);
  status |= io_write_data(y1 >> 8);
  status |= io_write_data(y1 & 0xFF);
	
	return status;
}

ili9341_tft_driver_status ili9341_tft_driver_set_pixel(
	uint16_t x,
	uint16_t y,
	uint16_t color
)
{
	ili9341_draw_area area = (ili9341_draw_area) {
		.start = x,
		.end = x,
		.top = y,
		.bottom = y
	};
	
	ili9341_tft_driver_status status = ili9341_tft_driver_set_area(&area);
	status |= io_write_reg(ILI9341_CMD_WRAM);
	status |= io_write_data(color);
	
	return status;
}

ili9341_tft_driver_status ili9341_tft_driver_fill_area(
	ili9341_draw_area *const area,
	uint16_t color
)
{
	uint32_t pixels_amount = ili9341_calculate_pixels_amount(area);
	ili9341_tft_driver_set_area(area);
	
	ili9341_tft_driver_status status = io_write_reg(ILI9341_CMD_WRAM);
	for (uint32_t i = 0; i < pixels_amount; i++)
		status |= io_write_data(color);
	
	return status;
}

uint16_t ili9341_tft_driver_get_color_from_rgb(
	uint8_t r,
	uint8_t g,
	uint8_t b
)
{
	return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

ili9341_tft_driver_status ili9341_tft_driver_draw_image(
	ili9341_draw_area *const area,
	uint16_t *const data
)
{
	uint32_t pixels_amount = ili9341_calculate_pixels_amount(area);
	ili9341_tft_driver_set_area(area);

	draw_faces(data);
	
	ili9341_tft_driver_status status = io_write_reg(ILI9341_CMD_WRAM);
	for (uint32_t i = 0; i < pixels_amount; i++)
		status |= io_write_data(data[i]);
	
	return status;
}

// __attribute__((always_inline))
// static inline uint16_t decolorize(const uint16_t *pixel)
// {
// 	uint8_t r = *pixel >> 11;
// 	uint8_t g = (*pixel >> 6) & 0x1f; // to 5 digits
// 	uint8_t b = *pixel & 0x1f;
	
// 	uint8_t grayscale = FAST_DIVIDE_BY_3(r + g + b);

// 	return ((uint16_t)grayscale << 11) | ((uint16_t)grayscale << 6) | grayscale;
// }

static uint32_t ili9341_calculate_pixels_amount(ili9341_draw_area *const area)
{
	return (area->end - area->start + 1) * (area->bottom - area->top + 1);
}

ili9341_tft_driver_status ili9341_tft_driver_clear()
{
	ili9341_draw_area max_area = ili9341_get_max_area();
	
	// For some reason, it doesnt work the first time in non-debug mode
	ili9341_tft_driver_status status = ili9341_tft_driver_fill_area(&max_area, 0x0);
	status |= ili9341_tft_driver_fill_area(&max_area, 0x0);
	
	return status;
}

static ili9341_draw_area ili9341_get_max_area()
{
	return (ili9341_draw_area) {
		.start = 0,
		.end = ILI9341_HRES - 1,
		.top = 0,
		.bottom = ILI9341_VRES - 1
	};
}

__attribute__((always_inline))
static inline bool is_not_safe_call(void *const function_ptr)
{
	return io_struct == NULL || function_ptr == NULL;
}

static ili9341_tft_driver_status io_init()
{
	if (is_not_safe_call(io_struct->io_init))
		return ILI9341_ERROR;
	
	io_struct->io_init();
	
	return ILI9341_OK;
}

__attribute__((always_inline))
static inline ili9341_tft_driver_status io_write_data(uint16_t data)
{
	if (is_not_safe_call(io_struct->io_write_data))
		return ILI9341_ERROR;
	
	io_struct->io_write_data(data);
	
	return ILI9341_OK;
}

__attribute__((always_inline))
static inline ili9341_tft_driver_status io_write_reg(uint16_t reg_value)
{
	if (is_not_safe_call(io_struct->io_write_reg))
		return ILI9341_ERROR;
	
	io_struct->io_write_reg(reg_value);
	
	return ILI9341_OK;
}

__attribute__((always_inline))
static inline ili9341_tft_driver_status io_read_data(
	uint8_t data_size,
	uint16_t *const value
)
{
	if (is_not_safe_call(io_struct->io_read_data))
		return ILI9341_ERROR;
	
	*value = io_struct->io_read_data(data_size);
	
	return ILI9341_OK;
}

static ili9341_tft_driver_status io_delay(uint32_t delay) {
	if (is_not_safe_call(io_struct->io_delay))
		return ILI9341_ERROR;
	
	io_struct->io_delay(delay);
	
	return ILI9341_OK;
}

static ili9341_tft_driver_status io_dummy_read(uint8_t amount)
{
	if (is_not_safe_call(io_struct->io_read_data))
		return ILI9341_ERROR;
	
	for (uint8_t i = 0; i < amount; i++)
		(void)io_struct->io_read_data(2);
	
	return ILI9341_OK;
}

static ili9341_tft_driver_status io_enable_backlight(bool is_enabled)
{
	if (is_not_safe_call(io_struct->io_enable_backlight))
		return ILI9341_ERROR;
	
	io_struct->io_enable_backlight(is_enabled);
	
	return ILI9341_OK;
}
