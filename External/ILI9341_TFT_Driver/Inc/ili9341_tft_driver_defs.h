#ifndef ILI9341_TFT_DRIVER_DEFS_H
#define ILI9341_TFT_DRIVER_DEFS_H

#include <stdint.h>
#include <stdbool.h>

#define FMC_BANK1_CMD ((volatile uint16_t *)0x60000000) // RS = 0
#define FMC_BANK1_DATA ((volatile uint16_t *)0x60080000) // RS = 1 (A18)

typedef struct {
	void (*io_init)(void);
	void (*io_write_data)(uint16_t); // data value
	void (*io_write_reg)(uint16_t); // reg value
	uint16_t (*io_read_data)(uint8_t); // offset, data size
	void (*io_delay)(uint32_t); // delay
	void (*io_enable_backlight)(bool); // is_enabled
} ili9341_tft_driver_io_struct;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} ili9341_rgb_pixel;

typedef struct {
	uint16_t start;
	uint16_t end;
	uint16_t top;
	uint16_t bottom;
} ili9341_draw_area;

typedef enum {
	ILI9341_OK,
	ILI9341_ERROR
} ili9341_tft_driver_status;

enum // consts
{
	ILI9341_ID = 0x9341U,
	ILI9341_HRES = 320U,
	ILI9341_VRES = 240U
};

enum // cmd
{
	ILI9341_CMD_RESET = 0x01,
	ILI9341_CMD_ID = 0xD3, // id4
	ILI9341_CMD_SETX = 0x2A,
	ILI9341_CMD_SETY = 0x2B,
	ILI9341_CMD_DISPLAY_ON = 0x29,
	ILI9341_CMD_WRAM = 0x2C,
	ILI9341_CMD_POWER_B = 0xCF,
	ILI9341_CMD_POWER_SEQ = 0xED,
	ILI9341_CMD_DTCA = 0xE8,
	ILI9341_CMD_POWER_A = 0xCB,
	ILI9341_CMD_PRC = 0xF7,
	ILI9341_CMD_DTCB = 0xEA,
	ILI9341_CMD_FRMCTR1 = 0xB1,
	ILI9341_CMD_DFC = 0xB6,
	ILI9341_CMD_POWER1 = 0xC0,
	ILI9341_CMD_POWER2 = 0xC1,
	ILI9341_CMD_VCOM1 = 0xC5,
	ILI9341_CMD_VCOM2 = 0xC7,
	ILI9341_CMD_MAC = 0x36,
	ILI9341_CMD_3GAMMA_EN = 0xF2,
	ILI9341_CMD_RGB_INTERFACE = 0xB0,
	ILI9341_CMD_COLUMN_ADDR = 0x2A,
	ILI9341_CMD_PAGE_ADDR = 0x2B,
	ILI9341_CMD_LCD_INTERFACE = 0xF6,
	ILI9341_CMD_SLEEP_OUT = 0x11,
	ILI9341_CMD_PIXEL_FORMAT = 0x3A,
	ILI9341_CMD_FRC = 0xB1
};

#endif
