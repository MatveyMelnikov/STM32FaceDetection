CFLAGS = \
-DTEST \
-DUNITY_INCLUDE_CONFIG_H

TESTS_DIR = Tests
MOCK_DIR = $(TESTS_DIR)/Mock
FAKES_DIR = $(TESTS_DIR)/Fakes
UNITY_DIR = External/Unity-2.5.2

INTEGRAL_IMAGE_TESTS = $(TESTS_DIR)/Modules/IntegralImage
BINARY_STAGE_PARSER_TESTS = $(TESTS_DIR)/Modules/BinaryStageParser
LBP_FEATURE_TESTS = $(TESTS_DIR)/Modules/LBPFeature
STAGE_TESTS = $(TESTS_DIR)/Modules/Stage
FACE_DETECTOR_TESTS = $(TESTS_DIR)/Modules/FaceDetector

BUILD_DIR = $(UNITY_DIR)/Build
TARGET = $(BUILD_DIR)/tests.out

C_INCLUDES += \
-I$(UNITY_DIR)/src \
-I$(UNITY_DIR)/extras/fixture/src \
-I$(UNITY_DIR)/extras/memory/src \
-I$(TESTS_DIR) \
-I$(MOCK_DIR)/Inc \
-I$(FAKES_DIR)/Inc

C_SOURCES += \
$(UNITY_DIR)/src/unity.c \
$(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
$(UNITY_DIR)/extras/memory/src/unity_memory.c \
$(TESTS_DIR)/tests.c \
$(INTEGRAL_IMAGE_TESTS)/integral_image_test_runner.c \
$(INTEGRAL_IMAGE_TESTS)/integral_image_test.c \
$(BINARY_STAGE_PARSER_TESTS)/binary_stage_parser_test_runner.c \
$(BINARY_STAGE_PARSER_TESTS)/binary_stage_parser_test.c \
$(LBP_FEATURE_TESTS)/lbp_feature_test_runner.c \
$(LBP_FEATURE_TESTS)/lbp_feature_test.c \
$(STAGE_TESTS)/stage_test_runner.c \
$(STAGE_TESTS)/stage_test.c \
$(FACE_DETECTOR_TESTS)/face_detector_test_runner.c \
$(FACE_DETECTOR_TESTS)/face_detector_test.c \
$(MOCK_DIR)/Src/mock_integral_image.c \
$(MOCK_DIR)/Src/mock_lbp_feature.c \
$(MOCK_DIR)/Src/mock_stage.c \
$(MOCK_DIR)/Src/mock.c

include MakefileTestFinal.mk