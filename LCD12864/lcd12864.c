/*
             define from bcm2835.h                       define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
    RPI_V2_GPIO_P1_03 | | 5V               ->                  SDA | | 5V
    RPI_V2_GPIO_P1_05 | | GND              ->                  SCL | | GND
       RPI_GPIO_P1_07 | | RPI_GPIO_P1_08   ->                  IO7 | | TX
                  GND | | RPI_GPIO_P1_10   ->                  GND | | RX
       RPI_GPIO_P1_11 | | RPI_GPIO_P1_12   ->                  IO0 | | IO1
    RPI_V2_GPIO_P1_13 | | GND              ->                  IO2 | | GND
       RPI_GPIO_P1_15 | | RPI_GPIO_P1_16   ->                  IO3 | | IO4
                  VCC | | RPI_GPIO_P1_18   ->                  VCC | | IO5
       RPI_GPIO_P1_19 | | GND              ->                 MOSI | | GND
       RPI_GPIO_P1_21 | | RPI_GPIO_P1_22   ->                 MISO | | IO6
       RPI_GPIO_P1_23 | | RPI_GPIO_P1_24   ->                  SCK | | CE0
                  GND | | RPI_GPIO_P1_26   ->                  GND | | CE1

::if your raspberry Pi is version 1 or rev 1 or rev A
RPI_V2_GPIO_P1_03->RPI_GPIO_P1_03
RPI_V2_GPIO_P1_05->RPI_GPIO_P1_05
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13
::
*/

#include <bcm2835.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void lcdint(void);
void sendcode(unsigned char code);
void senddata(unsigned char data);
void hexToAscii(unsigned char* str,unsigned int hex,unsigned char width);
void decToAscii(unsigned char* str,unsigned int dec,unsigned char width);
void refreshlcd(const unsigned char *lcdstack);
void showLine(const unsigned char x,const unsigned char y,unsigned char* lcd_stack,unsigned char * str,...);

#define CS RPI_GPIO_P1_15
char lcd_buffer[64];

int main(int argc, char **argv)
{
    unsigned char num,i;
      if (!bcm2835_init())
        return 1;
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
    
    bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);//bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_gpio_write(CS, HIGH);
	
	printf("spi-lcd12864 :\n Start .......\n  ");				
      
    lcdint();
     //                     "1234567890123456"
    showLine(0,0,lcd_buffer,"   waveshare    ");
    showLine(0,1,lcd_buffer,"   welcome      ");
    showLine(0,2,lcd_buffer,"   DVK511       ");
    showLine(0,3,lcd_buffer,"   raspberryPi  ");

    refreshlcd(lcd_buffer);
  while(1);  
//    bcm2835_delay(10000);   
    
    sendcode(0x01);//off the lcd ,if you don't want to off the lcd ,you can cancel the code 
    printf("\n.......now the lcd12864 had closed the show............\n  ");  
    bcm2835_spi_transfer(0xf8);bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
void lcdint(void)
{
    bcm2835_gpio_write(CS, HIGH);
    sendcode(0X0C);
    sendcode(0X01);
    bcm2835_delay(2);
    memset(lcd_buffer,0,sizeof(lcd_buffer));
}

void refreshlcd(const unsigned char *lcd_stack)
{
        unsigned char addr;
        sendcode(0X02);
	for(addr=0;addr < 16;addr++)
	{
		senddata(*(lcd_stack + addr));
	}
	for(addr=32;addr < 48;addr++)
	{
		senddata(*(lcd_stack + addr));
	}
	for(addr=16;addr < 32;addr++)
	{
		senddata(*(lcd_stack + addr));
	}
	
	for(addr=48;addr < 64;addr++)
	{
		senddata(*(lcd_stack + addr));
	}



}

void showLine(const unsigned char x,const unsigned char y,unsigned char* lcd_stack,unsigned char * str,...)
{
	unsigned char coordinate = 16*y + x;
	va_list marker;
	va_start(marker,str); /* Initialize variable arguments. */ 
	while(*str!='\0')
	{
		if(coordinate>64) break;	/*·ÀÖ¹¶ÑÕ»Òç³ö*/

		if(*str == '\\')
		{
			str++;
			lcd_stack[coordinate] = *str;
		}
		else if(*str == '%')
		{
			str++;
			if (*str=='d'||*str=='D')
			{
				str++;
				decToAscii(&lcd_stack[coordinate],va_arg(marker,unsigned int),(*str-'0'));
				coordinate+=(*str-'0'-1);
			}
			else if(*str=='c'||*str=='C')
			{
				lcd_stack[coordinate] = va_arg(marker,unsigned int);
			}
			else if(*str=='x'||*str=='X')
			{
				str++;
				hexToAscii(&lcd_stack[coordinate],va_arg(marker,unsigned int),(*str-'0'));
				coordinate+=(*str-'0'-1);
			}
		}
		
		else
		{
			lcd_stack[coordinate] = *str;
		}
		str++;
		coordinate++;
	}
	va_end( marker ); /* Reset variable arguments. */ 
}


void sendcode(unsigned char code)
{
    bcm2835_spi_transfer(0xf8);
    bcm2835_spi_transfer(code&0xf0);
    bcm2835_spi_transfer(code<<4);
    bcm2835_delayMicroseconds(75);
}

void senddata(unsigned char data)
{
	bcm2835_spi_transfer(0xFA);
	bcm2835_spi_transfer(data & 0xF0);
	bcm2835_spi_transfer((data<<4));
	bcm2835_delayMicroseconds(75);
}

void decToAscii(unsigned char* str,unsigned int dec,unsigned char width)
{
	while(width)
	{
		switch(width)
		{
			case 5:
			{
				*str = (dec/10000+'0');
				dec%=10000;
				break;
			}			
			case 4:
			{
				*str = (dec/1000+'0');
				dec%=1000;
				break;
			}
			case 3:
			{
				*str = (dec/100+'0');
				dec%=100;
				break;
			}
			case 2:
			{
				*str = (dec/10+'0');
				dec%=10;
				break;
			}
			case 1:
			{
				*str = (dec+'0');
				break;
			}
			default: *str = '0';
		}
		width--;
		str++;
	}
}

void hexToAscii(unsigned char* str,unsigned int hex,unsigned char width)
{
	unsigned int tmp;
	while(width)
	{
		switch(width)
		{		
			case 4:
			{
				tmp=(hex>>12);
				if(tmp>9) *str = tmp+('A'-10);
				else *str = tmp+'0';
				break;
			}
			case 3:
			{
				tmp=(hex>>8)&0x000F;
				if(tmp>9) *str = tmp+('A'-10);
				else *str = tmp+'0';
				break;
			}
			case 2:
			{
				tmp=(hex>>4)&0x000F;
				if(tmp>9) *str = tmp+('A'-10);
				else *str = tmp+'0';
				break;
			}
			case 1:
			{
				tmp=hex&0x000F;
				if(tmp>9) *str = tmp+('A'-10);
				else *str = tmp+'0';
				break;
			}
			default: *str = '0';
		}
		width--;
		str++;
	}
}

