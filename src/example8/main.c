#include "stm32l1xx_conf.h"

#include "nokialcd.h"
#include "main.h"

static volatile uint32_t TimingDelay;
void TimingDelay_Decrement(void);
 
void delayms(uint32_t nTime);

// glyph editor: http://www.carlos-rodrigues.com/projects/pcd8544/
static uint8_t custom_glyph[] = {0xff, 0x81, 0xbd, 0x81, 0xff};

int main(void)
{

  // tick every ms
  // Clock source PLL(HSE)
  // PLL 3
  SysTick_Config((HSE_VALUE / 3) / 1000); // Cannot exceed 16,777,215
 
  /* Set SysTick Preemption Priority, it's a system handler rather than a regular interrupt */
  NVIC_SetPriority(SysTick_IRQn, 0x04);

  lcdInit();
  lcdClear();
  lcdXY( 1, 2 );
  lcdStr( "LCD Test");
  lcdXY( 1, 3 );
  lcdGlyph( custom_glyph ) ;
  while (1);

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