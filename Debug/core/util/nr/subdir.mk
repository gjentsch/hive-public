################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/util/nr/ludcmp.cpp \
../core/util/nr/ran2.cpp 

OBJS += \
./core/util/nr/ludcmp.o \
./core/util/nr/ran2.o 

CPP_DEPS += \
./core/util/nr/ludcmp.d \
./core/util/nr/ran2.d 


# Each subdirectory must supply rules for building sources it contributes
core/util/nr/%.o: ../core/util/nr/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


