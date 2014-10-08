

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

ADC_InitTypeDef ADC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;

void delay(int a);

int main(void)
{

/*  ADC1 Channels
    check the user manual for breakdown of all adc pins
    
    channel 5 = PA5 - free pin using this channel for adc
*/        
  /* Enable the HSI */
  RCC_HSICmd(ENABLE);
  /* Wait until HSI oscillator is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  /* Enable the GPIOB Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  /* Configure PB.12 in analog mode (ADC1 Channel0)  -*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // green led
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // blue led
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* ADC1 Configuration -----------------------------------------------------*/
  /*
    This is set to ADC_Prescaler_Div1 by default and for a reason lost on me, does not work
    without these two lines the adc will not work
  */
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None  ;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel0 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_16Cycles);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait until the ADC1 is ready */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
  {
  }

  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConv(ADC1);

  static int led_state=0;

  while (1)
  {

  uint16_t adcValue = ADC_GetConversionValue(ADC1);
   if(adcValue > 2000){

    GPIO_WriteBit(GPIOB, GPIO_Pin_6, 1);
   }else{
    GPIO_WriteBit(GPIOB, GPIO_Pin_6, 0);

   }

    // port output - alternate between green and blue
    led_state = !led_state;
    GPIO_WriteBit(GPIOB, GPIO_Pin_7, led_state ? Bit_SET : Bit_RESET);
    delay(50000);

  }
}

//-----------------------------------------------------------------


void delay( int a )
{
  volatile int i, j;

  for ( i = 0; i < a; i++ )
  {
    j++;
  }
}


//-----------------------------------------------------------------





#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
