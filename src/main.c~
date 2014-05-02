#include "stm32l1xx_conf.h"

int main(void);
void delay(int a);

int main(void)
{
	RCC->AHBENR |=  (1UL <<  1);   
  	GPIOB->MODER   |=   (0x00005000);


	while (1)
	{
		// port output
		GPIOB->ODR ^= ( 1 << 7 );
		delay(50000);
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
