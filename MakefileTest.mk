CFLAGS = -DTEST -DUNITY_INCLUDE_CONFIG_H
TESTS_DIR = Tests
UNITY_DIR = External/Unity-2.5.2

FOR_TARGET = Y

C_INCLUDES += \
-I$(UNITY_DIR)/src \
-I$(UNITY_DIR)/extras/fixture/src \
-I$(UNITY_DIR)/extras/memory/src \
-I$(TESTS_DIR) \
-I$(TESTS_DIR)/Mocks/Inc \
-I$(TESTS_DIR)/Fakes/Inc

C_SOURCES += \
$(UNITY_DIR)/src/unity.c \
$(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
$(UNITY_DIR)/extras/memory/src/unity_memory.c

ifeq ($(FOR_TARGET), Y)
C_SOURCES += \
$(TESTS_DIR)/target_tests.c \
$(TESTS_DIR)/Target_Tests/Test_Module/test_module_test_runner.c \
$(TESTS_DIR)/Target_Tests/Test_Module/test_module_test.c \
$(TESTS_DIR)/Mocks/Src/mock_adc_data_handler.c

include MakefileTestTarget.mk
else
C_SOURCES += \
$(TESTS_DIR)/host_tests.c \
$(TESTS_DIR)/Host_Tests/Test_Module/test_module_test_runner.c \
$(TESTS_DIR)/Host_Tests/Test_Module/test_module_test.c \
$(TESTS_DIR)/Mocks/Src/mock_adc_data_handler.c

include MakefileTestHost.mk
endif
