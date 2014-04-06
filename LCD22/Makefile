main:main.o lcd_22.o
	gcc -o main main.o lcd_22.o -lbcm2835
mian.o:main.c lcd_22.h qqp_arm.h
	gcc -c main.c -lbcm2835
lcd_22.o:lcd_22.c lcd_22.h asciihex8X16.h
	gcc -c lcd_22.c -lbcm2835
