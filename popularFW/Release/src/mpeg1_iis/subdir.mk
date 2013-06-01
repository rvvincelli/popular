################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mpeg1_iis/common.c \
../src/mpeg1_iis/decode.c \
../src/mpeg1_iis/huffman.c 

OBJS += \
./src/mpeg1_iis/common.o \
./src/mpeg1_iis/decode.o \
./src/mpeg1_iis/huffman.o 

C_DEPS += \
./src/mpeg1_iis/common.d \
./src/mpeg1_iis/decode.d \
./src/mpeg1_iis/huffman.d 


# Each subdirectory must supply rules for building sources it contributes
src/mpeg1_iis/%.o: ../src/mpeg1_iis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


