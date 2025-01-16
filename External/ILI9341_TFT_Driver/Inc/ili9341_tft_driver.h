#ifndef ILI9341_TFT_DRIVER_H
#define ILI9341_TFT_DRIVER_H

#include "ili9341_tft_driver_defs.h"
#include <stdint.h>

ili9341_tft_driver_status ili9341_tft_driver_init(
	ili9341_tft_driver_io_struct *const io,
	void (*draw_faces_functor)(uint16_t *const image)
);
void ili9341_tft_driver_destroy(void);
ili9341_tft_driver_status ili9341_tft_driver_get_id(uint16_t *const value);
ili9341_tft_driver_status ili9341_tft_driver_set_area(
	ili9341_draw_area *const area
);
ili9341_tft_driver_status ili9341_tft_driver_set_pixel(
	uint16_t x,
	uint16_t y,
	uint16_t color
);
ili9341_tft_driver_status ili9341_tft_driver_fill_area(
	ili9341_draw_area *const area,
	uint16_t color
);
uint16_t ili9341_tft_driver_get_color_from_rgb(
	uint8_t r,
	uint8_t g,
	uint8_t b
);
ili9341_tft_driver_status ili9341_tft_driver_draw_image(
	ili9341_draw_area *const area,
	uint16_t *const data
);
ili9341_tft_driver_status ili9341_tft_driver_clear(void);

#endif
