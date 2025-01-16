#include "lbp_feature.h"
#include <stdlib.h>

// Static variables ----------------------------------------------------------

static lbp_feature_integral_image_handler integral_image_handler;

// Defines -------------------------------------------------------------------

#define GET_RECT_WITH_OFFSET(rect, offset_x, offset_y) \
  (lbp_feature_rectangle) { \
    .x = rect.x + offset_x, \
    .y = rect.y + offset_y, \
    .width = rect.width, \
    .height = rect.height \
  }

// Static prototypes ---------------------------------------------------------

static void lbp_feature_generate_scaled_rectangles(
  lbp_feature *const self,
  lbp_feature_rectangle *const scaled_rectangles,
  const float scale
);
static uint8_t lbp_feature_get_score(
  const lbp_feature_rectangle *const scaled_rectangles,
  const lbp_feature_arguments *const arguments
);
__attribute__((always_inline))
static inline uint8_t lbp_feature_get_lbp_value(
  const lbp_feature_rectangle *const scaled_rectangles,
  uint8_t rectangle_index,
  const lbp_feature_arguments *const arguments,
  const uint16_t center_value,
  const uint8_t success_value
);

// Implementations -----------------------------------------------------------

void lbp_feature_set_integral_image_handler(
  lbp_feature_integral_image_handler handler
)
{
  integral_image_handler = handler;
}

void lbp_feature_reset_integral_image_handler()
{
  integral_image_handler = (lbp_feature_integral_image_handler){ 0 };
}

float lbp_feature_calculate_vote(
  lbp_feature *const self,
  const lbp_feature_arguments *const arguments
)
{
  static lbp_feature_rectangle scaled_rectangles[LBP_FEATURE_RECTANGLES_AMOUNT];
  lbp_feature_generate_scaled_rectangles(
    self,
    (lbp_feature_rectangle*)scaled_rectangles,
    arguments->scale
  );

  uint8_t score = lbp_feature_get_score(
    (lbp_feature_rectangle*)scaled_rectangles,
    arguments
  );

  return (self->masks[score >> 5] & (1 << (score & 31))) ?
    self->right_value : self->left_value;
}

static void lbp_feature_generate_scaled_rectangles(
  lbp_feature *const self,
  lbp_feature_rectangle *const scaled_rectangles,
  const float scale
)
{
  for (uint8_t i = 0; i < LBP_FEATURE_RECTANGLES_AMOUNT; i++)
  {
    lbp_feature_rectangle current_rectangle = self->rectangles[i];

    scaled_rectangles[i] = (lbp_feature_rectangle) {
      .x = current_rectangle.x * scale,
      .y = current_rectangle.y * scale,
      .width = current_rectangle.width * scale,
      .height = current_rectangle.height * scale
    };
  }
}

static uint8_t lbp_feature_get_score(
  const lbp_feature_rectangle *const scaled_rectangles,
  const lbp_feature_arguments *const arguments
)
{
  uint8_t score = 0;
  
  lbp_feature_rectangle center_rect = GET_RECT_WITH_OFFSET(
    scaled_rectangles[LBP_FEATURE_RECT_CENTER],
    arguments->offset_x,
    arguments->offset_y
  );
  uint16_t center_value = integral_image_handler.get_rectangle_summarize(
    &center_rect
  );

  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_UP_LEFT,
    arguments,
    center_value,
    128U
  );
  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_UP_CENTER,
    arguments,
    center_value,
    64U
  );
  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_UP_RIGHT,
    arguments,
    center_value,
    32U
  );

  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_LEFT,
    arguments,
    center_value,
    1U
  );
  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_RIGHT,
    arguments,
    center_value,
    16U
  );

  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_DOWN_LEFT,
    arguments,
    center_value,
    2U
  );
  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_DOWN_CENTER,
    arguments,
    center_value,
    4U
  );
  score |= lbp_feature_get_lbp_value(
    scaled_rectangles,
    LBP_FEATURE_RECT_DOWN_RIGHT,
    arguments,
    center_value,
    8U
  );

  return score;
}

__attribute__((always_inline))
static inline uint8_t lbp_feature_get_lbp_value(
  const lbp_feature_rectangle *const scaled_rectangles,
  uint8_t rectangle_index,
  const lbp_feature_arguments *const arguments,
  const uint16_t center_value,
  const uint8_t success_value
)
{
  lbp_feature_rectangle rect_position = GET_RECT_WITH_OFFSET(
    scaled_rectangles[rectangle_index],
    arguments->offset_x,
    arguments->offset_y
  );

  if (
    integral_image_handler.get_rectangle_summarize(&rect_position) >=
      center_value
  )
    return success_value;
  else
    return 0;
}

void lbp_feature_destroy(lbp_feature *const self)
{
  if (self->scaled_rectangles_amount == 0)
    return;

  for (
    uint8_t scaled_rectangles_index = 0;
    scaled_rectangles_index < self->scaled_rectangles_amount;
    scaled_rectangles_index++
  )
  {
    free(self->scaled_rectangles[scaled_rectangles_index]);
  }
  free(self->scaled_rectangles);

  self->scaled_rectangles_amount = 0;
}
