################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/util/rand/MTrand/mtrand.cpp 

OBJS += \
./core/util/rand/MTrand/mtrand.o 

CPP_DEPS += \
./core/util/rand/MTrand/mtrand.d 


# Each subdirectory must supply rules for building sources it contributes
core/util/rand/MTrand/%.o: ../core/util/rand/MTrand/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


