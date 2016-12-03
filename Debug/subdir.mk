################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c \
../na_errno.c \
../na_genetics.c \
../na_routine.c 

OBJS += \
./main.o \
./na_errno.o \
./na_genetics.o \
./na_routine.o 

C_DEPS += \
./main.d \
./na_errno.d \
./na_genetics.d \
./na_routine.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


