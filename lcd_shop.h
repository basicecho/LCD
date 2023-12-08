#ifndef __LCD_SHOP__
#define __LCD_SHOP__

#include "lcd_draw.h"
#include "lcd_char.h"

extern double bill;

#define PRODUCT_NUMBER 6

enum DRINK {
    CHAPAI,
    HONGCHA,
    KELE,
    WANGLAOJIE,
    XUEBI,
    XUELI
};

extern char drink[6][40];

extern int product_x0;
extern int product_y0;
extern int column;

struct Product {
    char* name;
    int x0, y0;
    int width, height;
    double price;
    int count;

    int add_x0, add_y0;
    int delete_x0, delete_y0;
    int btn_width, btn_height;
};

extern struct Product products[PRODUCT_NUMBER];

void lcd_product_init(struct Product *product, char *name_, int x0_, int y0_, 
                      int width_, int height_);
int lcd_draw_product(struct Product *product);
int lcd_click_up(struct Product *product);
int lcd_click_down(struct Product *product);
int lcd_click_pay();


#endif