#include "ov7670.h"
#include <stdbool.h>
#include <string.h>
#include "ov7670_defs.h"
#include "ili9341_tft_driver.h"
#include "main.h"

// Static variables ----------------------------------------------------------

//static I2C_HandleTypeDef *i2c_io;
static ov7670_io_handler module_handler;
static uint16_t pixel_buffer[PIXEL_BUFFER_SIZE];
static volatile bool is_frame_captured = false;
static ili9341_draw_area area;

// Static functions prototypes -----------------------------------------------

static ov7670_status ov7670_set_fast_auto_exposure_and_gain_control(void);
static ov7670_status ov7670_set_edge_enhancement_factor(void);
static ov7670_status ov7670_set_uv_auto_adjustment(void);
static ov7670_status ov7670_enable_color_image(void);
__attribute__((always_inline))
static inline 
ov7670_status ov7670_add_byte(
  const uint8_t sub_addr,
  const uint8_t data
);
__attribute__((always_inline))
static inline ov7670_status ov7670_read_byte(
  const uint8_t sub_addr,
  uint8_t *const data
);
__attribute__((always_inline))
static inline ov7670_status ov7670_write_addr(
  const uint8_t sub_addr
);
__attribute__((always_inline))
static inline ov7670_status ov7670_write_byte(
  const uint8_t sub_addr,
  const uint8_t data
);

// Functions implementations -------------------------------------------------

void ov7670_create(
  ov7670_io_handler handler,
  const ov7670_window_size window
)
{
  module_handler = handler;
  area = (ili9341_draw_area) {
    .start = 0,
    .end = window.width - 1,
    .top = 0,
    .bottom = window.height - 1
  };
}

void ov7670_destroy(void)
{
  module_handler = (ov7670_io_handler){ 0 };
}

ov7670_status ov7670_check_link()
{
  uint8_t received_data = 0x0;

  ov7670_status status = ov7670_read_byte(PID, &received_data);
  status |= (received_data == 0x76) ? HAL_OK : HAL_ERROR;
  
  return status;
}

ov7670_status ov7670_reset()
{
  // 7 bit - reset
  ov7670_status status = ov7670_write_byte(COM_7, 0x80);
  HAL_Delay(1); // Datasheet pg. 6
  
  return status;
}

ov7670_status ov7670_start_capture()
{
  is_frame_captured = false;
  
  module_handler.start_receiving_frames(
    (uint32_t)pixel_buffer,
    PIXEL_BUFFER_SIZE / 2U
  );
  
  return OV7670_OK;
}

void ov7670_frame_received(void)
{
  is_frame_captured = true;
}

void ov7670_send_captured_frame(void)
{
  if (!is_frame_captured)
    return;

  ili9341_tft_driver_draw_image(&area, (uint16_t*)pixel_buffer);
  
  is_frame_captured = false;
}

// Enable 8-bar color bar
// TEST PATTERN - datasheet pg. 21
ov7670_status ov7670_set_test_pattern()
{
  ov7670_status status = ov7670_add_byte(SCALING_YSC, 0x80);
  
  return status;
}

// VGA processed bayer rgb mode (565)
// Implementation Guide pg. 8
ov7670_status ov7670_set_rgb_format()
{
  ov7670_status status = ov7670_add_byte(COM_7, 0x4);
  status |= ov7670_add_byte(COM_15, 0x10);
  
  return status;
}

ov7670_status ov7670_send_default_registers()
{
  ov7670_status status = ov7670_set_fast_auto_exposure_and_gain_control();
  status |= ov7670_set_edge_enhancement_factor();
  status |= ov7670_set_uv_auto_adjustment();
  status |= ov7670_enable_color_image();
  
  return status;
}

static ov7670_status ov7670_set_fast_auto_exposure_and_gain_control()
{
  return ov7670_write_byte(COM_8, 0x81);
}

static ov7670_status ov7670_set_edge_enhancement_factor()
{
  return ov7670_write_byte(EDGE, 0x01);
}

static ov7670_status ov7670_set_uv_auto_adjustment()
{
  return ov7670_write_byte(COM_13, 0xd9); 
}

static ov7670_status ov7670_enable_color_image()
{
  return ov7670_write_byte(0xb0, 0x84); // Undocumented register
}

uint32_t *ov7670_get_captured_data()
{
  return (uint32_t*)pixel_buffer;
}

ov7670_status ov7670_set_plck_prescalar(uint8_t pre_scalar)
{
  return ov7670_write_byte(CLKRC, pre_scalar & 0x1f);
}

ov7670_status ov7670_set_qcif()
{
  ov7670_status status = ov7670_write_byte(COM_3, 0x8);
  status |= ov7670_write_byte(COM_7, 0x8);
  
  return status;
}

__attribute__((always_inline))
static inline 
ov7670_status ov7670_add_byte(
  const uint8_t sub_addr,
  const uint8_t data
)
{
  uint8_t received_data = 0;
  ov7670_status status = ov7670_read_byte(sub_addr, &received_data);
  if (status)
    return status;
  
  return ov7670_write_byte(sub_addr, received_data | data);
}

__attribute__((always_inline))
static inline ov7670_status ov7670_read_byte(
  const uint8_t sub_addr,
  uint8_t *const data
)
{
  ov7670_status status = ov7670_write_addr(sub_addr);
  status |= module_handler.receive(
    READ_ADDR,
    data,
    sizeof(data),
    OV7670_TIMEOUT
  );

  return status;
}

__attribute__((always_inline))
static inline ov7670_status ov7670_write_addr(
  const uint8_t sub_addr
)
{
  return module_handler.transmit(
    WRITE_ADDR,
    (uint8_t*)&sub_addr,
    sizeof(sub_addr),
    OV7670_TIMEOUT
  );
}

__attribute__((always_inline))
static inline ov7670_status ov7670_write_byte(
  const uint8_t sub_addr,
  const uint8_t data
)
{
  uint8_t sent_data[] = { sub_addr, data };

  return module_handler.transmit(
    WRITE_ADDR,
    sent_data,
    sizeof(uint8_t) * 2,
    OV7670_TIMEOUT
  );
}
