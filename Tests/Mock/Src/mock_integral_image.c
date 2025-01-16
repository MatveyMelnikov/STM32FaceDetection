#include "mock_integral_image.h"
#include "mock.h"
#include "unity_fixture.h"

// Static variables ----------------------------------------------------------

static const char *name = "Integral image";

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_integral_image_create(const uint8_t max_expectations)
{
  mock_create(name, max_expectations);
}

void mock_integral_image_destroy()
{
  mock_destroy();
}

void mock_integral_image_expect_read_then_return(
  const uint16_t *const data
)
{
  mock_record_expectation(READ, (uint8_t*)data, sizeof(uint16_t));
}

void mock_integral_image_verify_complete()
{
  mock_verify_complete();
}

uint16_t mock_integral_image_get_rectangle(
	integral_image_rectangle_position *const rectangle
)
{
  (void)rectangle;

  expectation current_expectation = mock_get_expectation();
  mock_check_kind(&current_expectation, READ);

  return *(uint16_t*)current_expectation.data;
}
