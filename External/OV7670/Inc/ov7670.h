#ifndef OV7670_H
#define OV7670_H

#include "ov7670_defs.h"

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
  ov7670_io_handler handler,
  const ov7670_window_size window
);
void ov7670_destroy(void);
ov7670_status ov7670_check_link(void);
ov7670_status ov7670_reset(void);
ov7670_status ov7670_start_capture(void);
void ov7670_send_captured_frame(void);
void ov7670_frame_received(void);
ov7670_status ov7670_set_test_pattern(void);
ov7670_status ov7670_set_rgb_format(void);
ov7670_status ov7670_send_default_registers(void);
uint32_t *ov7670_get_captured_data(void);
ov7670_status ov7670_set_plck_prescalar(uint8_t pre_scalar);
ov7670_status ov7670_set_qcif(void);

#endif
