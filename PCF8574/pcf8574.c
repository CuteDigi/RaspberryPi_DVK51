 
/* 
             define from  wiringPi.h                     define from Board DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
                8/SDA | | 5V               ->                  SDA | | 5V
                9/SCL | | GND              ->                  SCL | | GND
                    7 | | 14/TX            ->                  IO7 | | TX
                  GND | | 15/RX            ->                  GND | | RX
                    0 | | 18               ->                  IO0 | | IO1
                    2 | | GND              ->                  IO2 | | GND
                    3 | | 23               ->                  IO3 | | IO4
                  VCC | | 24               ->                  VCC | | IO5
              MOSI/12 | | GND              ->                 MOSI | | GND
              MISO/13 | | 25               ->                 MISO | | IO6
               SCK/14 | | 8/CE0            ->                  SCK | | CE0
                  GND | | 9/CE19           ->                  GND | | CE1
*/



#include <stdio.h>
#include <string.h> 
#include <errno.h>

#include<wiringPi.h>
#include<wiringPiI2C.h>

int devid = 0x20;   //you cam use "i2cdetect" to find this devid
int fd;	// 

int main(){

if ((fd=wiringPiI2CSetup(devid)) < 0)
{
    fprintf (stderr, "Unable to open I2C device: %s\n", strerror (errno)) ;
    return 1 ;
}

if ((wiringPiI2CWrite(fd,0xff)) < 0)
{
    fprintf (stderr, "write badly %s\n", strerror (errno)) ;
    return 1 ;
}   
delay(500);

if ((wiringPiI2CWrite(fd,0)) < 0)
{
    fprintf (stderr, "write badly %s\n", strerror (errno)) ;
    return 1 ;
}

delay(500);

printf("read from I2C: %d\n",wiringPiI2CRead(fd)); 

}
