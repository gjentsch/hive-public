################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/communicator/parallel/reagan.cpp 

OBJS += \
./core/communicator/parallel/reagan.o 

CPP_DEPS += \
./core/communicator/parallel/reagan.d 


# Each subdirectory must supply rules for building sources it contributes
core/communicator/parallel/%.o: ../core/communicator/parallel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


