#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include "face_detector_defs.h"
#include "area.h"

void face_detector_create(
  uint8_t *const binary_classifiers_source,
  const uint8_t stages_amount,
  face_detector_stage_handler handler
);
void face_detector_destroy(void);
void face_detector_detect(
  const face_detector_arguments *const arguments
);
face_detector_result face_detector_get_result(void);

#endif
