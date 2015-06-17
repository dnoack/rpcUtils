################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ComPoint.cpp \
../src/ComPointB.cpp \
../src/JsonRPC.cpp \
../src/LogUnit.cpp \
../src/PluginInterface.cpp \
../src/RPCMsg.cpp \
../src/RegClient.cpp 

OBJS += \
./src/ComPoint.o \
./src/ComPointB.o \
./src/JsonRPC.o \
./src/LogUnit.o \
./src/PluginInterface.o \
./src/RPCMsg.o \
./src/RegClient.o 

CPP_DEPS += \
./src/ComPoint.d \
./src/ComPointB.d \
./src/JsonRPC.d \
./src/LogUnit.d \
./src/PluginInterface.d \
./src/RPCMsg.d \
./src/RegClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/dave2/git/rpcUtils/include" -I/home/dave2/git/rapidjson/include/rapidjson -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


