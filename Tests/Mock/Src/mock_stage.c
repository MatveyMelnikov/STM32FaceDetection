#include "mock_stage.h"
#include "mock.h"
#include "unity_fixture.h"

// Static variables ----------------------------------------------------------

static const char *name = "Stage";

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_stage_create(const uint8_t max_expectations)
{
  mock_create(name, max_expectations);
}

void mock_stage_destroy()
{
  mock_destroy();
}

void mock_stage_verify_complete()
{
  mock_verify_complete();
}

void mock_stage_expect_create_stage_from_source(
  const stage *const stages,
  const uint8_t stages_amount
)
{
  mock_record_expectation(CREATE, (uint8_t*)stages, stages_amount);
}

void mock_stage_expect_calculate_prediction(
  const bool *const value
)
{
  mock_record_expectation(CALCULATE, (uint8_t*)value, sizeof(bool));
}

stage *const mock_stage_create_stage_from_source(
  uint8_t *const source,
  uint8_t stages_amount
)
{
  UNUSED(source);
  UNUSED(stages_amount);

  expectation current_expectation = mock_get_expectation();
  mock_check_kind(&current_expectation, CREATE);

  return (stage*)current_expectation.data;
}

bool mock_stage_calculate_prediction(
  stage *const self,
  const lbp_feature_arguments *const arguments
)
{
  UNUSED(self);
  UNUSED(arguments);

  expectation current_expectation = mock_get_expectation();
  mock_check_kind(&current_expectation, CALCULATE);

  return *(bool*)current_expectation.data;
}
