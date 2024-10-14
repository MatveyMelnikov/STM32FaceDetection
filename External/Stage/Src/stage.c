#include "stage.h"

// Static variables ----------------------------------------------------------

static stage_feature_handler feature_handler;

// Implementations -----------------------------------------------------------

void stage_set_feature_handler(stage_feature_handler handler)
{
  feature_handler = handler;
}

void stage_reset_feature_handler()
{
  feature_handler = (stage_feature_handler){ 0 };
}

void stage_destroy(stage *const self)
{
  for (uint8_t i = 0; i < self->features_amount; i++)
  {
    feature_handler.destroy(&self->features[i]);
  }
}

bool stage_calculate_prediction(
  stage *const self,
  const lbp_feature_arguments *const arguments
)
{
  float score = 0.f;

  for (uint8_t i = 0; i < self->features_amount; i++)
  {
    score += feature_handler.calculate_vote(&self->features[i], arguments);
  }

  return (score >= (self->threshold - STAGE_EPSILON)) ? true : false;
}
