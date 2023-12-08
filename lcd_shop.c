#include "lcd_shop.h"

int product_x0 = 20;
int product_y0 = 100;
int column = 3;

double bill = 0;

char drink[6][40] = {
    "jun/drink/chapai.bmp",
    "jun/drink/hongcha.bmp",
    "jun/drink/kele.bmp",
    "jun/drink/wanglaoji.bmp",
    "jun/drink/xuebi.bmp",
    "jun/drink/xueli.bmp"
};


void lcd_product_init(struct Product *product, char *name_, int x0_, int y0_, 
                      int width_, int height_)
{
    product->name   = name_;
    product->x0     = x0_;
    product->y0     = y0_;
    product->width  = width_;
    product->height = height_;
    product->price  = 99;

    product->count      = 0;
    product->add_x0     = product->x0;
    product->add_y0     = product->y0 + product->height + 20;
    product->delete_x0  = product->add_x0 + 100;
    product->delete_y0  = product->add_y0;
    product->btn_width  = 40;
    product->btn_height = 40;
}


int lcd_draw_product(struct Product *product)
{
    int ret;

    ret = lcd_draw_bmp(product->name, product->width, product->height,
                       product->x0, product->y0);


    if(ret >= 0)
        ret = lcd_draw_number_double(product->price, product->x0 + 43,
                                     product->y0 + product->height, BLACK);
    
    lcd_draw_color(product->add_x0, product->add_y0, product->btn_width,
                   product->btn_height, BLACK);
    if(ret >= 0)
        ret = lcd_draw_word(w_add, sizeof(w_add), 16, product->add_x0 + 10,
                            product->add_y0 + 5, GREEN);

    int count_x0 = product->add_x0 + product->btn_width + 30;
    int count_y0 = product->add_y0 + 13;
    int disx = product->delete_x0 - count_x0 + 10;
    lcd_draw_color(count_x0 - 20, count_y0, disx, 40, WHITE);

    if(ret >= 0)
        ret = lcd_draw_number(product->count, count_x0,
                              count_y0, RED);

    
    lcd_draw_color(product->delete_x0, product->delete_y0, product->btn_width,
                   product->btn_height, BLACK);    
    if(ret >= 0)
        ret = lcd_draw_word(w_delete, sizeof(w_delete), 16, product->delete_x0 + 10,
                            product->delete_y0 + 5, GREEN);

    if(ret < 0) {
        fprintf(stderr, "lcd_draw_product failed\n");
        return -1;
    }
    
    return 0;
}


int lcd_click_up(struct Product *product)
{
    return 0;
}


int lcd_click_down(struct Product *product)
{
    return 0;
}


int lcd_click_pay()
{
    return 0;
}