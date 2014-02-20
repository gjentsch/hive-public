################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/util/conversion/conversion.cpp 

OBJS += \
./core/util/conversion/conversion.o 

CPP_DEPS += \
./core/util/conversion/conversion.d 


# Each subdirectory must supply rules for building sources it contributes
core/util/conversion/%.o: ../core/util/conversion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


