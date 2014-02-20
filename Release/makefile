################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include projects/chemotaxis_population/chemopopSimulators/cellSimulators/subdir.mk
-include projects/chemotaxis_population/chemopopSimulators/cellMovement/chemotaxisMovement/subdir.mk
-include projects/chemotaxis_population/chemopopMessageGenerator/subdir.mk
-include projects/chemotaxis_population/chemopopInitialiser/subdir.mk
-include projects/chemotaxis_population/chemopopFactories/subdir.mk
-include projects/chemotaxis_population/chemopopAction/subdir.mk
-include projects/chemotaxis_population/subdir.mk
-include core/util/xml/TinyXML/subdir.mk
-include core/util/string/subdir.mk
-include core/util/rand/MTrand/subdir.mk
-include core/util/rand/subdir.mk
-include core/util/nr/subdir.mk
-include core/util/matrix/subdir.mk
-include core/util/function/muParser/subdir.mk
-include core/util/conversion/subdir.mk
-include core/simulators/timekeeper/subdir.mk
-include core/registrar/subdir.mk
-include core/parallel/subdir.mk
-include core/input/systemParser/subdir.mk
-include core/input/commandLineParser/subdir.mk
-include core/exception/subdir.mk
-include core/communicator/serial/subdir.mk
-include core/communicator/parallel/subdir.mk
-include core/agent/message/subdir.mk
-include core/agent/database/subdir.mk
-include core/agent/data/primitive/subdir.mk
-include core/agent/data/mapdata/subdir.mk
-include core/agent/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: chemotaxis

# Tool invocations
chemotaxis: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	g++  -o "chemotaxis" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) chemotaxis
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
