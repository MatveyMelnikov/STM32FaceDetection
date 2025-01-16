#ifndef MOCK_H
#define MOCK_H

#include "mock_defs.h"

void mock_create(const char *const mock_name, const uint8_t max_expectations);
void mock_destroy(void);
void mock_record_expectation(
  const int kind,
  const uint8_t *const data,
  const uint8_t data_size
);
expectation mock_get_expectation(void);
void mock_check_kind(
  const expectation *const current_expectation,
  int kind
);
void mock_verify_complete(void);

#endif
