#include "stm32l1xx.h"

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  uint16_t PrescalerValue = 0;

  void delay(int a);
  int main(void)
  {

  /* --------------------------- System Clocks Configuration ---------------------*/
   /* Enable the HSI */
  RCC_HSICmd(ENABLE);
  /* Wait until HSI oscillator is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);        
  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  /*--------------------------------- GPIO Configuration -------------------------*/

  /* Configure PB.12 in analog mode (ADC1 Channel0)  -*/
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //initialize pin PA1 as the led to fade
  //PA1 is associated with TIM2_CH2
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);     

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

/*----------------------------------TIM2 Configuration -------------------------------------
  The TIM2CLK frequency is set to SystemCoreClock (Hz)
  SystemCoreClock is = 32MHz (32000000)
  The TIM2 is running at 32000000/1000000 

  Configure Timer (I sure hope this math is correct):

  PWM frequency = 1000 hz from a 32 000 000 hz system clock , using 1000 steps of the timer clock. 
  32000000/32 = 1000000
  1000000/1000 = 1000

  This allows us to define a pulse width output from 0-100% with a precision of 1%
----------------------------------------------------------------------- */

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;        //0...31
  TIM_TimeBaseStructure.TIM_Period = 1000-1;                          //0...999
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);

  while (1)
  {

    if(ADC_GetSoftwareStartConvStatus(ADC1) == RESET){

    uint16_t adcValue = ADC_GetConversionValue(ADC1)/4;
    TIM_SetCompare2(TIM2,adcValue);      
    }                                  
     
  }
}

