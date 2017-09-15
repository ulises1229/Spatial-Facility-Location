################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DisplayImage.cpp \
../src/distance.cpp \
../src/exploracion.cpp \
../src/main.cpp \
../src/tree.cpp 

OBJS += \
./src/DisplayImage.o \
./src/distance.o \
./src/exploracion.o \
./src/main.o \
./src/tree.o 

CPP_DEPS += \
./src/DisplayImage.d \
./src/distance.d \
./src/exploracion.d \
./src/main.d \
./src/tree.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/local/include/opencv -I/usr/include/python2.7/ -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


