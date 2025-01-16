#include "unity_fixture.h"

TEST_GROUP_RUNNER(binary_stage_parser)
{
  RUN_TEST_CASE(binary_stage_parser, create_is_ok);
  RUN_TEST_CASE(binary_stage_parser, parse_stages_is_ok);
  RUN_TEST_CASE(binary_stage_parser, parse_features_is_ok);
}