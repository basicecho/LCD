#ifndef __LCD_THREAD__
#define __LCD_THREAD__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "lcd_draw.h"
#include "lcd_shop.h"

#define REFRESH_INTERVAL 1 * 1000 * 1000

#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 500

#define QUIET_TIME 10

void *lcd_refresh_time(void *arg);
void *lcd_touch_screen(void *arg);
void *lcd_play_adve(void *arg);
void *lcd_play_small_adve(void *arg);


#endif