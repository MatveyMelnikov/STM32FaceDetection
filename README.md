# STM32FaceDetection
### Launch
* ```make -f MakefileMain.mk``` - building a production version of the code (does not contain tests);
* ```make TESTS=Y FOR_TARGET=Y -f MakefileMain.mk``` - building a test version for target device;
* ```make TESTS=Y FOR_TARGET=N -f MakefileMain.mk start``` - building test version for host device and running it.

### Hardware
The video stream comes from the OV7670 camera without a FIFO buffer. The output is made on a TFT display with an ILI9341 controller.
