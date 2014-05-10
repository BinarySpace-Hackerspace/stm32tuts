#include "stm32l1xx_conf.h"
 

#define LCD_PIN_ENABLE      GPIO_Pin_12
#define LCD_PIN_RESET       GPIO_Pin_11
#define LCD_PIN_COMMAND     GPIO_Pin_10
#define LCD_PIN_DATA        GPIO_Pin_9
#define LCD_PIN_CLOCK       GPIO_Pin_8

#define LCD_CMD             0
#define LCD_DATA            1

GPIO_InitTypeDef GPIO_InitStructure;
uint8_t messageBytes[] = {
  0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x7c, 0x08, 0x04, 0x04, 0x08, 0x00,
  0x04, 0x3f, 0x44, 0x40, 0x20, 0x00, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, 0x21, 0x41, 0x45, 0x4b, 0x31, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x46, 0x49, 0x49, 0x49, 0x31, 0x00, 0x01, 0x01, 0x7f, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00,
  0x21, 0x41, 0x45, 0x4b, 0x31, 0x00, 0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00,
  0x00, 0x44, 0x7d, 0x40, 0x00, 0x00, 0x48, 0x54, 0x54, 0x54, 0x20, 0x00, 0x38, 0x44, 0x44, 0x44, 0x20, 0x00,
  0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x1c, 0x20, 0x40, 0x20, 0x1c, 0x00, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00,
  0x7c, 0x08, 0x04, 0x04, 0x08, 0x00, 0x0c, 0x50, 0x50, 0x50, 0x3c, 0x00
};

void LcdWrite(uint8_t dataOrCommand, uint8_t value);


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

  // Reset LCD
  //GPIO_SetBits(GPIOA, LCD_PIN_RESET);
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_SET);
  delayms(10);
  //GPIOResetBits(GPIOA, LCD_PIN_RESET);
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_RESET);
  delayms(10);
  //GPIO_SetBits(GPIOA, LCD_PIN_RESET);
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_SET);

  // Initialize LCD state
  LcdWrite(LCD_CMD, 0x21); // LCD Extended Commands.
  LcdWrite(LCD_CMD, 0xB1); // Set LCD Vop (Contrast).
  LcdWrite(LCD_CMD, 0x04); // Set Temp coefficent.
  LcdWrite(LCD_CMD, 0x15); // LCD bias mode
  LcdWrite(LCD_CMD, 0x20);
  LcdWrite(LCD_CMD, 0x0C); // LCD in normal mode.

  // Write the message
  int i;
  for (i=0; i<sizeof(messageBytes); i++)
  {
    LcdWrite(LCD_DATA, messageBytes[i]);
  }

  // Clear the remainder of the display
  for (i=0; i<84*4; i++)
    LcdWrite(LCD_DATA, 0x00);

  while (1);

}
 
 
void GPIO_Configuration(void)
{
  /*
  // Configure all unused GPIO port pins in Analog Input mode
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB |
             RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD |
             RCC_AHBPeriph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB |
               RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD |
               RCC_AHBPeriph_GPIOE, DISABLE);
  */

  // Initialize PA8-PA12 as outputs
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = LCD_PIN_ENABLE | LCD_PIN_RESET | LCD_PIN_COMMAND | LCD_PIN_DATA | LCD_PIN_CLOCK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void LcdWrite(uint8_t dataOrCommand, uint8_t value)
{
  if (dataOrCommand)
    GPIO_WriteBit(GPIOA, LCD_PIN_COMMAND, Bit_SET);
  else
    GPIO_WriteBit(GPIOA, LCD_PIN_COMMAND, Bit_RESET);

  GPIO_WriteBit(GPIOA, LCD_PIN_ENABLE, Bit_RESET);

  int i;
  for (i = 0; i < 8; i++)
  {
    GPIO_WriteBit(GPIOA, LCD_PIN_CLOCK, Bit_RESET);
    if (value & 0x80)
      GPIO_WriteBit(GPIOA, LCD_PIN_DATA, Bit_SET);
    else
      GPIO_WriteBit(GPIOA, LCD_PIN_DATA, Bit_RESET);
    value <<= 1;
    GPIO_WriteBit(GPIOA, LCD_PIN_CLOCK, Bit_SET);

  }

  GPIO_WriteBit(GPIOA, LCD_PIN_ENABLE, Bit_SET);

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