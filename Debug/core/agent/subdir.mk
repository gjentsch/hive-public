################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/agent/agent.cpp \
../core/agent/special_agent.cpp 

OBJS += \
./core/agent/agent.o \
./core/agent/special_agent.o 

CPP_DEPS += \
./core/agent/agent.d \
./core/agent/special_agent.d 


# Each subdirectory must supply rules for building sources it contributes
core/agent/%.o: ../core/agent/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


