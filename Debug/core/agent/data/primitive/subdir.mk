################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/agent/data/primitive/primitiveData.cpp 

OBJS += \
./core/agent/data/primitive/primitiveData.o 

CPP_DEPS += \
./core/agent/data/primitive/primitiveData.d 


# Each subdirectory must supply rules for building sources it contributes
core/agent/data/primitive/%.o: ../core/agent/data/primitive/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


