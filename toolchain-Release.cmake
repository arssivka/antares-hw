# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(/home/vub/linaro/linaro-7.2.1/bin/arm-linux-gnueabihf-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(/home/vub/linaro/linaro-7.2.1/bin/arm-linux-gnueabihf-gcc GNU)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /home/vub/linaro/linaro-7.2.1/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER /home/vub/linaro/linaro-7.2.1/bin/arm-linux-gnueabihf-g++)
SET(CROSSCOMPILE_FLAGS "-mthumb -O3 -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mvectorize-with-neon-quad -mfloat-abi=hard -ffast-math")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CROSSCOMPILE_FLAGS} -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CROSSCOMPILE_FLAGS} -static-libstdc++ -std=c++17")
SET(CMAKE_SYSROOT "/home/vub/smarc-fsl-bsp-test/imx6q1g-build-qt5x11/tmp/sysroots/imx6q1gsmarc")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH /home/vub/linaro/linaro-7.2.1/arm-linux-gnueabihf /home/vub/smarc-fsl-bsp-test/imx6q1g-build-qt5x11/tmp/sysroots/imx6q1gsmarc)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)