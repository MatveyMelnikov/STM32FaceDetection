#include "face_detector_builder.h"
#include "face_detector.h"
#include "stage.h"
#include "binary_stage_parser.h"
#include "lbp_feature.h"
#include "integral_image.h"

void face_detector_builder_create(
  uint8_t *const binary_classifiers_source,
  const uint8_t stages_amount,
  uint16_t (*get_rectangle_summarize)(
    const lbp_feature_rectangle *const feature_rectangle
  )
)
{
  lbp_feature_set_integral_image_handler(
    (lbp_feature_integral_image_handler) {
      .get_rectangle_summarize = get_rectangle_summarize
    }
  );

  stage_set_feature_handler(
    (stage_feature_handler) {
      .calculate_vote = lbp_feature_calculate_vote,
      .destroy = lbp_feature_destroy,
      .generate_scaled_rectangles = lbp_feature_generate_scaled_rectangles
    }
  );

  face_detector_create(
    binary_classifiers_source,
    stages_amount,
    (face_detector_stage_handler) {
      .calculate_prediction = stage_calculate_prediction,
      .calculate_scaled_features = stage_calculate_scaled_features,
      .create_stage_from_source = binary_stage_parser_create
    }
  );
}