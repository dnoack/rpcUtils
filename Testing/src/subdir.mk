################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ComPoint.cpp \
../src/ComPointB.cpp \
../src/JsonRPC.cpp \
../src/RPCMsg.cpp 

OBJS += \
./src/ComPoint.o \
./src/ComPointB.o \
./src/JsonRPC.o \
./src/RPCMsg.o 

CPP_DEPS += \
./src/ComPoint.d \
./src/ComPointB.d \
./src/JsonRPC.d \
./src/RPCMsg.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -DDEBUG -I"/home/dave2/git/rpcUtils/include" -I"/home/dave2/git/rpcUtils/include/mocks" -I/home/dave2/git/cpputest/include -I/home/dave2/git/cpputest/include/CppUTest -I/home/dave2/git/cpputest/include/CppUTestExt -I/home/dave2/git/rapidjson/include/rapidjson -O0 -g3 -Wall -c -fmessage-length=0 -I ../include -I /home/dave2/libs/rapidjson/include/rapidjson -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


