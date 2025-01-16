#include "unity_fixture.h"

TEST_GROUP_RUNNER(integral_image)
{
  RUN_TEST_CASE(integral_image, set_image_is_ok);
  RUN_TEST_CASE(integral_image, total_summarize_is_ok);
  RUN_TEST_CASE(integral_image, left_top_part_summarize_is_ok);
  RUN_TEST_CASE(integral_image, right_top_part_summarize_is_ok);
  RUN_TEST_CASE(integral_image, left_bottom_part_summarize_is_ok);
  RUN_TEST_CASE(integral_image, right_bottom_part_summarize_is_ok);
}