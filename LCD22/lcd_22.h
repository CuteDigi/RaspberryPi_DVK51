 /*
*========================================================================================================
*
* File                : lcd_22.h
* Hardware Environment:
* By                  : jack ben
*
*                                  (c) Copyright 2011-2016, WaveShare
*                                       http://www.waveShare.net
*                                          All Rights Reserved
*
*========================================================================================================
*/
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

#ifndef LCD_22_H
#define LCD_22_H

#include <bcm2835.h>
#include <stdio.h>

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int

#define LCD22_SPI
#define  PENIRQ			     RPI_GPIO_P1_15   //IN

#define en_touch() 			 {bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);}// T_CS 
#define dis_touch() 		 {bcm2835_gpio_write(RPI_GPIO_P1_16, HIGH);}

#define reset_clr			{bcm2835_gpio_write(RPI_GPIO_P1_22, LOW);}//RST    
#define reset_set			{bcm2835_gpio_write(RPI_GPIO_P1_22, HIGH);}//

#define en_lcd() 			  {bcm2835_gpio_write(RPI_GPIO_P1_24, LOW);}// CS
#define dis_lcd() 			 {bcm2835_gpio_write(RPI_GPIO_P1_24, HIGH);}//

#define en_lcd_data() 	     {bcm2835_gpio_write(RPI_GPIO_P1_26, HIGH);}// RS
#define en_lcd_index() 	     {bcm2835_gpio_write(RPI_GPIO_P1_26, LOW);}//


#define SAMP_COUNT 			5
#define SAMP_THRESHOLD 	     5

#define TOUCH_CMD_X 		0xD0
#define TOUCH_CMD_Y 		0x90

#define COLOR_YELLOW 		0xFFE0
#define COLOR_BLACK 		0x0000
#define COLOR_WHITE 		0xFFFF
#define COLOR_INIT 			COLOR_YELLOW

#define DOT_WIDTH 4

typedef struct xy
{
	u16 x;
	u16 y;
}xy_t;

#define TOUCH_MAX_CACHE 8
volatile xy_t touch_xy_buffer[TOUCH_MAX_CACHE];
volatile u8 touch_wr_index;
volatile u8 touch_rd_index;
volatile u8 touch_counter;
/**/
void init_lcd_spi(void);
void init_touch_spi(void);
void post_data(u16 data);
void post_cmd(u16 index, u16 cmd);
/**/
u16 get_touch_data(u8 cmd);
xy_t get_touch_xy(void);
u8 get_point_xy(void);
u8 draw_lcd(void);
void init_lcd(void);
void LCD_test(void);
void LCD_QQ();
//void DisplayString(u8 *s,u8 x,u8 y,u8 Reverse);
#endif
