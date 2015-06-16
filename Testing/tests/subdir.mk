################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/TestRunner.cpp \
../tests/test_ComPoint.cpp \
../tests/test_Error.cpp \
../tests/test_JsonRPC.cpp \
../tests/test_LogUnit.cpp \
../tests/test_Plugin.cpp 

OBJS += \
./tests/TestRunner.o \
./tests/test_ComPoint.o \
./tests/test_Error.o \
./tests/test_JsonRPC.o \
./tests/test_LogUnit.o \
./tests/test_Plugin.o 

CPP_DEPS += \
./tests/TestRunner.d \
./tests/test_ComPoint.d \
./tests/test_Error.d \
./tests/test_JsonRPC.d \
./tests/test_LogUnit.d \
./tests/test_Plugin.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -DDEBUG -I"/home/dave2/git/rpcUtils/include" -I"/home/dave2/git/rpcUtils/include/mocks" -I/home/dave2/git/cpputest/include -I/home/dave2/git/cpputest/include/CppUTest -I/home/dave2/git/cpputest/include/CppUTestExt -I/home/dave2/git/rapidjson/include/rapidjson -O0 -g3 -Wall -c -fmessage-length=0 ${CXXFLAGS} -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


