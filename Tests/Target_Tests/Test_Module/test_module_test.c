#include "unity_fixture.h"
#include "test_module.h"
#include "mock_adc_data_handler.h"
#include <string.h>
#include <stdint.h>

// Defines -------------------------------------------------------------------

enum {
  ADC_MOCK_SIZE = 10U
};

// Static functions ----------------------------------------------------------

// Tests ---------------------------------------------------------------------

TEST_GROUP(test_module);

TEST_SETUP(test_module)
{
  mock_adc_data_handler_create(ADC_MOCK_SIZE);

  test_module_set_data_handler(
    (test_module_data_handler) {
      .get_data = mock_adc_data_handler_get_data
    }
  );
}

TEST_TEAR_DOWN(test_module)
{
  mock_adc_data_handler_verify_complete();
  mock_adc_data_handler_destroy();
  test_module_destroy();
}

TEST(test_module, multiply_is_ok)
{
  uint32_t expect_data[] = { 10U, 20U };

  mock_adc_data_handler_expect_read_then_return((uint32_t*)expect_data);
  mock_adc_data_handler_expect_read_then_return((uint32_t*)(expect_data + 1));

  TEST_ASSERT_EQUAL(expect_data[0] * 2U, test_module_multiply());
  TEST_ASSERT_EQUAL(expect_data[1] * 2U, test_module_multiply());
}
