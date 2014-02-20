################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/input/commandLineParser/commandLineParser.cpp 

OBJS += \
./core/input/commandLineParser/commandLineParser.o 

CPP_DEPS += \
./core/input/commandLineParser/commandLineParser.d 


# Each subdirectory must supply rules for building sources it contributes
core/input/commandLineParser/%.o: ../core/input/commandLineParser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


