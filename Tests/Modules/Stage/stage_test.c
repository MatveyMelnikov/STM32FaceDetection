#include "unity_fixture.h"
#include "stage.h"
#include "mock_lbp_feature.h"
#include <stddef.h>

// Defines -------------------------------------------------------------------

enum {
  MOCK_LBP_FEATURE_SIZE = 10U,
  STAGE_FEATURES_AMOUNT = 3U,
  SCALES_AMOUNT = 6U,
};

// Static variables ----------------------------------------------------------

static float scales[SCALES_AMOUNT] = {
  1.0f, 1.2f, 1.44f, 1.728f, 2.0736f, 2.48832f 
};

static stage test_stage = { 0 };

// Static functions ----------------------------------------------------------

// Tests ---------------------------------------------------------------------

TEST_GROUP(stage_test);

TEST_SETUP(stage_test)
{
  mock_lbp_feature_create(MOCK_LBP_FEATURE_SIZE);
  stage_set_feature_handler(
    (stage_feature_handler) {
      .calculate_vote = mock_lbp_feature_calculate_vote,
      .destroy = mock_lbp_feature_destroy_feature
    }
  );

  lbp_feature *new_features = calloc(
    STAGE_FEATURES_AMOUNT,
    sizeof(lbp_feature)
  );
  test_stage = (stage) {
    .features = new_features,
    .features_amount = STAGE_FEATURES_AMOUNT,
    .threshold = 5.f
  };
}

TEST_TEAR_DOWN(stage_test)
{
  free(test_stage.features);

  stage_reset_feature_handler();

  mock_lbp_feature_verify_complete();
  mock_lbp_feature_destroy();
}

TEST(stage_test, destroy_features_is_ok)
{
  mock_lbp_feature_expect_destroy_feature();
  mock_lbp_feature_expect_destroy_feature();
  mock_lbp_feature_expect_destroy_feature();

  stage_destroy(&test_stage);
}

TEST(stage_test, calculate_vote_is_true)
{
  float lbp_votes[3] = { 1.f, 2.f, 3.f };

  mock_lbp_feature_expect_calculate_vote(&lbp_votes[0]);
  mock_lbp_feature_expect_calculate_vote(&lbp_votes[1]);
  mock_lbp_feature_expect_calculate_vote(&lbp_votes[2]);

  lbp_feature_arguments arguments = (lbp_feature_arguments) {
    .offset_x = 0U,
    .offset_y = 0U,
    .scale = 1.f
  };
  bool result = stage_calculate_prediction(&test_stage, &arguments);

  TEST_ASSERT_EQUAL(true, result);
}

TEST(stage_test, calculate_vote_is_false)
{
  float lbp_votes[3] = { 0.5f, 1.f, 2.5f };

  mock_lbp_feature_expect_calculate_vote(&lbp_votes[0]);
  mock_lbp_feature_expect_calculate_vote(&lbp_votes[1]);
  mock_lbp_feature_expect_calculate_vote(&lbp_votes[2]);

  lbp_feature_arguments arguments = (lbp_feature_arguments) {
    .offset_x = 0U,
    .offset_y = 0U,
    .scale = 1.f
  };
  bool result = stage_calculate_prediction(&test_stage, &arguments);

  TEST_ASSERT_EQUAL(false, result);
}

