#ifndef AREA_H
#define AREA_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct
{
	uint8_t x;
	uint8_t y;
  uint8_t size;
} area;

__attribute__((always_inline))
inline static bool area_is_similar(
  const area *const first_area,
  const area *const second_area
)
{
  int16_t threshold = first_area->size * 0.2f;

  int16_t delta_x = first_area->x - second_area->x;
  int16_t delta_y = first_area->y - second_area->y;
  int16_t delta_size = second_area->size - first_area->size;
  int16_t delta_size_x = second_area->size - (delta_x + first_area->size);
  int16_t delta_size_y = second_area->size - (delta_y + first_area->size);

  // Similar in position and size
  if (abs(delta_x) <= threshold && abs(delta_y) <= threshold &&
    abs(delta_size) <= threshold)
    return true;

  // The second area includes the first
  if (delta_x >= 0 && delta_y >= 0 && delta_size_x >= 0 && delta_size_y >= 0)
    return true;

  return false;
}

#endif
