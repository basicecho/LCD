#ifndef __LCD_DRAW__
#define __LCD_DRAW__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h>

#include "lcd_char.h"
#include "lcd_shop.h"

#define LCD_WIDTH  1024
#define LCD_HEIGHT 600

#define ADVE_WIDTH  300
#define ADVE_HEIGHT 500

#define KUN_COUNT  594
#define ADVE_COUNT 32

#define WHITE 0xFFFF
#define BLACK 0x0000
#define RED   0xF800
#define GREEN 0x07E0
#define BLUE  0x001F

#define LCD_INIT_COLOR WHITE

#define lcd_draw_point(x,y,color) 	*(unsigned short *)(plcd + (x) + (y)*LCD_WIDTH) = color

extern unsigned short *plcd;

struct Button {
    int x0, y0;
    int width, height;
};

void lcd_button_init(struct Button *btn, int x0_, int y0_, int width_, int height_);
int lcd_button_check(struct Button *btn, int x, int y);

int lcd_draw_init();
int lcd_draw_color(int x0, int y0, int width, int height, int color);
int lcd_draw_bmp(const char *name, int w, int h, int x0, int y0);
int lcd_draw_word(char *ch, int len, int w, int x0, int y0, unsigned short color);
int lcd_draw_number(unsigned number, int x0, int y0, unsigned short color);
int lcd_draw_number_double(double number, int x0, int y0, unsigned short color);
int lcd_draw_boot(char* file);
int lcd_draw_name();
int lcd_draw_date();
int lcd_draw_pay(struct Button *btn);

int lcd_draw_kun();
int lcd_draw_adve();

#endif