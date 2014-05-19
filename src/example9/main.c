#include "stm32l1xx_conf.h"
#include <stdio.h>

int main(void);

void clockInit()
{
  // divide HCLK / 2
  RCC_HCLKConfig(RCC_SYSCLK_Div2);

  // enable HSI
  RCC_HSICmd(ENABLE);
  RCC_PLLCmd(DISABLE);
  // wait for HSI to get ready
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  // configure PLL - x4 /2 
  RCC_PLLConfig( RCC_PLLSource_HSI, RCC_PLLMul_8,  RCC_PLLDiv_4 );
  RCC_PLLCmd(ENABLE);

  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET );

  // set hsi as clock
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

  // set pll as clock
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
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

