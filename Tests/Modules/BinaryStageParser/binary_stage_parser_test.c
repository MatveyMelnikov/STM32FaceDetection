#include "unity_fixture.h"
#include "binary_stage_parser.h"
#include "binary_stage_parser_defs.h"
#include "lbp_feature.h"
#include "stage.h"
#include <string.h>

// Defines -------------------------------------------------------------------

enum {
  STAGES_AMOUNT = 2U
};

#define GET_ARRAY_LEN(arr) \
  sizeof(arr) / sizeof(arr[0])
#define GET_RELATIVE_ADDRESS(base_obj_ptr, obj_ptr) \
  ((ADDR_TYPE)(obj_ptr) - (ADDR_TYPE)(base_obj_ptr))

// Static variables ----------------------------------------------------------

  // lbp_feature_rectangle rectangles[LBP_FEATURE_RECTANGLES_AMOUNT];
  // // There should just be a pointer - a dynamic array, since we
  // // generate them at runtime and don't know the size
  // lbp_feature_rectangle **scaled_rectangles;
  // uint8_t scaled_rectangles_amount;
  // int32_t masks[LBP_FEATURE_MASKS_AMOUNT];
  // int16_t left_value;
  // int16_t right_value;

static lbp_feature data_features[] = {
  {
    .left_value = -389,
    .right_value = 997,
    .masks = { -1, -1, -17409, -1, -1, -1, -1, -1 },
    .rectangles = {
      (lbp_feature_rectangle) { .x = 4, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 30, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 4, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 30, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 4, .y = 14, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 14, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 30, .y = 14, .width = 13, .height = 7 }
    }
  },
  {
    .left_value = -25,
    .right_value = -986,
    .masks = { -1, -1, -21569, -20545, -1, -1, -20545, -1 },
    .rectangles = {
      (lbp_feature_rectangle) { .x = 2, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 15, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 28, .y = 0, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 2, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 15, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 28, .y = 7, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 2, .y = 14, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 15, .y = 14, .width = 13, .height = 7 },
      (lbp_feature_rectangle) { .x = 28, .y = 14, .width = 13, .height = 7 }
    }
  },
  {
    .left_value = -140,
    .right_value = -964,
    .masks = { -21569, -16449, 1006578219, -20801, -16449, -1, -21585, -1 },
    .rectangles = {
      (lbp_feature_rectangle) { .x = 5, .y = 1, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 1, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 29, .y = 1, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 5, .y = 8, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 8, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 29, .y = 8, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 5, .y = 15, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 17, .y = 15, .width = 12, .height = 7 },
      (lbp_feature_rectangle) { .x = 29, .y = 15, .width = 12, .height = 7 }
    }
  }
};

static stage data_stages[STAGES_AMOUNT] = {
  {
    .features = (lbp_feature*)data_features + 0,
    .features_amount = 2,
    .threshold = -2
  },
  {
    .features = (lbp_feature*)data_features + 2,
    .features_amount = 1,
    .threshold = -4
  }
};

static uint8_t *binary_data = NULL;
static stage *parsed_stages = NULL; 

// Static functions ----------------------------------------------------------

// Tests ---------------------------------------------------------------------

TEST_GROUP(binary_stage_parser);

TEST_SETUP(binary_stage_parser)
{
  // First come the stages, then the features
  uint64_t features_offset = sizeof(data_stages);
  uint64_t result_binary_data_size = sizeof(data_stages) +
    sizeof(data_features);
  binary_data = malloc(result_binary_data_size);

  stage* current_stage_in_data = (stage*)binary_data;
  for (uint8_t i = 0; i < GET_ARRAY_LEN(data_stages); i++)
  {
    current_stage_in_data[i] = (stage) {
      .threshold = data_stages[i].threshold,
      .features_amount = data_stages[i].features_amount,
      .features = (lbp_feature*)(
        GET_RELATIVE_ADDRESS(
          data_features,
          data_stages[i].features
        ) + features_offset
      ),
    };
  }

  memcpy(
    binary_data + features_offset,
    (uint8_t*)&data_features,
    sizeof(data_features)
  );

  parsed_stages = (stage*)binary_stage_parser_create(
    binary_data,
    STAGES_AMOUNT
  );
}

TEST_TEAR_DOWN(binary_stage_parser)
{
  free(binary_data);
  binary_data = NULL;
  parsed_stages = NULL;
}

TEST(binary_stage_parser, create_is_ok)
{
  TEST_ASSERT_NOT_EQUAL(NULL, parsed_stages);
}

TEST(binary_stage_parser, parse_stages_is_ok)
{
  for (uint8_t stage_index = 0; stage_index < STAGES_AMOUNT; stage_index++)
  {
    TEST_ASSERT_EQUAL(
      data_stages[stage_index].threshold,
      parsed_stages[stage_index].threshold
    );
    TEST_ASSERT_EQUAL(
      data_stages[stage_index].features_amount,
      parsed_stages[stage_index].features_amount
    );
  }
}

TEST(binary_stage_parser, parse_features_is_ok)
{
  for (uint8_t stage_index = 0; stage_index < STAGES_AMOUNT; stage_index++)
  {
    lbp_feature *data_features = data_stages[stage_index].features;
    lbp_feature *parsed_features = parsed_stages[stage_index].features;

    for (
      uint8_t feature_index = 0;
      feature_index < data_stages[stage_index].features_amount;
      feature_index++
    )
    {
      TEST_ASSERT_EQUAL(
        data_features[feature_index].left_value,
        parsed_features[feature_index].left_value
      );
      TEST_ASSERT_EQUAL(
        data_features[feature_index].right_value,
        parsed_features[feature_index].right_value
      );

      TEST_ASSERT_EQUAL(
        0,
        memcmp(
          data_features[feature_index].masks,
          parsed_features[feature_index].masks,
          sizeof(data_features[feature_index].masks)
        )
      );

      TEST_ASSERT_EQUAL(
        0,
        memcmp(
          data_features[feature_index].rectangles,
          parsed_features[feature_index].rectangles,
          sizeof(data_features[feature_index].rectangles)
        )
      );
    }
  }
}
