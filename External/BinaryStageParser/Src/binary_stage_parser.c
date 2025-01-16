#include "binary_stage_parser.h"
#include "binary_stage_parser_defs.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

// Defines -------------------------------------------------------------------

#define GET_ABSOLUTE_ADDRESS(base_obj_ptr, obj_ptr) \
	((ADDR_TYPE)(obj_ptr) + (ADDR_TYPE)(base_obj_ptr))

// Implementations -----------------------------------------------------------

stage *const binary_stage_parser_create(
  uint8_t *const source,
  uint8_t stages_amount
)
{
	for (uint8_t i = 0; i < stages_amount; i++)
	{
    stage* current_stage = (stage*)source + i;

		current_stage->features = (lbp_feature*)GET_ABSOLUTE_ADDRESS(
			source,
			current_stage->features
		);
	}

	return (stage*)source;
}
