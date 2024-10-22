#include "integral_image.h"
#include "simd_instructions.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Static variables ----------------------------------------------------------

// static uint16_t *integral_image = NULL;
__attribute__((section(".ccmram")))
static uint16_t integral_image[INTEGRAL_IMAGE_SIZE];
static integral_image_size image_size = { 0 };
static integral_image_size source_image_size = { 0 };
static void (*fill_image)(
  FILL_LINE_FUNCTOR,
  const uint8_t *const,
  integral_image_size
);

// Defines -------------------------------------------------------------------

#define GET_PIXEL(arr, x, y) \
	arr[(x) + (y) * image_size.width]

// Static prototypes ---------------------------------------------------------

static void integral_image_fill_line(
  const uint16_t *const image_data,
  const uint8_t line_index
);
static void integral_image_calculate_initial_column_and_line(void);
static void integral_image_calculate_top_line(void);
static void integral_image_calculate_start_column(void);
static void integral_image_calculate_other_elements(void);
__attribute__((always_inline))
static inline void integral_image_calculate_group(uint16_t x);

// Implementations -----------------------------------------------------------

void integral_image_create(integral_image_size size, FILL_IMAGE_FUNCTOR)
{
  image_size = (integral_image_size) {
    .height = size.height + 1,
    .width = size.width + 1
  };
  source_image_size = size;

  fill_image = fill_integral_image_line;
}

void integral_image_set(const uint8_t *const image)
{
  fill_image(integral_image_fill_line, image, source_image_size);
}

static void integral_image_fill_line(
  const uint16_t *const image_data,
  const uint8_t line_index
)
{
  memcpy(
    integral_image + 1 + ((line_index + 1) * image_size.width),
    image_data,
    source_image_size.width * sizeof(uint16_t)
  );
}

void integral_image_destroy(void)
{
  image_size = (integral_image_size){ 0 };
}

void integral_image_calculate(void)
{
	integral_image_calculate_initial_column_and_line();
	integral_image_calculate_other_elements();
}

static void integral_image_calculate_initial_column_and_line()
{
	integral_image_calculate_top_line();
	integral_image_calculate_start_column();
}

static void integral_image_calculate_top_line()
{
	for (uint16_t x = 0; x < image_size.width; x++)
	{
		GET_PIXEL(integral_image, x, 0) = 0U;
	}
}

static void integral_image_calculate_start_column()
{
	for (uint16_t y = 1; y < image_size.height; y++)
	{
		GET_PIXEL(integral_image, 0, y) = 0U;
	}
}

static void integral_image_calculate_other_elements()
{
	for (uint16_t x = 1; x < image_size.width; x += 2)
	{
		integral_image_calculate_group(x);
	}
}

__attribute__((always_inline))
static inline void integral_image_calculate_group(uint16_t x)
{
	union {
		uint16_t parts[2];
		uint32_t full;
	} row_sum = { 0 };

	for (uint16_t y = 1; y < image_size.height; y++)
	{
		row_sum.full = UQADD16(
			row_sum.full,
			*(uint32_t*)(integral_image + x + y * image_size.width)
		);

		GET_PIXEL(integral_image, x, y) = GET_PIXEL(integral_image, x - 1, y) +
			row_sum.parts[0];
		GET_PIXEL(integral_image, x + 1, y) = GET_PIXEL(integral_image, x, y) +
			row_sum.parts[1];
	}
}

const uint16_t integral_image_get_rectangle(
	integral_image_rectangle_position* const rectangle
)
{
	uint16_t top_left_corner = GET_PIXEL(
		integral_image,
		rectangle->top_left_corner.x,
		rectangle->top_left_corner.y
	);
	uint16_t bottom_right_corner = GET_PIXEL(
		integral_image,
		rectangle->bottom_right_corner.x,
		rectangle->bottom_right_corner.y
	);
	uint16_t top_right_corner = GET_PIXEL(
		integral_image,
		rectangle->bottom_right_corner.x,
		rectangle->top_left_corner.y
	);
	uint16_t bottom_left_corner = GET_PIXEL(
		integral_image,
		rectangle->top_left_corner.x,
		rectangle->bottom_right_corner.y
	);

	uint16_t res = bottom_right_corner + top_left_corner -
			top_right_corner - bottom_left_corner;

	return res;
}
