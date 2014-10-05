//Code for fading an LED using PWM on the STM32l1xx
/*
  This code uses TIM2 to fade an led connected to pin1 on port A (PA1)
*/

#include "stm32l1xx.h"

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  uint16_t PrescalerValue = 0;

  void delay(int a);
  int main(void)
  {
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */     
       
  /* --------------------------- System Clocks Configuration ---------------------*/
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);         //this is for the fading led

  /*--------------------------------- GPIO Configuration -------------------------*/
  GPIO_StructInit(&GPIO_InitStructure);
  //initialize pin PA1 as the led to fade
  //PA1 is associated with TIM2_CH2
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //set pin mode to use an alternate function (AF)
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //who knows think its 'push-pull mode'
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);     //configure alternate function to us TIM2

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
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;         //Timer counts up from 0

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel2 
     PA1 is associated with TIM2_CH2
     The Channel number is in the function call: 
     TIM_OC2Init = channel 2 , 
     TIM_OC3Init = channel 3 (associated with PA3) */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   //pin output mode configured as PWM (repetitive)
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);

  while (1)
  {

    //fade up
    for (int i = 0; i <999; i ++){

      TIM_SetCompare2(TIM2,i);                                         //Pulse width is set with this function
      delay(100);                                                      // the '2' relates to the channel in use
    }

    delay(10000);

    //fade down
    for (int i = 999; i >0; i --){

      TIM_SetCompare2(TIM2,i);
      delay(100);
    }

    delay(10000);

  }
}

void delay( int a )
{
  volatile int i, j;

  for ( i = 0; i < a; i++ )
  {
    j++;
  }
}

