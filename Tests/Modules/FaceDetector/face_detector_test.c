#include "unity_fixture.h"
#include "face_detector.h"
#include "mock_stage.h"
#include <string.h>

// Defines -------------------------------------------------------------------

enum {
  STAGE_MOCK_SIZE = 20U,
  STAGES_AMOUNT = 3U,
  MIN_NEIGHBOURS = 0U
};

#define BASE_SCALE 1.f
#define SCALE_INCREMENT 1.1f
#define POSITION_INCREMENT 0.1f

// Static variables ----------------------------------------------------------

static stage stages[STAGES_AMOUNT] = {
  (stage) {
    .features = NULL,
    .features_amount = 0,
    .threshold = 0.f
  },
  (stage) {
    .features = NULL,
    .features_amount = 0,
    .threshold = 0.f
  },
  (stage) {
    .features = NULL,
    .features_amount = 0,
    .threshold = 0.f
  }
};

// Static functions ----------------------------------------------------------

// Tests ---------------------------------------------------------------------

TEST_GROUP(face_detector);

TEST_SETUP(face_detector)
{
  mock_stage_create(STAGE_MOCK_SIZE);
  mock_stage_expect_create_stage_from_source(stages, STAGES_AMOUNT);

  face_detector_create(
    NULL,
    STAGES_AMOUNT,
    (face_detector_stage_handler) {
      .calculate_prediction = mock_stage_calculate_prediction,
      .create_stage_from_source = mock_stage_create_stage_from_source
    }
  );
}

TEST_TEAR_DOWN(face_detector)
{
  face_detector_destroy();
  mock_stage_verify_complete();
  mock_stage_destroy();
}

TEST(face_detector, create_is_ok)
{
  // Test code in TEST_SETUP
}

TEST(face_detector, detect_with_one_window_is_ok)
{
  face_detector_arguments arguments = (face_detector_arguments) {
    .base_scale = BASE_SCALE,
    .scale_increment = SCALE_INCREMENT,
    .position_increment = POSITION_INCREMENT,
    .image_size_x = FACE_DETECTOR_FEATURE_SIZE,
    .image_size_y = FACE_DETECTOR_FEATURE_SIZE,
    .min_neighbours = MIN_NEIGHBOURS
  };
  bool prediction_values[STAGES_AMOUNT] = { true, true, true };
  area expected_result = (area) {
    .x = 0,
    .y = 0,
    .size = FACE_DETECTOR_FEATURE_SIZE
  };

  for (uint8_t i = 0; i < STAGES_AMOUNT; i++)
    mock_stage_expect_calculate_prediction(&prediction_values[i]);

  face_detector_detect(&arguments);

  face_detector_result result = face_detector_get_result();
  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_result,
    (uint8_t*)result.faces,
    sizeof(area)
  );
}

TEST(face_detector, detect_with_three_windows_is_ok)
{
  uint8_t two_steps = 2.f * FACE_DETECTOR_FEATURE_SIZE * POSITION_INCREMENT;

  face_detector_arguments arguments = (face_detector_arguments) {
    .base_scale = BASE_SCALE,
    .scale_increment = SCALE_INCREMENT,
    .position_increment = POSITION_INCREMENT,
    .image_size_x = FACE_DETECTOR_FEATURE_SIZE + two_steps,
    .image_size_y = FACE_DETECTOR_FEATURE_SIZE,
    .min_neighbours = MIN_NEIGHBOURS
  };
  bool prediction_values[3U * STAGES_AMOUNT] = {
    true, true, true, true, true, true, true, true, true
  };
  area expected_result = (area) {
    // Average between three positions
    .x = (3.f * FACE_DETECTOR_FEATURE_SIZE * POSITION_INCREMENT) / 3.f,
    .y = 0,
    .size = FACE_DETECTOR_FEATURE_SIZE
  };

  for (uint8_t i = 0; i < 3U * STAGES_AMOUNT; i++)
    mock_stage_expect_calculate_prediction(&prediction_values[i]);

  face_detector_detect(&arguments);

  face_detector_result result = face_detector_get_result();
  TEST_ASSERT_EQUAL_UINT8_ARRAY(
    (uint8_t*)&expected_result,
    (uint8_t*)result.faces,
    sizeof(area)
  );
}