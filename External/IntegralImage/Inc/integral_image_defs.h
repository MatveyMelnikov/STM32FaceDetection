#ifndef INTEGRAL_IMAGE_DEFS_H
#define INTEGRAL_IMAGE_DEFS_H

#include <stdint.h>

// args: image data line (grayscale pixels), line index
#define FILL_LINE_FUNCTOR void (*fill_line)( \
    const uint16_t *const, \
    const uint8_t \
  )
#define FILL_IMAGE_FUNCTOR void (*fill_integral_image_line)( \
    FILL_LINE_FUNCTOR, \
    const uint8_t *const, \
    integral_image_size \
  )

typedef struct
{
	uint8_t width;
	uint8_t height;
} integral_image_size;

typedef struct
{
	uint8_t x;
	uint8_t y;
} integral_image_position;

typedef struct
{
	integral_image_position top_left_corner;
	integral_image_position bottom_right_corner;
} integral_image_rectangle_position;

#endif