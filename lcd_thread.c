#include "lcd_thread.h"

extern int reflesh_flag;
extern time_t pre_time, cur_time;
extern int is_adve;
extern int recover;


void *lcd_refresh_time(void *arg)
{
    for(;;) {
        if(is_adve) {
            usleep(REFRESH_INTERVAL);
            continue;
        }
        usleep(REFRESH_INTERVAL);
        lcd_draw_date();
    }

    return NULL;
}

static void lcd_refresh_bill(struct Button *btns, int x, int y)
{
    printf("%d - %d\n", x, y);
    for(int i = 0; i < PRODUCT_NUMBER * 2 + 1; i++) {
        if(lcd_button_check(btns + i, x, y)) {
            reflesh_flag = i;
            printf("%d reflesh_flag is yes\n", i);
            break;
        }
    }
}

static void touch_map_piex(int *x, int *y)
{
    *x = TOUCH_WIDTH - 1 - *x;
    *y = TOUCH_HEIGHT - 1 - *y;
    *x = *x * LCD_WIDTH / TOUCH_WIDTH;
    *y = *y * LCD_HEIGHT / TOUCH_HEIGHT;
}

void *lcd_touch_screen(void *arg)
{
    int ret;
    struct Button *btns = (struct Button *)arg;
    

    for(int i = 0; i < PRODUCT_NUMBER * 2 + 1; i++) {
        printf("%d, %d, %d, %d\n", btns[i].x0, btns[i].y0, btns[i].width, btns[i].height);
    }

    int fd = open("/dev/input/touchscreen0", O_RDWR);
    if(-1 == fd) {
        perror("open /dev/input/touchscreen0 failed\n");
        exit(0); 
    }

    struct input_event event;
    int x1 = -1, y1 = -1;
    int x2 = -1, y2 = -1;

    for(;;) {
        read(fd, &event, sizeof(event));

        // 第一个坐标
        if(event.type == EV_ABS && event.code == ABS_X && x1 == -1) {
            x1 = event.value;
        }
        else if(event.type == EV_ABS && event.code == ABS_Y && y1 == -1) {
            y1 = event.value;
        }

        // 最后一个坐标
        if(event.type == EV_ABS && event.code == ABS_X) {
            x2 = event.value;
        }
        else if(event.type == EV_ABS && event.code == ABS_Y) {
            y2 = event.value;
        }

        // 松手
        if(event.type == EV_ABS && event.code == ABS_PRESSURE && event.value == 0) {
            // 无效点击
            if(x1 == -1 || y1 == -1) {
                x1 = y1 = x2 = y2 = -1;
                continue;
            }

            time(&pre_time);

            printf("is_adve = %d, recover = %d\n", is_adve, recover);

            // 进行切换
            if(is_adve) {
                is_adve = 0;
                recover = 1;
                continue;
            }

            

            if(x1 == x2 && y1 == y2) { // 点击
                touch_map_piex(&x1, &y1);
                lcd_refresh_bill(btns, x1, y1);
            }
            else {
                // printf("two\n");
                // printf("x1: %d - y1: %d\n", x1, y1);
                // printf("x2: %d - y2: %d\n", x2, y2);
            }
            x1 = y1 = x2 = y2 = -1;
        }

    }
}

void* lcd_play_adve(void *arg)
{
    for(int i = 0;;) {
        time(&cur_time);
        int seconds_diff = difftime(cur_time, pre_time);
        if(!is_adve && seconds_diff >= QUIET_TIME) {
            is_adve = 1;
            i = 0;
        }

        if(!is_adve) {
            usleep(100 * 1000);
            continue;
        }

		if(i == KUN_COUNT)i = 0;

        usleep(1000 * 10);
		char file[20] = {};
		snprintf(file, sizeof(file), "jun/kun/%03dkun.bmp", i + 1);
		// printf("%s\n", file);

		if(lcd_draw_bmp(file, LCD_WIDTH, LCD_HEIGHT, 0, 0) < 0) {
			fprintf(stderr, "lcd_draw_bmp error\n");
			return NULL;
		}
        i++;
	}

    return NULL;
}

void *lcd_play_small_adve(void *arg)
{
    struct Button *btn = arg; 

    

    for(int i = 0;;) {
        // 有大广告就休息
        if(is_adve) {
            usleep(100 * 1000);
            continue;
        }

        if(i == ADVE_COUNT)i = 0;

        usleep(1000 * 100);
		char file[20] = {};
		snprintf(file, sizeof(file), "jun/adve/%02d.bmp", i + 1);
		// printf("%s\n", file);

        if(lcd_draw_bmp(file, btn->width, btn->height, btn->x0, btn->y0) < 0) {
			fprintf(stderr, "lcd_draw_bmp error\n");
			return NULL;
		}
        
        i++;
    }
}