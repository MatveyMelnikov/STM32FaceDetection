#include "mock.h"
#include "unity_fixture.h"
#include <stddef.h>
#include <stdio.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 30U
};

// Static variables ----------------------------------------------------------

static char *report_not_init = "%s mock not initialized";
static char *report_no_room_for_expectation = "No room for expectations in "
  "%s mock";
static char *report_verify_error = "Verify error in %s mock. Expected %u "
  "operations, but got %u";
static char *report_kind_error = "Error kind in %s mock. "
  "Num of expectation %u ";
static char *report_index_error = "Index error in %s mock. Index: %u";

static const char *current_mock_name;
static expectation *expectations = NULL;
static uint8_t set_expectation_count = 0;
static uint8_t get_expectation_count = 0;
static uint8_t max_expectation_count;

// Static prototypes ---------------------------------------------------------

static void mock_reset(void);
static void mock_fail_when_no_room_for_expectations(void);
static void mock_fail_when_no_init(void);
static void mock_fail_with_report_not_init(void);
static void mock_fail_with_report_no_room_for_expectation(void);
static void mock_fail_with_report_verify_error(void);
static void mock_fail_with_report_kind_error(void);
static void mock_fail_with_report_index_error(const uint8_t index);

// Implementations -----------------------------------------------------------

void mock_create(const char *const mock_name, const uint8_t max_expectations)
{
  if (expectations != NULL)
    free(expectations);
  expectations = calloc(max_expectations, sizeof(expectation));
  mock_reset();
  max_expectation_count = max_expectations;
  current_mock_name = mock_name;
}

void mock_destroy()
{
  if (expectations)
    free(expectations);
  expectations = NULL;

  mock_reset();
  max_expectation_count = 0;
  current_mock_name = NULL;
}

static void mock_reset()
{
  set_expectation_count = 0;
  get_expectation_count = 0;
}

void mock_record_expectation(
  const int kind,
  const uint8_t *const data,
  const uint8_t data_size
)
{
  mock_fail_when_no_room_for_expectations();

  expectations[set_expectation_count].kind = kind;
  expectations[set_expectation_count].data = data;
  expectations[set_expectation_count].data_size = data_size;
  set_expectation_count++;
}

expectation mock_get_expectation()
{
  mock_fail_when_no_init();

  if (get_expectation_count > set_expectation_count)
    mock_fail_with_report_index_error(get_expectation_count);

  expectation current_exception = expectations[get_expectation_count++];

  return current_exception;
}

void mock_check_kind(
  const expectation *const current_expectation,
  int kind
)
{
  if (current_expectation->kind == kind)
    return;

  mock_fail_with_report_kind_error();
}

void mock_verify_complete()
{
  if (set_expectation_count == get_expectation_count)
    return;

  mock_fail_with_report_verify_error();
}

static void mock_fail_when_no_room_for_expectations()
{
  mock_fail_when_no_init();
  if (set_expectation_count >= max_expectation_count)
    mock_fail_with_report_no_room_for_expectation();
}

static void mock_fail_when_no_init()
{
  if (expectations == NULL)
    mock_fail_with_report_not_init();
}

static void mock_fail_with_report_not_init()
{
  char *message[sizeof(report_not_init) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_not_init,
    current_mock_name
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_no_room_for_expectation()
{
  char *message[
    sizeof(report_no_room_for_expectation) + OUTPUT_CHARS_ADDITIVE
  ];
  
  sprintf(
    (char*)message,
    report_no_room_for_expectation,
    current_mock_name
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_verify_error()
{
  char *message[sizeof(report_verify_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_verify_error,
    current_mock_name,
    set_expectation_count,
    get_expectation_count
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_kind_error()
{
  char *message[sizeof(report_kind_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_kind_error,
    current_mock_name,
    get_expectation_count
  );
  FAIL((char*)message);
}

static void mock_fail_with_report_index_error(const uint8_t index)
{
  char *message[sizeof(report_index_error) + OUTPUT_CHARS_ADDITIVE];

  sprintf(
    (char*)message,
    report_index_error,
    current_mock_name,
    index
  );
  FAIL((char*)message);
}
