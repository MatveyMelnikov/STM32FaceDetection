#ifndef STAGE_H
#define STAGE_H

#include "stage_defs.h"
#include "lbp_feature_defs.h"
#include <stdbool.h>

void stage_set_feature_handler(stage_feature_handler handler);
void stage_reset_feature_handler(void);
void stage_calculate_scaled_features(
  stage *const self,
  const float *const scales,
  const uint8_t scales_amount
);
void stage_destroy(stage *const self);
bool stage_calculate_prediction(
  stage *const self,
  const lbp_feature_arguments *const arguments
);

#endif
