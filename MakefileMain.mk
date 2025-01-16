MODULES_DIR = External

ILI9341_TFT_DRIVER_DIR = $(MODULES_DIR)/ILI9341_TFT_Driver
OV7670_DIR = $(MODULES_DIR)/OV7670
INTEGRAL_IMAGE_DIR = $(MODULES_DIR)/IntegralImage
LBP_FEATURE_DIR = $(MODULES_DIR)/LBPFeature
STAGE_DIR = $(MODULES_DIR)/Stage
BINARY_STAGE_PARSER_DIR = $(MODULES_DIR)/BinaryStageParser
FACE_DETECTOR_DIR = $(MODULES_DIR)/FaceDetector
FACE_DETECTOR_BUILDER_DIR = $(MODULES_DIR)/FaceDetectorBuilder
AREA_DIR = $(MODULES_DIR)/Area
SIMD_INSTRUCTIONS = $(MODULES_DIR)/SIMD_Instructions

TESTS = N

C_INCLUDES = \
-I$(INTEGRAL_IMAGE_DIR)/Inc \
-I$(LBP_FEATURE_DIR)/Inc \
-I$(STAGE_DIR)/Inc \
-I$(BINARY_STAGE_PARSER_DIR)/Inc \
-I$(FACE_DETECTOR_DIR)/Inc \
-I$(FACE_DETECTOR_BUILDER_DIR)/Inc \
-I$(AREA_DIR)/Inc

C_SOURCES = \
$(INTEGRAL_IMAGE_DIR)/Src/integral_image.c \
$(LBP_FEATURE_DIR)/Src/lbp_feature.c \
$(STAGE_DIR)/Src/stage.c \
$(BINARY_STAGE_PARSER_DIR)/Src/binary_stage_parser.c \
$(FACE_DETECTOR_DIR)/Src/face_detector.c \
$(FACE_DETECTOR_BUILDER_DIR)/Src/face_detector_builder.c

ifeq ($(TESTS), Y)
include MakefileTest.mk
else

include MakefileTarget.mk                                       
endif