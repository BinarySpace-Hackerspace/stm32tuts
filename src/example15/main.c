//Code for Servo Control on the STM32l1xx
/*
  This code uses TIM2 to control servo on channel1 (PA5)
  I believe this was orginally written by sourcer32@gmail.com.
  I had to tweak it some to get it working for the stm32l152 but thanks to him/her.
*/

#include "stm32l1xx.h"

/**************************************************************************************/
 
// Integers, or scaled integers (*10) will be more efficient here
volatile int led_state=1;
volatile float servo_angle= 0.0; // +/- 90.0 degrees
 int counter = 0;
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  { 
    
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
 
    // minimum high of 600 us for -90 degrees, with +90 degrees at 2400 us, 10 us per degree
    //  timer timebase set to us units to simplify the configuration/math
    TIM_SetCompare1(TIM2, 1500 + (int)(servo_angle * 10.0f) );
    
  }
}

  /* --------------------------- System Clocks Configuration -----------------*/
void RCC_Configuration(void)
{
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
  /* GPIOD clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
}

 /*--------------------------------- NVIC Configuration -------------------------*/
void NVIC_Configuration(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
 
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

 /*--------------------------------- GPIO Configuration -------------------------*/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit(&GPIO_InitStructure);

  /*servo pin setup*/
  //PA1 is associated with TIM2_CH2
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);     //configure alternate function to us TIM2

  /* I used this pin for debugging, it toggles an led whenever I pushed the buttons to set the servo position */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                             
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*----------button pins-----------*/
  /* used for setting the position of the servo*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                             
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                             
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                             
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*----------------------------------TIM2 Configuration -------------------------------------*/
void TIM2_Configuration(void)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* Time base configuration - SystemCoreClock = 32000000 for 32 MHz board */
  /* this is where  struggled, although the system clock is set to 32Mhz it is in reality i would guess at 2Mhz */
  TIM_TimeBaseStructure.TIM_Prescaler  = (uint16_t) (2 - 1);        //prescaler is 2 so timebase is 1Mhz (1us)
  TIM_TimeBaseStructure.TIM_Period = 20000-1;                       //1Mhz / 20000 = 50hz ; (20ms, its what servos need)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;       
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

   /* Enable TIM4 Preload register on ARR */
  /* I dont know what this does,or is for that matter but the code works with ot without it*/
  TIM_ARRPreloadConfig(TIM2, ENABLE);   

  /* PWM1 Mode configuration: PA1 is associated with TIM2_CH2 */
  /* TIM PWM1 Mode configuration: Channel */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                   
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  /* Again, i dont know what this is or does and the code works with or without it*/
  TIM_OCInitStructure.TIM_Pulse = 1500; // Servo Top-Center
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}
  
  int main(void)
  {

    RCC_Configuration();
 
    GPIO_Configuration();
 
    NVIC_Configuration();
 
    TIM2_Configuration();

  while (1)
  {

    // read buttons connected to PB12, PB13, PB14 and set the postion of the servo
    // The led being set high and low (PA12) was used for debugging purposes and isn't needed
    // you can set the value of the servo in degrees. The range is from -90 to 90.
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
    {
      GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
      servo_angle = -80.0 ;
    } 
    else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
    {
      GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
      servo_angle= 0.0 ;
    }
    else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
    {
      GPIO_WriteBit(GPIOA, GPIO_Pin_0, 1);
      servo_angle = 90.0 ;
    }
    else 
    {
      GPIO_WriteBit(GPIOA, GPIO_Pin_0, 0);
    }
    
  }
}



