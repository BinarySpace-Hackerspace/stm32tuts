#include "stm32l1xx_conf.h"
#include <stdio.h>
#include "mini-printf.h"
#include "discoverylcd.h"

/*
  Real Time Clock Example
*/

char strDisp[20] ;

int main(void);

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

  // setup lse for rtc and lcd
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

  RTC_WaitForSynchro();  

}

void RCC_Configuration(void)
{  
  
  /* Enable comparator, LCD and PWR mngt clocks */
  // RCC_APB1Periph_COMP |
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_LCD | RCC_APB1Periph_PWR,ENABLE);
    
  /* Enable ADC & SYSCFG clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);
}


int main(void)
{
  RCC_Configuration();
  EnableClock();

  LCD_GLASS_Configure_GPIO();
	LCD_GLASS_Init();


  /*
  - display clock frequency on lcd

  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  mini_snprintf( strDisp, 11,  "%d", (RCC_Clocks.SYSCLK_Frequency / 10));
  LCD_GLASS_Clear();
  LCD_GLASS_DisplayString( strDisp );
  */

  // init rtc
  RTC_InitTypeDef rtcInit;
  rtcInit.RTC_HourFormat = RTC_HourFormat_24;
  rtcInit.RTC_AsynchPrediv = 0x7F; 
  rtcInit.RTC_SynchPrediv = 0xFF; 
  RTC_Init(&rtcInit);  

/*
  // Set the date (BCD)
  RTC_DateTypeDef RTC_DateStructure;
  RTC_DateStructure.RTC_Year = 0x13;
  RTC_DateStructure.RTC_Month = RTC_Month_October;
  RTC_DateStructure.RTC_Date = 0x26;
  RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
  RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
*/

  // Set the time (BCD)
  RTC_TimeTypeDef RTC_TimeStructure;
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x01;
  RTC_TimeStructure.RTC_Minutes = 0x00;
  RTC_TimeStructure.RTC_Seconds = 0x00;
  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

  int prevSecond = -1;

  while (1)
  {
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure );

    if ( RTC_TimeStructure.RTC_Seconds != prevSecond )
    {      
      mini_snprintf( strDisp, 10,  "%d %d", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes );
      LCD_GLASS_Clear();
      LCD_GLASS_DisplayString( strDisp );
      prevSecond = RTC_TimeStructure.RTC_Seconds;
    }
  }
}
