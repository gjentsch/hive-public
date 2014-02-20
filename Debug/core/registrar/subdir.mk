################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/registrar/registrar.cpp 

OBJS += \
./core/registrar/registrar.o 

CPP_DEPS += \
./core/registrar/registrar.d 


# Each subdirectory must supply rules for building sources it contributes
core/registrar/%.o: ../core/registrar/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


