#ifndef LBP_FEATURE_DEFS_H
#define LBP_FEATURE_DEFS_H

#include <stdint.h>

enum {
  LBP_FEATURE_RECTANGLES_AMOUNT = 9U,
  LBP_FEATURE_MASKS_AMOUNT = 8U,
  LBP_FEATURE_RECT_UP_LEFT = 0U,
  LBP_FEATURE_RECT_UP_CENTER = 1U,
  LBP_FEATURE_RECT_UP_RIGHT = 2U,
  LBP_FEATURE_RECT_LEFT = 3U,
  LBP_FEATURE_RECT_CENTER = 4U,
  LBP_FEATURE_RECT_RIGHT = 5U,
  LBP_FEATURE_RECT_DOWN_LEFT = 6U,
  LBP_FEATURE_RECT_DOWN_CENTER = 7U,
  LBP_FEATURE_RECT_DOWN_RIGHT = 8U
};

typedef struct
{
	uint8_t x;
	uint8_t y;
	uint8_t width;
	uint8_t height;
} lbp_feature_rectangle;

typedef struct
{
  uint8_t offset_x;
  uint8_t offset_y;
  uint8_t scale_index;
} lbp_feature_arguments;

typedef struct
{
	lbp_feature_rectangle rectangles[LBP_FEATURE_RECTANGLES_AMOUNT];
  // There should just be a pointer - a dynamic array, since we
  // generate them at runtime and don't know the size
	lbp_feature_rectangle **scaled_rectangles;
  uint8_t scaled_rectangles_amount;
	int32_t masks[LBP_FEATURE_MASKS_AMOUNT];
	int16_t left_value;
	int16_t right_value;
} lbp_feature;

typedef struct
{
  uint16_t (*get_rectangle_summarize)(
    const lbp_feature_rectangle *const
  );
} lbp_feature_integral_image_handler;


#endif
