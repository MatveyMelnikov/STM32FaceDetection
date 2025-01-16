#include "mock_lbp_feature.h"
#include "mock.h"
#include "unity_fixture.h"

// Static variables ----------------------------------------------------------

static const char *name = "LBP feature";

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_lbp_feature_create(const uint8_t max_expectations)
{
  mock_create(name, max_expectations);
}

void mock_lbp_feature_destroy()
{
  mock_destroy();
}

void mock_lbp_feature_expect_destroy_feature()
{
  mock_record_expectation(DESTROY, NULL, 0U);
}

void mock_lbp_feature_expect_calculate_vote(const float *const data)
{
  mock_record_expectation(CALCULATE, (uint8_t*)data, sizeof(float));
}

void mock_lbp_feature_verify_complete()
{
  mock_verify_complete();
}

void mock_lbp_feature_destroy_feature(lbp_feature *const self)
{
  (void)self;

  expectation current_expectation = mock_get_expectation();
  mock_check_kind(&current_expectation, DESTROY);
}

float mock_lbp_feature_calculate_vote(
  lbp_feature *const self,
  const lbp_feature_arguments *const arguments
)
{
  (void)self;
  (void)arguments;

  expectation current_expectation = mock_get_expectation();
  mock_check_kind(&current_expectation, CALCULATE);

  return *(float*)current_expectation.data;
}
