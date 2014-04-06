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
void ee_write(unsigned int BufferOffset,unsigned char data);
unsigned char ee_read(unsigned int BufferOffset);

#define PIN RPI_GPIO_P1_24

int main(int argc, char **argv)
{
    unsigned char num,i;
      if (!bcm2835_init())
        return 1;
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);//    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_gpio_write(PIN, HIGH);
	
	printf("spi-at45db TEST :\n Start to write  please to wait.......\n  ");				
	for(num=0;num<255;num++)
	{
		ee_write((unsigned int)num,num);
	        delayMicroseconds(75);
        }

        printf("start to read:");
	if(ee_read((unsigned int)1)!=1)
	{
		printf("open at45db err! bad!\n");
	} 
	else
		for(num=0;num<255;num++)
		{
		printf( "%02X  ", ee_read((unsigned int)num));
        	bcm2835_delay(5);
		}

    printf("test end!\n");
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

void ee_write(unsigned int BufferOffset,unsigned char data)
{
bcm2835_gpio_write(PIN, LOW);
        bcm2835_spi_transfer(0x84);
	delayMicroseconds(6);
        bcm2835_spi_transfer(0xff);
	delayMicroseconds(12);
        bcm2835_spi_transfer((unsigned char)(BufferOffset>>8));
	delayMicroseconds(12);
        bcm2835_spi_transfer((unsigned char)BufferOffset);
	delayMicroseconds(12);
        bcm2835_spi_transfer(data);
	delayMicroseconds(12);
bcm2835_gpio_write(PIN, HIGH);

}

unsigned char ee_read(unsigned int BufferOffset)
{
      unsigned char temp;
bcm2835_gpio_write(PIN, LOW);
        delayMicroseconds(12);
        bcm2835_spi_transfer(0xD4);
	delayMicroseconds(12);
        bcm2835_spi_transfer(0xff);
	delayMicroseconds(12);
        bcm2835_spi_transfer((unsigned char)(BufferOffset>>8));
	delayMicroseconds(12);
        bcm2835_spi_transfer((unsigned char)BufferOffset);
	delayMicroseconds(12);
        bcm2835_spi_transfer(0xff);
	delayMicroseconds(12);
        temp=bcm2835_spi_transfer(0xff);        

        delayMicroseconds(12);
bcm2835_gpio_write(PIN, HIGH);
        delayMicroseconds(12);
      return temp;
}





