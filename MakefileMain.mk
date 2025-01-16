MODULES_DIR = External
ILI9341_TFT_DRIVER_DIR = $(MODULES_DIR)/ili9341_tft_driver
OV7670_DIR = $(MODULES_DIR)/ov7670

TESTS = N

C_INCLUDES = \
-I$(ILI9341_TFT_DRIVER_DIR)/Inc \
-I$(OV7670_DIR)/Inc

C_SOURCES = \
$(ILI9341_TFT_DRIVER_DIR)/Src/ili9341_tft_driver.c \
$(OV7670_DIR)/Src/ov7670.c

ifeq ($(TESTS), Y)
include MakefileTest.mk
else
include MakefileTarget.mk                                       
endif