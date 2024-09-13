#ifndef MOCK_ADC_DATA_HANDLER_H
#define MOCK_ADC_DATA_HANDLER_H

#include <stdint.h>

void mock_adc_data_handler_create(const uint8_t max_expectations);
void mock_adc_data_handler_destroy(void);
void mock_adc_data_handler_expect_read_then_return(
  const uint32_t *const data
);
void mock_adc_data_handler_verify_complete(void);

uint32_t mock_adc_data_handler_get_data(void);

#endif
