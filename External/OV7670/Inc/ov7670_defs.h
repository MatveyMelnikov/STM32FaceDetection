#ifndef OV7670_DEFS_H
#define OV7670_DEFS_H

#include <stdint.h>

typedef enum {
  OV7670_OK = 0x00U,
  OV7670_ERROR = 0x01U,
  OV7670_BUSY = 0x02U,
  OV7670_ERROR_TIMEOUT = 0x03U
} ov7670_status;

typedef struct {
  // dev_address, data, data_size, timeout
  ov7670_status (*receive)(
    const uint16_t,
    uint8_t *const,
    const uint16_t,
    const uint16_t
  );
  // dev_address, data, data_size, timeout
  ov7670_status (*transmit)(
    const uint16_t,
    const uint8_t *const,
    const uint16_t,
    const uint16_t
  );
  // data, data_size
  ov7670_status (*start_receiving_frames)(uint32_t, uint32_t);
} ov7670_io_handler;

typedef enum
{
  WRITE_ADDR = 0x42U,
  READ_ADDR = 0x43U
} ov7670_addr;

typedef enum
{
  VREF = 0x03U,
  PID = 0xaU,
  VER = 0xbU,
  COM_7 = 0x12U,
  CLKRC = 0x11U,
  COM_3 = 0x0cU,
  COM_10 = 0x15U,
  COM_8 = 0x13U,
  HSTART = 0x17U,
  HSTOP = 0x18U,
  VSTRT = 0x19U,
  VSTOP = 0x1AU,
  EXHCH = 0x2aU,
  EXHCL = 0x2bU,
  HREF = 0x32U,
  TSLB = 0x3aU,
  COM_12 = 0x3cU,
  COM_13 = 0x3dU,
  COM_14 = 0x3eU,
  COM_15 = 0x40U,
  EDGE = 0x3fU,
  DBLV = 0x6bU,
  SCALING_XSC = 0x70U,
  SCALING_YSC = 0x71U,
  SCALING_DCWCTR = 0x72U,
  SCALING_PCLK_DIV = 0x73U,
  SCALING_PCLK_DELAY = 0xa2U,
  MTX1 = 0x4f,
  MTX2 = 0x50,
  MTX3 = 0x51,
  MTX4 = 0x52,
  MTX5 = 0x53,
  MTX6 = 0x54,
  MTXS = 0x58,
} ov7670_register_addr;

enum
{
  OV7670_IMAGE_WIDTH = 172U,
  OV7670_IMAGE_HEIGHT = 144U,
  OV7670_TIMEOUT = 500U,
  PIXEL_BUFFER_SIZE = 24768U, // 172 x 144 (RGB, 1 px = 2 byte)
  REGISTERS_NUM = 123U
};

#endif
