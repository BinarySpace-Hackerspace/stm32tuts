#include "stm32l1xx_conf.h"
 

/* using systick handler */

static volatile uint32_t TimingDelay;
void TimingDelay_Decrement(void);
 
void GPIO_Configuration(void);
void delayms(uint32_t nTime);

void EnableClock()
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
  GPIO_Configuration();
  EnableClock();
 
  // tick every ms

  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  SysTick_Config((RCC_Clocks.SYSCLK_Frequency / 2) / 1000); // Cannot exceed 16,777,215
 
  /* Set SysTick Preemption Priority, it's a system handler rather than a regular interrupt */
  NVIC_SetPriority(SysTick_IRQn, 0x04);

	static int led_state=0;

	while (1)
	{
		// port output - alternate between green and blue
		led_state = !led_state;
		GPIO_WriteBit(GPIOB, GPIO_Pin_7, led_state ? Bit_SET : Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_6, led_state ? Bit_RESET : Bit_SET );
		delayms(1000);
	}
}
 
 
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
 
  // PB.06 BLUE LED, PB.07 GREEN LED
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 
void delayms(uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}
 
// Called from SysTick_Handler
 
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00){
    TimingDelay--;}
}
 
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}