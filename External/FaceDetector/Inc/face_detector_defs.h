#ifndef FACE_DETECTOR_DEFS_H
#define FACE_DETECTOR_DEFS_H

#include <stdint.h>
#include <stdbool.h>
#include "area.h"
#include "stage_defs.h"

enum {
  FACE_DETECTOR_SCALES_ARRAY_SIZE = 40U,
  FACE_DETECTOR_FEATURE_SIZE = 45U, // default - 24, improved - 45
  FACE_DETECTOR_MAX_FACE_AREAS = 40U,
  FACE_DETECTOR_MAX_WINDOW_SIZE = 96U
};

typedef struct
{
  const uint8_t image_size_x;
  const uint8_t image_size_y;
  float base_scale;
  float scale_increment;
  float position_increment;
  uint8_t min_neighbours;
} face_detector_arguments;

typedef struct
{
  const area *const faces;
  const uint8_t faces_amount;
} face_detector_result;

typedef struct
{
  // source, stages_amount
  stage *const (*create_stage_from_source)(uint8_t *const, uint8_t);
  // self, scales, scales_amount
  void (*calculate_scaled_features)(
    stage *const,
    const float *const,
    const uint8_t
  );
  // self, arguments
  bool (*calculate_prediction)(
    stage *const,
    const lbp_feature_arguments *const
  );
} face_detector_stage_handler;

#endif
