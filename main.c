#include "lcd_draw.h"
#include "lcd_shop.h"
#include "lcd_thread.h"

int reflesh_flag = -1;
int recover = 0;
int is_adve = 0;

time_t pre_time, cur_time;

struct Product products[PRODUCT_NUMBER];
struct Button btn[PRODUCT_NUMBER * 2 + 1];
struct Button small_adve;

pthread_mutex_t myMutex1 = PTHREAD_MUTEX_INITIALIZER;

//把整个界面刷新一遍
void flush_all() {
    pthread_mutex_lock(&myMutex1);

    lcd_draw_color(0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE);
    lcd_draw_name();
    for(int i = 0; i < PRODUCT_NUMBER; i++)
        lcd_draw_product(products + i);
    lcd_draw_pay(btn + PRODUCT_NUMBER * 2);

    pthread_mutex_unlock(&myMutex1);
}

int main(int argc, char **argv)
{
    if(lcd_draw_init() < 0) {
        fprintf(stderr, "lcd_draw_init failed\n");
        return -1;
    }

    //开机动画
    lcd_draw_boot("jun/boot.bmp");
    
    //组员名字
    lcd_draw_name();
    
    for(int i = 0; i < PRODUCT_NUMBER; i++) {
        int x0 = product_x0 + (i % column) * 150;
        int y0 = product_y0 + (i / column) * 200;
        lcd_product_init(products + i, drink[i], x0, y0, 128, 128);//先初化图片  
    }

    products[CHAPAI].price = 5;
    products[HONGCHA].price = 3;
    products[KELE].price = 2.5;
    products[WANGLAOJIE].price = 4;
    products[XUEBI].price = 3;
    products[XUELI].price = 3;

    //显示商品图片
    for(int i = 0; i < PRODUCT_NUMBER; i++)
        lcd_draw_product(products + i);

    //初始化+ - 按钮
    for(int i = 0; i < PRODUCT_NUMBER * 2; i += 2) {
        lcd_button_init(btn + i, products[i / 2].add_x0, products[i / 2].add_y0,
                        products[i / 2].btn_width, products[i / 2].btn_height);
        lcd_button_init(btn + i + 1, products[i / 2].delete_x0, products[i / 2].delete_y0,
                        products[i / 2].btn_width, products[i / 2].btn_height);
    }

    for(int i = 0; i < PRODUCT_NUMBER; i++) {
        printf("%d - %d\n", products[i].delete_x0, products[i].delete_y0);
    }

    //初始化按钮位置
    lcd_button_init(btn + PRODUCT_NUMBER * 2, 200, 520, 100, 50);
    
    //显示付款按钮
    lcd_draw_pay(btn + PRODUCT_NUMBER * 2);

    
    time(&pre_time);//最后一次点击屏幕的时间
    time(&cur_time);//现在的时间

    // 线程创建
    // 时钟
    pthread_t time_id;
    if(pthread_create(&time_id, NULL, lcd_refresh_time, NULL) < 0) {
        fprintf(stderr, "create time thread failed\n");
        return -1;
    }    

    // 触摸
    pthread_t touch_id;
    if(pthread_create(&touch_id, NULL, lcd_touch_screen, btn) < 0) {
        fprintf(stderr, "create touch thread failed\n");
        return -1;
    }


    // 广告
    pthread_t adve_id;
    if(pthread_create(&adve_id, NULL, lcd_play_adve, btn) < 0) {
        fprintf(stderr, "create touch thread failed\n");
        return -1;
    }

    // 小广告
    lcd_button_init(&small_adve, 600, 50, 300, 500);
    pthread_t small_adve_id;
    if(pthread_create(&small_adve_id, NULL, lcd_play_small_adve, &small_adve) < 0) {
        fprintf(stderr, "create touch thread failed\n");
        return -1;
    }


    for(;;) {

        if(recover) {
            printf("recover\n");
            recover = 0;
            flush_all();
            flush_all();
            flush_all();
        }

        if(reflesh_flag == -1) {
            usleep(10 * 1000);
            continue;
        }

        if(reflesh_flag == PRODUCT_NUMBER * 2 + 1) { //结帐
            continue;
        }


        int product_id = reflesh_flag / 2;
        
        if(reflesh_flag & 1) {
            if(products[product_id].count > 0) {
                products[product_id].count --;
                bill -= products[product_id].price;
            }
        }
        else {
            if(products[product_id].count < 99) {
                products[product_id].count++;
                bill += products[product_id].price;
            }
        }

        reflesh_flag = -1;
        lcd_draw_product(products + product_id);
        lcd_draw_pay(btn + PRODUCT_NUMBER * 2);
    }

    if(pthread_join(time_id, NULL) < 0) {
        fprintf(stderr, "time thread join thread\n");
        return -1;
    }

    if(pthread_join(touch_id, NULL) < 0) {
        fprintf(stderr, "touch thread join thread\n");
        return -1;
    }

    if(pthread_join(adve_id, NULL) < 0) {
        fprintf(stderr, "touch thread join thread\n");
        return -1;
    }

    if(pthread_join(small_adve_id, NULL) < 0) {
        fprintf(stderr, "touch thread join thread\n");
        return -1;
    }



    return 0;
}