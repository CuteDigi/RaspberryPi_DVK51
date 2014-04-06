 /*
*========================================================================================================
*
* File                : main.c
* By                  : jack ben
*
*                                  (c) Copyright 2011-2016, WaveShare
*                                       http://www.waveShare.net
*                                          All Rights Reserved
*
*========================================================================================================
*/

#include "lcd_22.h"
unsigned int GetTouchADC (unsigned char data)
{
        u8 tmp1,tmp2;
        u16  tmp;
        dis_lcd();
        en_touch();
	delayMicroseconds(2);
        bcm2835_spi_transfer(data);
	delayMicroseconds(2);
        tmp1=bcm2835_spi_transfer(0x00);
        tmp2=bcm2835_spi_transfer(0x00);
        tmp=((u16)(tmp1)<<5 | (u16)(tmp2)>>3 );
	dis_touch();
        return (tmp);
/*
        unsigned int NUMH , NUML;
        unsigned int Num;
        dis_lcd();//LCD_CS_H()
        en_touch();//TOUCH_nCS_L();

        delayMicroseconds(2);
        bcm2835_spi_transfer(data);
        delayMicroseconds(2);              // ÑÓÊ±µÈ´ý×ª»»Íê³É
        NUMH=bcm2835_spi_transfer(0x00);
        NUML=bcm2835_spi_transfer(0x00);
        Num=((NUMH)<<8)+NUML;
        Num>>=4;                // Ö»ÓÐ¸ß12Î»ÓÐÐ§.
        dis_touch();//TOUCH_nCS_H();

        return(Num);
*/
}
 typedef struct
{
        unsigned int  x;//LCD×ø±ê
        unsigned int  y;
        unsigned long x_ad_val; //ADCÖµ
        unsigned long y_ad_val;
        unsigned char  pen_status;//±ÊµÄ×´Ì¬
}_touch_dot;


_touch_dot touch_dot,*p_touch_dot;

#define READ_TIMES 10 //
#define LOST_VAL 4        //

unsigned int GetTouchADCEx(unsigned char cmd_code)
{
        unsigned int i, j;
        unsigned int buf[READ_TIMES];
        unsigned int sum=0;
        unsigned int temp;

        for(i=0;i<READ_TIMES;i++)
        {
                buf[i]=GetTouchADC(cmd_code);
        }
        for(i=0;i<READ_TIMES-1; i++)//ÅÅÐò
        {
                for(j=i+1;j<READ_TIMES;j++)
                {
                        if(buf[i]>buf[j])//ÉýÐòÅÅÁÐ
                        {
                                temp=buf[i];
                                buf[i]=buf[j];
                                buf[j]=temp;
                        }
                }
        }
        sum=0;
        for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
        temp=sum/(READ_TIMES-2*LOST_VAL);
        return temp;
}

unsigned char Read_ADS(unsigned int *x_ad,unsigned int *y_ad)
{
        unsigned int xtemp,ytemp;
        xtemp=GetTouchADCEx(TOUCH_CMD_X);    //ÓÐÉ¸Ñ¡µÄ¶ÁÈ¡XÖáAD×ª»»½á¹û
        ytemp=GetTouchADCEx(TOUCH_CMD_Y);            //ÓÐÉ¸Ñ¡µÄ¶ÁÈ¡YÖáAD×ª»»½á¹û           
        if(xtemp<100||ytemp<100)
        return 1;   //¶ÁÊýÊ§°Ü
        *x_ad=xtemp;
        *y_ad=ytemp;
        return 0;//¶ÁÊý³É¹¦
}

#define ERR_RANGE 50 //Îó²î·¶Î§

unsigned char Read_ADS2(unsigned long *x_ad,unsigned long *y_ad)
{
        unsigned int x1,y1;
        unsigned int x2,y2;
        unsigned char res;

    res=Read_ADS(&x1,&y1);  // µÚÒ»´Î¶ÁÈ¡ADCÖµ
    if(res==1)  return(1);      // Èç¹û¶ÁÊýÊ§°Ü£¬·µ»Ø1
    res=Read_ADS(&x2,&y2);      // µÚ¶þ´Î¶ÁÈ¡ADCÖµ
    if(res==1)  return(1);      // Èç¹û¶ÁÊýÊ§°Ü£¬·µ»Ø1
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//Ç°ºóÁ½´Î²ÉÑùÔÚ+-50ÄÚ
        &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x_ad=(x1+x2)/2;
        *y_ad=(y1+y2)/2;
        return 0;        // ÕýÈ·¶ÁÈ¡£¬·µ»Ø0
    }
    else return 1;       // Ç°ºó²»ÔÚ+-50ÄÚ£¬¶ÁÊý´íÎó
}

void convert_ad_to_xy(void)
{
      touch_dot.x=((touch_dot.x_ad_val-121)/13.030); // °Ñ¶Áµ½µÄX_ADCÖµ×ª»»³ÉTFT X×ø±êÖµ
      touch_dot.y=((touch_dot.y_ad_val-101)/10.500); // °Ñ¶Áµ½µÄY_ADCÖµ×ª»»³ÉTFT Y×ø±êÖµ
}

unsigned char Read_Once(void)
{
//      touch_dot.pen_status=Pen_Up;
        if(Read_ADS2(&touch_dot.x_ad_val,&touch_dot.y_ad_val)==0)       // Èç¹û¶ÁÈ¡Êý¾Ý³É¹¦
        {
                while(((bcm2835_gpio_lev))==0);   // ¼ì²â±ÊÊÇ²»ÊÇ»¹ÔÚÆÁÉÏ:IRQÎªµÍµçÆ½(bit7Îª0)ËµÃ÷±ÊÔÚÆÁÉÏ
                convert_ad_to_xy();   // °Ñ¶Áµ½µÄADCÖµ×ª±ä³ÉTFT×ø±êÖµ
                return 0;       // ·µ»Ø0£¬±íÊ¾³É¹¦
        }
        else return 1;  // Èç¹û¶ÁÈ¡Êý¾ÝÊ§°Ü£¬·µ»Ø1±íÊ¾Ê§°Ü
}

unsigned char Read_Continue(void)
{
//      touch_dot.pen_status=Pen_Up;
        if(Read_ADS2( &touch_dot.x_ad_val,&touch_dot.y_ad_val )==0)      // Èç¹û¶ÁÈ¡Êý¾Ý³É¹¦
        {
                convert_ad_to_xy();   // °Ñ¶Áµ½µÄADCÖµ×ª±ä³ÉTFT×ø±êÖµ
                return 0;          // ·µ»Ø0£¬±íÊ¾³É¹¦
        }
        else return 1;     // Èç¹û¶ÁÈ¡Êý¾ÝÊ§°Ü£¬·µ»Ø1±íÊ¾Ê§°Ü
}
void LCD_WRITE_COMMAND(unsigned int index,unsigned int data)
{
        //select command register
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(index>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        //send data
        en_lcd_data();//LCD_RS_H();

        en_lcd()//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
        dis_lcd();//LCD_CS_H();
}
void LCD_WRITE_REG(unsigned int index)
{
        unsigned int value_index;
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
        value_index=index;
        value_index =value_index>>8;
    bcm2835_spi_transfer((unsigned char)(value_index));    //00000000 000000000

        value_index=index;
        value_index &=0x00ff;
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        en_lcd_data();//LCD_RS_H();
}
void LCD_WRITE_DATA(unsigned int data)
{
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
}
void LCD_SEND_COMMAND(unsigned int index,unsigned int data)
{
        //select command register
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(index>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        //send data
        en_lcd_data();//LCD_RS_H();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
        dis_lcd();//LCD_CS_H();
}

void lcd_draw_dot(unsigned int color_front,
                  unsigned char x,
                  unsigned int y)
{

        LCD_SEND_COMMAND(0x210,x);
        LCD_SEND_COMMAND(0x212,y);
        LCD_SEND_COMMAND(0x211,x+1);
        LCD_SEND_COMMAND(0x213,y+1);


        en_lcd_index();//LCD_RS_L();
        LCD_WRITE_REG(0x202);   //RAM Write index
        en_lcd();//LCD_CS_L();
        LCD_WRITE_DATA(color_front);


}
void lcd_draw_bigdot(unsigned int   color_front,
                     unsigned int    x,
                     unsigned int    y )
{
    lcd_draw_dot(color_front,x,y);
    lcd_draw_dot(color_front,x,y+1);
    lcd_draw_dot(color_front,x,y-1);

    lcd_draw_dot(color_front,x+1,y);
    lcd_draw_dot(color_front,x+1,y+1);
    lcd_draw_dot(color_front,x+1,y-1);

    lcd_draw_dot(color_front,x-1,y);
    lcd_draw_dot(color_front,x-1,y+1);
    lcd_draw_dot(color_front,x-1,y-1);

}

unsigned char read_res ;

void display_touch_debug()
{        //ADC results show
        DisplayString("READ SUCCESS:",0,0xffff,1,3);
        lcd_display_number(14,3,read_res,6);

        DisplayString("X AD Val:",0,0xffff,1,4);
        lcd_display_number(10,4,touch_dot.x_ad_val,6);

        DisplayString("Y AD Val:",0,0xffff,1,5);
        lcd_display_number(10,5,touch_dot.y_ad_val,6);

        //Display coordinates
        DisplayString("X:",0,0xffff,1,6);
        lcd_display_number(10,6,touch_dot.x,5);
        DisplayString("Y:",0,0xffff,1,7);
        lcd_display_number(10,7,touch_dot.y,5);

}

void main(void)
{
        u16 num;
        u16 *p;
        u8 c,g;

	init_lcd();

	LCD_test();
	DisplayString("WaveShare",6,9,0);
	DisplayString("WaveShare",6,10,1);
	while(1){
//	display_touch_debug();		
	read_res=Read_Continue();
	lcd_draw_bigdot(0xffe0,(touch_dot.x-20),(touch_dot.y-2));
	}
}
