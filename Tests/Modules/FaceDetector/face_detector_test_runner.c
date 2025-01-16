#include "unity_fixture.h"

TEST_GROUP_RUNNER(face_detector)
{
  RUN_TEST_CASE(face_detector, create_is_ok);
  RUN_TEST_CASE(face_detector, detect_with_one_window_is_ok);
  RUN_TEST_CASE(face_detector, detect_with_three_windows_is_ok);
}