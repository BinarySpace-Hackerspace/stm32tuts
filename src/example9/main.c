#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);

** do not use **

void clockInit()
{
	// set flash latency
	FLASH_SetLatency(FLASH_Latency_1);

	// enable hsi - 16Mhz
	RCC_HSICmd(ENABLE);	

	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
	{		
	}	


	// start hsi clock (16Mhz)
	// divide by 2, multiply by 4
	// 32 Mhz Clock
	RCC_PLLConfig( RCC_PLLSource_HSI, RCC_PLLMul_4,  RCC_PLLDiv_2 );
	RCC_PLLCmd(ENABLE);

	while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET ) 
	{		
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	RCC_HCLKConfig(RCC_SYSCLK_Div1);                                                           
  	RCC_PCLK1Config(RCC_HCLK_Div1);                                                             
  	RCC_PCLK2Config(RCC_HCLK_Div1);
}

int main(void)
{
	RCC_DeInit();
	clockInit();
	

	GPIO_InitTypeDef gpio;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_8;	
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_40MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &gpio);

	RCC_MCOConfig(RCC_MCOSource_HSI, RCC_MCODiv_16);

	while (1)
	{
	}

}

