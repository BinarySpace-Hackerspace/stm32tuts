// nokia lcd 3310 library
// tom van den bon - 11 May 2014
// based of information found here
// https://learn.sparkfun.com/tutorials/graphic-lcd-hookup-guide/all
// https://code.google.com/p/pcd8544/

#include "nokialcd.h"

#include "main.h"
// requires a delayms function from main.h

void lcdInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // Initialize PA8-PA12 as outputs
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = LCD_PIN_ENABLE | LCD_PIN_RESET | LCD_PIN_COMMAND | LCD_PIN_DATA | LCD_PIN_CLOCK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Reset LCD
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_SET);
  delayms(10);
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_RESET);
  delayms(10);
  GPIO_WriteBit(GPIOA, LCD_PIN_RESET, Bit_SET);

  // Initialize LCD state
  lcdWrite(LCD_CMD, 0x21); // LCD Extended Commands.
  lcdWrite(LCD_CMD, 0xB1); // Set LCD Vop (Contrast).
  lcdWrite(LCD_CMD, 0x04); // Set Temp coefficent.
  lcdWrite(LCD_CMD, 0x15); // LCD bias mode
  lcdWrite(LCD_CMD, 0x20);
  lcdWrite(LCD_CMD, 0x0C); // LCD in normal mode.    
}

void lcdXY( uint8_t x, uint8_t y)
{
  lcdWrite( 0, 0x80 | x);
  lcdWrite( 0, 0x40 | y);
}

void lcdChar(char ch)
{
  lcdWrite( LCD_DATA, 0x00);
  for ( int i = 0; i < 5; i++)
  {
    lcdWrite(LCD_DATA, ASCII[ch - 0x20][i]);
  }
  lcdWrite( LCD_DATA, 0x00);
}

void lcdGlyph( uint8_t glyph[] )
{
  lcdWrite( LCD_DATA, 0x00);
  for ( int i = 0; i < 5; i++)
  {
    lcdWrite(LCD_DATA, glyph[i]);
  }
  lcdWrite( LCD_DATA, 0x00);  
}

void lcdClear(void)
{
  for ( int i = 0; i < ( 84 * 48 ) / 8; i++)
  {
    lcdWrite(LCD_DATA, 0x00);
  }
}

void lcdStr( char *msg )
{
  while (*msg)
  {
    lcdChar(*msg++);
  }
}

void lcdWrite(uint8_t dataOrCommand, uint8_t value)
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
