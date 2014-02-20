################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../projects/chemotaxis_population/ChemotaxisProject.cpp \
../projects/chemotaxis_population/chemopopcomposer.cpp 

OBJS += \
./projects/chemotaxis_population/ChemotaxisProject.o \
./projects/chemotaxis_population/chemopopcomposer.o 

CPP_DEPS += \
./projects/chemotaxis_population/ChemotaxisProject.d \
./projects/chemotaxis_population/chemopopcomposer.d 


# Each subdirectory must supply rules for building sources it contributes
projects/chemotaxis_population/%.o: ../projects/chemotaxis_population/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


