################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../core/agent/database/database.cpp 

OBJS += \
./core/agent/database/database.o 

CPP_DEPS += \
./core/agent/database/database.d 


# Each subdirectory must supply rules for building sources it contributes
core/agent/database/%.o: ../core/agent/database/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/local/include/ -IC:/cygwin64/lib/gcc/x86_64-pc-cygwin/4.8.1/include/c++ -IC:/cygwin64/lib/gcc/x86_64-pc-cygwin/4.8.1/include/c++/x86_64-pc-cygwin -IC:/cygwin64/lib/gcc/x86_64-pc-cygwin/4.8.1/include/c++/backward -IC:/cygwin64/lib/gcc/x86_64-pc-cygwin/4.8.1/include/ -IC:/cygwin64/usr/local/include -IC:/cygwin64/usr/include -IC:/cygwin64/usr/include/w32api -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


