################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../fft_lnkx.cmd 

LIB_SRCS += \
../usbstk5515bsl.lib 

ASM_SRCS += \
../cbrev32.asm \
../cfft32_noscale.asm \
../cfft32_scale.asm \
../cifft32_noscale.asm \
../cifft32_scale.asm \
../twiddle32.asm \
../vectors.asm 

C_SRCS += \
../C5515_AIC3204_setup.c \
../C5515_UART.c \
../C55_setup.c \
../InitializeC55xxCPU.c \
../Support_Plotting_1_1.c \
../WP_packet_support.c \
../main.c \
../main0.c 

OBJS += \
./C5515_AIC3204_setup.obj \
./C5515_UART.obj \
./C55_setup.obj \
./InitializeC55xxCPU.obj \
./Support_Plotting_1_1.obj \
./WP_packet_support.obj \
./cbrev32.obj \
./cfft32_noscale.obj \
./cfft32_scale.obj \
./cifft32_noscale.obj \
./cifft32_scale.obj \
./main.obj \
./main0.obj \
./twiddle32.obj \
./vectors.obj 

ASM_DEPS += \
./cbrev32.pp \
./cfft32_noscale.pp \
./cfft32_scale.pp \
./cifft32_noscale.pp \
./cifft32_scale.pp \
./twiddle32.pp \
./vectors.pp 

C_DEPS += \
./C5515_AIC3204_setup.pp \
./C5515_UART.pp \
./C55_setup.pp \
./InitializeC55xxCPU.pp \
./Support_Plotting_1_1.pp \
./WP_packet_support.pp \
./main.pp \
./main0.pp 

C_DEPS__QUOTED += \
"C5515_AIC3204_setup.pp" \
"C5515_UART.pp" \
"C55_setup.pp" \
"InitializeC55xxCPU.pp" \
"Support_Plotting_1_1.pp" \
"WP_packet_support.pp" \
"main.pp" \
"main0.pp" 

OBJS__QUOTED += \
"C5515_AIC3204_setup.obj" \
"C5515_UART.obj" \
"C55_setup.obj" \
"InitializeC55xxCPU.obj" \
"Support_Plotting_1_1.obj" \
"WP_packet_support.obj" \
"cbrev32.obj" \
"cfft32_noscale.obj" \
"cfft32_scale.obj" \
"cifft32_noscale.obj" \
"cifft32_scale.obj" \
"main.obj" \
"main0.obj" \
"twiddle32.obj" \
"vectors.obj" 

ASM_DEPS__QUOTED += \
"cbrev32.pp" \
"cfft32_noscale.pp" \
"cfft32_scale.pp" \
"cifft32_noscale.pp" \
"cifft32_scale.pp" \
"twiddle32.pp" \
"vectors.pp" 

C_SRCS__QUOTED += \
"../C5515_AIC3204_setup.c" \
"../C5515_UART.c" \
"../C55_setup.c" \
"../InitializeC55xxCPU.c" \
"../Support_Plotting_1_1.c" \
"../WP_packet_support.c" \
"../main.c" \
"../main0.c" 

ASM_SRCS__QUOTED += \
"../cbrev32.asm" \
"../cfft32_noscale.asm" \
"../cfft32_scale.asm" \
"../cifft32_noscale.asm" \
"../cifft32_scale.asm" \
"../twiddle32.asm" 


