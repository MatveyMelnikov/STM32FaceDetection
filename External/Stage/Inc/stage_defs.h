#ifndef STAGE_DEFS_H
#define STAGE_DEFS_H

#include <stdint.h>
#include "lbp_feature_defs.h"

enum {
  STAGE_EPSILON = 1
};

typedef struct
{
	lbp_feature *features;
	uint8_t features_amount;
	int16_t threshold;
} stage;

typedef struct
{
  // self, scales, scales_amount
  void (*generate_scaled_rectangles)(
    lbp_feature *const,
    const float *const,
    const uint8_t
  );
  // self
  void (*destroy)(lbp_feature *const);
  // self, arguments
  float (*calculate_vote)(
    lbp_feature *const,
    const lbp_feature_arguments *const
  );
} stage_feature_handler;

#endif
