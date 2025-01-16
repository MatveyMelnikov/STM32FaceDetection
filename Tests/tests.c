#include "unity_fixture.h"
#include "unity_config.h"
#include <stdio.h>

void unity_config_put_c(uint8_t c);

static void run_all_tests()
{
	RUN_TEST_GROUP(integral_image);
  RUN_TEST_GROUP(binary_stage_parser);
  RUN_TEST_GROUP(lbp_feature_test);
  RUN_TEST_GROUP(stage_test);
  RUN_TEST_GROUP(face_detector);
}

int main(int argc, char *argv[])
{
	return UnityMain(argc, (const char**)argv, run_all_tests);
}

void unity_config_put_c(uint8_t c)
{
	(void)putchar(c);
}
