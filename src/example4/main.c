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

// taken from https://github.com/maniacbug/RF24/blob/master/nRF24L01.h

/* Memory Map */
#define CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW 0x03
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS 0x07
#define OBSERVE_TX 0x08
#define CD 0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
#define DYNPD 0x1C
#define FEATURE 0x1D

/* Bit Mnemonics */
#define MASK_RX_DR 6
#define MASK_TX_DS 5
#define MASK_MAX_RT 4
#define EN_CRC 3
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0
#define ENAA_P5 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0
#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0
#define AW 0
#define ARD 4
#define ARC 0
#define PLL_LOCK 4
#define RF_DR 3
#define RF_PWR 6
#define RX_DR 6
#define TX_DS 5
#define MAX_RT 4
#define RX_P_NO 1
#define TX_FULL 0
#define PLOS_CNT 4
#define ARC_CNT 0
#define TX_REUSE 6
#define FIFO_FULL 5
#define TX_EMPTY 4
#define RX_FULL 1
#define RX_EMPTY 0
#define DPL_P5 5
#define DPL_P4 4
#define DPL_P3 3
#define DPL_P2 2
#define DPL_P1 1
#define DPL_P0 0
#define EN_DPL 2
#define EN_ACK_PAY 1
#define EN_DYN_ACK 0

/* Instruction Mnemonics */
#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE 0x50
#define R_RX_PL_WID 0x60
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define NOP 0xFF

/* Non-P omissions */
#define LNA_HCURR 0

/* P model memory Map */
#define RPD 0x09

/* P model bit Mnemonics */
#define RF_DR_LOW 5
#define RF_DR_HIGH 3
#define RF_PWR_LOW 1
#define RF_PWR_HIGH 2

#define SPI_CS_HIGH GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
#define SPI_CS_LOW GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);

#define NRF_CE_HIGH GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_SET);
#define NRF_CE_LOW GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_RESET);

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

uint16_t spi_transfer( uint16_t data )
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	usart_print( "(senddata)" );
	SPI_I2S_SendData( SPI2,	data );
	usart_print( "(wait)" );
	// wait for transfer to finish
	while (SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) == RESET);
	usart_print( "(recvdata)" );
	return SPI_I2S_ReceiveData(SPI2);
}

void spi_send( uint16_t data )
{
	// wait for transfer to finish
	while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) == RESET );

	SPI_I2S_SendData( SPI2, data );
}

uint16_t spi_read(void)
{
	// wait for transfer to finish	
	while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) == RESET );

	return SPI_I2S_ReceiveData(SPI2);
}

uint16_t write_register_data(uint8_t reg, const uint8_t* buff, uint8_t len )
{
	uint16_t status;

	SPI_CS_LOW;
	status = spi_transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
	while ( len-- )
	{
		spi_transfer(*buff++);
	}	
	SPI_CS_HIGH;

	return status;
}

uint16_t read_register_data(uint8_t reg, uint8_t* buff, uint8_t len )
{
	uint16_t status;

	SPI_CS_LOW;
	status = spi_transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
	while ( len-- )
	{
		*buff++ = spi_transfer(0xff);
	}	
	SPI_CS_HIGH;

	return status;
}

uint16_t write_register_value(uint8_t reg, uint8_t value)
{
	uint16_t status;

	usart_print( "(LOW)");
	SPI_CS_LOW;
//	usart_print( "(reg)");
//	usart_write( reg);
//	usart_print( "(value)" );
//	usart_write( value );
	status = spi_transfer(W_REGISTER | ( REGISTER_MASK & reg ) );
	spi_transfer( value );
	usart_print( "(HIGH)");
	SPI_CS_HIGH;

	return status;
}

uint16_t read_register_value(uint8_t reg)
{
	uint16_t status;

	SPI_CS_LOW;
	status = spi_transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
	spi_transfer( 0xff );
	SPI_CS_HIGH;

	return status;
}


int main(void) {
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
        gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
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
		spi_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
		spi_init.SPI_NSS = SPI_NSS_Hard;
		SPI_Init(SPI2, &spi_init);

		SPI_CS_HIGH;
		NRF_CE_LOW;

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

		// allow radio to settle
	

		delay(50000);
		
		usart_print( "set 1500us timeout: " );
		uint16_t status = write_register_value(SETUP_RETR,(4 << ARD) | (15 << ARC));
		usart_write( status ); usart_print( "\r\n" );

		usart_print( "begin\r\n" );

        while (1) {   

			
  		}
 

        return 0;
}
