#include "stm32l1xx_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// uart
// RX = PA10
// TX = PA9

// nrf
// CS  = PB12 (SPI_NSS)
// SCK = PB13 (SPI_SCK)
// MI  = PB14 (SPI_MISO)
// MO  = PB15 (SPI_MOSI)
// CE  = PD2


void usart_write(uint8_t ch)
{
      USART_SendData(USART1, (uint8_t) ch);
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      {
      }
}

uint8_t usart_read(void){
     while ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
        return (uint8_t)USART_ReceiveData(USART1);
}

uint8_t usart_available(void)
{
	if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET )
		return 1;

	return 0;		
}
 

void usart_print( char *msg )
{
	int len = strlen( msg );

	for ( int c = 0; c < len; c++ )
		usart_write( (uint8_t)*msg++ );
}

void delay( int a )
{
	volatile int i, j;

	for ( i = 0; i < a; i++ )
	{
		j++;
	}
} 

int main(void) {
 		int i = 10;

        GPIO_InitTypeDef gpio_init;
        USART_InitTypeDef usart_init;
        USART_ClockInitTypeDef usart_clk_init;
		SPI_InitTypeDef spi_init;
 
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOD, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

		// spi
		// CS  = PB12 (SPI_NSS)
		// SCK = PB13 (SPI_SCK)
		// MI  = PB14 (SPI_MISO)
		// MO  = PB15 (SPI_MOSI)
		// CE  = PD2

		gpio_init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOB, &gpio_init);

		GPIO_PinAFConfig( GPIOB, GPIO_PinSource12, GPIO_AF_SPI2 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource13, GPIO_AF_SPI2 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource14, GPIO_AF_SPI2 );
		GPIO_PinAFConfig( GPIOB, GPIO_PinSource15, GPIO_AF_SPI2 );

		gpio_init.GPIO_Pin = GPIO_Pin_2;
        gpio_init.GPIO_Mode = GPIO_Mode_OUT;
        gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOD, &gpio_init);

		SPI_StructInit(&spi_init);
		spi_init.SPI_Mode = SPI_Mode_Master;
		SPI_Init(SPI2, &spi_init);

		SPI_Cmd(SPI2, ENABLE);

		// uart
        // PA9 = Tx, PA10 = Rx
        gpio_init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        gpio_init.GPIO_Mode = GPIO_Mode_AF;
        gpio_init.GPIO_Speed = GPIO_Speed_40MHz;
        gpio_init.GPIO_OType = GPIO_OType_PP;
        gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &gpio_init);

        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
 
        USART_ClockStructInit(&usart_clk_init);
        USART_ClockInit(USART1, &usart_clk_init);
 
        usart_init.USART_BaudRate =            9600;
        usart_init.USART_WordLength =          USART_WordLength_8b;
        usart_init.USART_StopBits =            USART_StopBits_1;
        usart_init.USART_Parity =              USART_Parity_No ;
        usart_init.USART_Mode =                USART_Mode_Rx | USART_Mode_Tx;
        usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(USART1, &usart_init);
        USART_Cmd(USART1,ENABLE);
 
	    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
		usart_print( "nrf radio example\r\n" );
 
		// void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data)
		// uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx)

        while (1) {   

			
  		}
 

        return 0;
}
