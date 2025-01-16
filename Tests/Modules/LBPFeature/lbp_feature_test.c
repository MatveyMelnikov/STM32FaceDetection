#include "unity_fixture.h"
#include "lbp_feature.h"
#include "mock_integral_image.h"
#include <stddef.h>

// Defines -------------------------------------------------------------------

enum {
  MOCK_INTEGRAL_IMAGE_SIZE = 10U,
};

#define GET_ARRAY_SIZE(arr) \
  sizeof((arr)) / sizeof((arr)[0])

// Static variables ----------------------------------------------------------

static lbp_feature feature = { 0 };

// Static functions ----------------------------------------------------------

static uint16_t get_rectangle_summarize(
  const lbp_feature_rectangle *const feature_rectangle
)
{
  integral_image_rectangle_position integral_position = {
    .top_left_corner = {
      .x = feature_rectangle->x,
      .y = feature_rectangle->y
    },
    .bottom_right_corner = {
      .x = feature_rectangle->x + feature_rectangle->width,
      .y = feature_rectangle->y + feature_rectangle->height
    }
  };

  return mock_integral_image_get_rectangle(&integral_position);
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(lbp_feature_test);

TEST_SETUP(lbp_feature_test)
{
  mock_integral_image_create(MOCK_INTEGRAL_IMAGE_SIZE);
  lbp_feature_set_integral_image_handler(
    (lbp_feature_integral_image_handler) {
      .get_rectangle_summarize = get_rectangle_summarize
    }
  );

  feature = (lbp_feature) {
    .left_value = 2174,
    .right_value = -4054,
    .masks = { 
      -67130709, -21569, -1426120013, -1275125205,
      -21585, -16385, 587145899, -24005 
    },
    .scaled_rectangles = NULL,
    .scaled_rectangles_amount = 0,
    .rectangles = {
      { .x = 6, .y = 5, .width = 4, .height = 3 },
      { .x = 10, .y = 5, .width = 4, .height = 3 },
      { .x = 14, .y = 5, .width = 4, .height = 3 },
      { .x = 6, .y = 8, .width = 4, .height = 3 },
      { .x = 10, .y = 8, .width = 4, .height = 3 },
      { .x = 14, .y = 8, .width = 4, .height = 3 },
      { .x = 6, .y = 11, .width = 4, .height = 3 },
      { .x = 10, .y = 11, .width = 4, .height = 3 },
      { .x = 14, .y = 11, .width = 4, .height = 3 }
    }
  };
}

TEST_TEAR_DOWN(lbp_feature_test)
{
  lbp_feature_destroy(&feature);
  lbp_feature_reset_integral_image_handler();

  mock_integral_image_verify_complete();
  mock_integral_image_destroy();
}

TEST(lbp_feature_test, calculate_vote_right_value_returned)
{
  uint16_t summarize_region_values[9] = {
    945, 924, 809, 834, 1050, 1006, 1109, 1080, 1188
  };

  for (
    uint8_t i = 0;
    i < GET_ARRAY_SIZE(summarize_region_values);
    i++
  )
  {
    mock_integral_image_expect_read_then_return(
      (uint16_t*)&summarize_region_values[i]
    );
  }

  lbp_feature_arguments vote_arguments = (lbp_feature_arguments) {
    .offset_x = 0U,
    .offset_y = 0U,
    .scale = 1.f
  };
  float result =  lbp_feature_calculate_vote(&feature, &vote_arguments);

  TEST_ASSERT_EQUAL_FLOAT(feature.right_value, result);
}

TEST(lbp_feature_test, calculate_vote_left_value_returned)
{
  uint16_t summarize_region_values[9] = {
    947, 887, 849, 787, 934, 936, 898, 960, 988
  };

  for (
    uint8_t i = 0;
    i < GET_ARRAY_SIZE(summarize_region_values);
    i++
  )
  {
    mock_integral_image_expect_read_then_return(
      (uint16_t*)&summarize_region_values[i]
    );
  }

  lbp_feature_arguments vote_arguments = (lbp_feature_arguments) {
    .offset_x = 0U,
    .offset_y = 0U,
    .scale = 1.f
  };
  float result =  lbp_feature_calculate_vote(&feature, &vote_arguments);

  TEST_ASSERT_EQUAL_FLOAT(feature.left_value, result);
}
