################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c \
../na_check_position.c \
../na_errno.c \
../na_figure.c \
../na_genetics.c \
../na_rastr.c \
../na_rastr_nest.c \
../na_routine.c \
../na_transform.c 

OBJS += \
./main.o \
./na_check_position.o \
./na_errno.o \
./na_figure.o \
./na_genetics.o \
./na_rastr.o \
./na_rastr_nest.o \
./na_routine.o \
./na_transform.o 

C_DEPS += \
./main.d \
./na_check_position.d \
./na_errno.d \
./na_figure.d \
./na_genetics.d \
./na_rastr.d \
./na_rastr_nest.d \
./na_routine.d \
./na_transform.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


