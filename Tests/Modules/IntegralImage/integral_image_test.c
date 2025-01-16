#include "unity_fixture.h"
#include "integral_image.h"
#include <string.h>

// Defines -------------------------------------------------------------------

enum {
  IMAGE_SIZE = 6U
};

// Static variables ----------------------------------------------------------

static uint8_t image[IMAGE_SIZE][IMAGE_SIZE] = {
  { 3, 1, 4, 1, 5, 9 },
  { 2, 6, 5, 3, 5, 8 },
  { 9, 7, 9, 3, 2, 3 },
  { 8, 4, 6, 2, 6, 4 },
  { 3, 3, 8, 3, 2, 7 },
  { 9, 5, 0, 2, 8, 8 }
};

// Static functions ----------------------------------------------------------

static void fill_integral_image(
  FILL_LINE_FUNCTOR,
  const uint8_t *const source_image,
  integral_image_size image_size
)
{
  uint16_t convertion_buffer[IMAGE_SIZE];

  for (uint8_t y = 0; y < image_size.height; y++)
  {
    for (uint8_t x = 0; x < image_size.width; x++)
      convertion_buffer[x] = *(source_image + x + (y * image_size.width));

    fill_line((uint16_t*)convertion_buffer, y);
  }
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(integral_image);

TEST_SETUP(integral_image)
{
  integral_image_create(
    (integral_image_size) {
      .width = 6U,
      .height = 6U
    },
    fill_integral_image
  );

  integral_image_set((uint8_t*)image);
}

TEST_TEAR_DOWN(integral_image)
{
  integral_image_destroy();
}

TEST(integral_image, set_image_is_ok)
{
  integral_image_rectangle_position first_column = {
    .top_left_corner = { .x = 0, .y = 0 },
    .bottom_right_corner = { .x = 0, .y = 7 }
  };
  integral_image_rectangle_position first_row = {
    .top_left_corner = { .x = 0, .y = 0 },
    .bottom_right_corner = { .x = 0, .y = 7 }
  };

  TEST_ASSERT_EQUAL(0, integral_image_get_rectangle(&first_column));
  TEST_ASSERT_EQUAL(0, integral_image_get_rectangle(&first_row));
}

TEST(integral_image, total_summarize_is_ok)
{
  integral_image_rectangle_position pos = {
    .top_left_corner = {
      .x = 1,
      .y = 1
    },
    .bottom_right_corner = {
      .x = 6,
      .y = 6
    }
  };

  integral_image_calculate();

  TEST_ASSERT_EQUAL(119, integral_image_get_rectangle(&pos));
}

TEST(integral_image, left_top_part_summarize_is_ok)
{
  integral_image_rectangle_position pos = {
    .top_left_corner = {
      .x = 1,
      .y = 1
    },
    .bottom_right_corner = {
      .x = 3,
      .y = 3
    }
  };

  integral_image_calculate();

  TEST_ASSERT_EQUAL(27, integral_image_get_rectangle(&pos));
}

TEST(integral_image, right_top_part_summarize_is_ok)
{
  integral_image_rectangle_position pos = {
    .top_left_corner = {
      .x = 4,
      .y = 1
    },
    .bottom_right_corner = {
      .x = 6,
      .y = 3
    }
  };

  integral_image_calculate();

  TEST_ASSERT_EQUAL(18, integral_image_get_rectangle(&pos));
}

TEST(integral_image, left_bottom_part_summarize_is_ok)
{
  integral_image_rectangle_position pos = {
    .top_left_corner = {
      .x = 1,
      .y = 4
    },
    .bottom_right_corner = {
      .x = 3,
      .y = 6
    }
  };

  integral_image_calculate();

  TEST_ASSERT_EQUAL(16, integral_image_get_rectangle(&pos));
}

TEST(integral_image, right_bottom_part_summarize_is_ok)
{
  integral_image_rectangle_position pos = {
    .top_left_corner = {
      .x = 4,
      .y = 4
    },
    .bottom_right_corner = {
      .x = 6,
      .y = 6
    }
  };

  integral_image_calculate();

  TEST_ASSERT_EQUAL(25, integral_image_get_rectangle(&pos));
}
