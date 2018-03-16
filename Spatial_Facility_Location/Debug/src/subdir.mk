################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DisplayImage.cpp \
../src/bellford.cpp \
../src/dijkstra.cpp \
../src/distance.cpp \
../src/exploracion.cpp \
../src/kmeans.cpp \
../src/main.cpp \
../src/morton.cpp \
../src/tree.cpp 

OBJS += \
./src/DisplayImage.o \
./src/bellford.o \
./src/dijkstra.o \
./src/distance.o \
./src/exploracion.o \
./src/kmeans.o \
./src/main.o \
./src/morton.o \
./src/tree.o 

CPP_DEPS += \
./src/DisplayImage.d \
./src/bellford.d \
./src/dijkstra.d \
./src/distance.d \
./src/exploracion.d \
./src/kmeans.d \
./src/main.d \
./src/morton.d \
./src/tree.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I/usr/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


