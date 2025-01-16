#ifndef MOCK_INTEGRAL_IMAGE_H
#define MOCK_INTEGRAL_IMAGE_H

#include <stdint.h>
#include "integral_image_defs.h"

void mock_integral_image_create(const uint8_t max_expectations);
void mock_integral_image_destroy(void);
void mock_integral_image_expect_read_then_return(
  const uint16_t *const data
);
void mock_integral_image_verify_complete(void);
uint16_t mock_integral_image_get_rectangle(
	integral_image_rectangle_position *const rectangle
);


#endif
