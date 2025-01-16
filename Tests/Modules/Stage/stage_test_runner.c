#include "unity_fixture.h"

TEST_GROUP_RUNNER(stage_test)
{
  RUN_TEST_CASE(stage_test, destroy_features_is_ok);
  RUN_TEST_CASE(stage_test, calculate_vote_is_true);
}