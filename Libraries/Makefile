CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###########################################

vpath %.c STM32L1xx_StdPeriph_Driver/src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m0 -march=armv6s-m
CFLAGS += -ffreestanding -nostdlib
#CFLAGS += -includestm32f0xx_conf.h -ICMSIS/Include -ICMSIS/Device/ST/STM32F0xx/Include -ISTM32F0xx_StdPeriph_Driver/inc
CFLAGS += -ICMSIS/Include -ICMSIS/Device/ST/STM32L1xx/Include -ISTM32L1xx_StdPeriph_Driver/inc

SRCS = stm32l1xx_adc.c stm32l1xx_comp.c stm32l1xx_crc.c \
	stm32l1xx_dac.c stm32l1xx_dbgmcu.c stm32l1xx_dma.c stm32l1xx_exti.c \
	stm32l1xx_flash.c stm32l1xx_gpio.c stm32l1xx_i2c.c stm32l1xx_iwdg.c \
	misc.c stm32l1xx_pwr.c stm32l1xx_rcc.c stm32l1xx_rtc.c \
	stm32l1xx_spi.c stm32l1xx_syscfg.c stm32l1xx_tim.c \
	stm32l1xx_usart.c stm32l1xx_wwdg.c

OBJS = $(SRCS:.c=.o)

.PHONY: libstm32l1.a

all: libstm32l1.a

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

libstm32l1.a: $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) libstm32l1.a
