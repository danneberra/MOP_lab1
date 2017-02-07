/*
 * 	startup.c
 *
 */

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );

#define GPIO_D 0x40020C00
#define GPIO_MODER ((volatile unsigned int *) (GPIO_D))
#define GPIO_OTYPER ((volatile unsigned short *) (GPIO_D+0x4))
#define GPIO_PUPDR ((volatile unsigned int *) (GPIO_D+0xC))
#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO_D+0x10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (GPIO_D+0x11))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_D+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_D+0x15)) 
#define GPIO_E 0x40021000



void startup ( void )
{
asm volatile(
	" LDR R0,=0x2001C000\n"		/* set stack */
	" MOV SP,R0\n"
	" BL main\n"				/* call main */
	".L1: B .L1\n"				/* never return */
	) ;
}


void appInit_alt (void)
{
	*((unsigned short*) 0x40021000) = 0x5555; //aktivera port GPIO_E som utport till 7segDisplay
	*GPIO_MODER &= 0x0000FFFF;
	*GPIO_MODER |= 0x55000000;
	*GPIO_OTYPER &= 0x00FF;
	*GPIO_PUPDR &= 0x0000FFFF;
	*GPIO_PUPDR |= 0xAAAA00000;
}


/* gammal kod, sparad ifall den behövs: void appInit (void)
{
	*((unsigned short*) 0x40021000) = 0x5555; //aktivera port GPIO_E som utport till 7segDisplay
	*((unsigned short*) 0x40020C02) = 0x5500;
	*((unsigned char*) 0x40020C05) = 0x0F;
	*((unsigned short*) 0x40020C0E) = 0x0000;
	*((unsigned short*) 0x40020C0E) = 0xAA;
	
} */

void kbdActivate (unsigned int row)
{ /* aktiverar angiven rad hos tangentbordet, eller deaktivera samtliga */

	switch (row) 
	{
		case 1: *GPIO_ODR_HIGH = 0x10; break;
		case 2: *GPIO_ODR_HIGH = 0x20; break;
		case 3: *GPIO_ODR_HIGH = 0x40; break;
		case 4: *GPIO_ODR_HIGH = 0x80; break;
		case 0: *GPIO_ODR_HIGH = 0x00; break;
	}
}



int kbdGetCol (void)
{ /* om någon tangent (i aktiverad rad) är nedtryckt, returnera dess kolumner, annars, returnera 0 */
	unsigned char c;
	c = *GPIO_IDR_HIGH;
	if (c & 0x8) return 4;
	if (c & 0x4) return 3;
	if (c & 0x2) return 2;
	if (c & 0x1) return 1;
	return 0;
}




unsigned char keyb (void)
{
	unsigned char key[]= {1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
	int row, col;
	for (row=1; row <= 4; row++){
		kbdActivate(row);
		if((col = kbdGetCol() ))
		{
			kbdActivate(0);
			return key [4*(row-1)+(col-1)];
		}
	}
	kbdActivate(0);
	return 0xFF;
}


void out7seg(unsigned char key_)
{
	if (key_ > 0xF)
	{
	*((unsigned char*) 0x40021014) = 0x00;
	}
	else
	{
	
	unsigned char SegCodes[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x77,0x7C,0x58,0x5E,0x79,0x71};
	*((unsigned char*) 0x40021014) = SegCodes[key_];
	}

}


int main(void)
{
	appInit_alt();
	
	*((unsigned char*) 0x40021014) = 0x6D;
	while(1)
	{
		out7seg(keyb());
	}
	
}

