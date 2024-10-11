#ifndef FACE_DETECTOR_BUILDER_H
#define FACE_DETECTOR_BUILDER_H

#include <stdint.h>
#include "lbp_feature_defs.h"

void face_detector_builder_create(
  uint8_t *const binary_classifiers_source,
  const uint8_t stages_amount,
  uint16_t (*get_rectangle_summarize)(
    const lbp_feature_rectangle *const feature_rectangle
  )
);

#endif
