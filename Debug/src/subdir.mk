################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/compiler.c \
../src/errorcodes.c \
../src/functions.c \
../src/main.c \
../src/parser.c \
../src/tokens.c \
../src/utils.c 

OBJS += \
./src/compiler.o \
./src/errorcodes.o \
./src/functions.o \
./src/main.o \
./src/parser.o \
./src/tokens.o \
./src/utils.o 

C_DEPS += \
./src/compiler.d \
./src/errorcodes.d \
./src/functions.d \
./src/main.d \
./src/parser.d \
./src/tokens.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


