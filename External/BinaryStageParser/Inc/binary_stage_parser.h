#ifndef BINARY_STAGE_PARSER_H
#define BINARY_STAGE_PARSER_H

#include <stdint.h>
#include "stage_defs.h"
#include "lbp_feature.h"

stage *const binary_stage_parser_create(
  uint8_t *const source,
  uint8_t stages_amount
);

#endif
