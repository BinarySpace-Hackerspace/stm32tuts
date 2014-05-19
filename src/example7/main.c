#include "stm32l1xx_conf.h"
#include <stdio.h>
#include "mini-printf.h"
#include "discoverylcd.h"

/*
  Setup Timer 2 and display seconds passing on the lcd
*/

char strDisp[20] ;

int main(void);

void initTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure; 
    // trigger every ms

    // get current clock
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    timerInitStructure.TIM_Prescaler = ((RCC_Clocks.SYSCLK_Frequency / 2) / 1000)-1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // timer range (16bit value) - 10 seconds
    timerInitStructure.TIM_Period = 10000-1;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);  
}

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

void RCC_Configuration(void)
{  
  
  /* Enable comparator, LCD and PWR mngt clocks */
  // RCC_APB1Periph_COMP |
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_LCD | RCC_APB1Periph_PWR,ENABLE);
    
  /* Enable ADC & SYSCFG clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  
   /* RTC Clock Source Selection */ 
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
  
  /* Enable the RTC */
  RCC_RTCCLKCmd(ENABLE);   
  
}


int main(void)
{
  RCC_Configuration();

	LCD_GLASS_Configure_GPIO();
	LCD_GLASS_Init();

  EnableClock();

  /*
  - display clock frequency on lcd

  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  mini_snprintf( strDisp, 11,  "%d", (RCC_Clocks.SYSCLK_Frequency / 10));
  LCD_GLASS_Clear();
  LCD_GLASS_DisplayString( strDisp );
  */
  
  initTimer();

  int value = 5;

  mini_snprintf( strDisp, 5,  "%d", value);
  LCD_GLASS_DisplayString( strDisp );

  int timerValue = 0;
  int prevTimerValue = -1;

  while (1)
  {
    timerValue = TIM_GetCounter(TIM2);

    if ( timerValue != prevTimerValue )
    {
        mini_snprintf( strDisp, 10,  "%d", timerValue/1000);
        if ( timerValue < prevTimerValue ) // overflowed
          LCD_GLASS_Clear();
        LCD_GLASS_DisplayString( strDisp );
        prevTimerValue = timerValue;
    }
  }
}
