#ifndef LBP_FEATURE_H
#define LBP_FEATURE_H

#include "lbp_feature_defs.h"

void lbp_feature_set_integral_image_handler(
  lbp_feature_integral_image_handler handler
);
void lbp_feature_reset_integral_image_handler(void);
float lbp_feature_calculate_vote(
  lbp_feature *const self,
  const lbp_feature_arguments *const arguments
);
void lbp_feature_destroy(lbp_feature *const self);

#endif
