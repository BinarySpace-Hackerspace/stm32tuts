#include "stm32l1xx_conf.h"
 

/* using systick handler */

static volatile uint32_t TimingDelay;
void TimingDelay_Decrement(void);
 
void GPIO_Configuration(void);
void delayms(uint32_t nTime);

int main(void)
{
  GPIO_Configuration();
 
  // tick every ms
  // Clock source PLL(HSE)
  // PLL 3
  SysTick_Config((HSE_VALUE / 3) / 1000); // Cannot exceed 16,777,215
 
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