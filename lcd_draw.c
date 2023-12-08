#include "lcd_draw.h"
#include "lcd_shop.h"

unsigned short *plcd = NULL;


void lcd_button_init(struct Button *btn, int x0_, int y0_, int width_, int height_)
{
    btn->x0 = x0_;
    btn->y0 = y0_;
    btn->width = width_;
    btn->height = height_;
}


//检查是否点到这个按钮
int lcd_button_check(struct Button *btn, int x, int y)
{
	int x0 = btn->x0;
	int y0 = btn->y0;
	int x1 = btn->x0 + btn->width;
	int y1 = btn->y0 + btn->height;
	if(x > 0 && y > 0 &&
	   x >= x0 && x <= x1 &&
	   y >= y0 && y <= y1) {
		return 1;
	}

	return 0;
}

//初始化主界面为白色
int lcd_draw_init()
{
	int fd;
	fd = open("/dev/fb0", O_RDWR);
	if (-1 == fd) {
		perror("open /dev/fb0 ");
		return -1;
	}

	int x, y;
	unsigned short color = 0xF800;        
	
	plcd = mmap(NULL, LCD_WIDTH*LCD_HEIGHT*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	for (y = 0; y < LCD_HEIGHT; y++) {
		for (x = 0; x < LCD_WIDTH; x++)
			*(unsigned short *)(plcd + LCD_WIDTH*y + x) = LCD_INIT_COLOR;
	}	
	return 0;
}

//给背景区域上色
int lcd_draw_color(int x0, int y0, int width, int height, int color)
{
	for(int y = y0; y < y0 + height; y++)
		for(int x = x0; x < x0 + width; x++)
			*(unsigned short *)(plcd + LCD_WIDTH*y + x) = color;
	
	return 0;
}

//画图
int lcd_draw_bmp(const char *name, int w, int h, int x0, int y0)
{
	int fd;
	fd = open(name, O_RDWR);
	if (-1 == fd)
	{
		perror("open bmp file ");
		return -1;
	}

	char pixels[w*h*3];
	unsigned short color;
	int i = 0;

	lseek(fd, 54, SEEK_SET);
	read(fd, pixels, sizeof(pixels));

	int x, y;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			color = (pixels[i+2]>>3)<<11 | (pixels[i+1]>>2)<<5 | pixels[i]>>3;
			i += 3;
			lcd_draw_point(x+x0, h-1-y+y0, color);
		}
	}
	
	close(fd);
    return 0;
}

//写字
int lcd_draw_word(char *ch, int len, int w, int x0, int y0, unsigned short color)
{  
	int i, m, x, y;
	for (m = 0; m < len; m++)
	{
		for (i = 7; i >= 0; i--)
		{
			if (ch[m] & (1<<i))
			{
				x = (7-i)+8*(m%(w/8)) + x0;
				y = m/(w/8) + y0;
				lcd_draw_point(x,y,color);
			}
		}	
	}

    return 0;
}

//写数字
int lcd_draw_number(unsigned number, int x0, int y0, unsigned short color)
{
	int n, i = 0;
	if (number == 0)
	{
		lcd_draw_word(numbers[0],sizeof(numbers[0]),16, x0, y0, color);
		return 0;
	}
	
	while (number)
	{
		n = number % 10;
		lcd_draw_word(numbers[n],sizeof(numbers[n]),16, x0-18*i, y0, color);
		i++;
		number /= 10;
	}

	return 0;
}

//写带小数点的数字
int lcd_draw_number_double(double number, int x0, int y0, unsigned short color)
{
	char s[20] = {};
	snprintf(s, sizeof(s), "%.1lf", number);
	
	int dist = 0;
	for(int i = 0; s[i]; i++) {
		if(s[i] >= '0' && s[i] <= '9') {
			lcd_draw_word(numbers[s[i] - '0'], sizeof(numbers[s[i] - '0']), 16, x0 + dist, y0, color);
			dist += 18;
		}
		else {
			lcd_draw_word(w_point, sizeof(w_point), 16, x0 + dist, y0, color);
			dist += 9;
		}
			
	}

    return 0;
}

//
int lcd_draw_boot(char* file)
{
	//开机动画15行往下掉
    for(int line = 0; line < LCD_HEIGHT; line += 15) {
        lcd_draw_bmp(file, LCD_WIDTH, line, 0, 0);
    }

    // 商店界面为白色背景
    int x, y;
	for (y = 0; y < LCD_HEIGHT; y++) {
		for (x = 0; x < LCD_WIDTH; x++)
			*(unsigned short *)(plcd + LCD_WIDTH*y + x) = LCD_INIT_COLOR;
	}
}

//写组员名字
int lcd_draw_name()
{
	int ret;

	for(int i = 0; i < 13; i++) {
		int x0 = 10 + i * 32;
		int y0 = 10;
		ret = lcd_draw_word(w_name[i], sizeof(w_name[i]), 32, x0, y0, RED);
		if(ret < 0) {
			fprintf(stderr, "lcd_draw_word failed\n");
			return -1;
		}
	}
	return 0;
}

//显示时间
int lcd_draw_date()
{
	time_t currentTime;
    time(&currentTime);

	struct tm *localTime = localtime(&currentTime);

	int year   = localTime->tm_year + 1900;
    int month  = localTime->tm_mon + 1;
    int day    = localTime->tm_mday;
    int hour   = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

	int x0 = 700;
	int y0 = 10;
	int interval = 8;

	lcd_draw_color(x0, y0, 300, 30, WHITE);

	lcd_draw_number(year,   x0,                  y0, RED);
	lcd_draw_number(month,  x0 + 6   * interval, y0, RED);
	lcd_draw_number(day,    x0 + 12  * interval, y0, RED);
	lcd_draw_number(hour,   x0 + 18  * interval, y0, RED);
	lcd_draw_number(minute, x0 + 24  * interval, y0, RED);
	lcd_draw_number(second, x0 + 30  * interval, y0, RED);

	lcd_draw_word(w_xie,    sizeof(w_xie),    16, x0 + 2  * interval, y0, RED);
	lcd_draw_word(w_xie,    sizeof(w_xie),    16, x0 + 8  * interval, y0, RED);
	lcd_draw_word(w_maohao, sizeof(w_maohao), 16, x0 + 20 * interval, y0, RED);
	lcd_draw_word(w_maohao, sizeof(w_maohao), 16, x0 + 26 * interval, y0, RED);


	return 0;
}

//付款按钮
int lcd_draw_pay(struct Button *btn)
{
	int money_x0 = btn->x0 - 70;
	int money_y0 = btn->y0 + 10;

	lcd_draw_color(money_x0, money_y0, 70, 32, WHITE);
	lcd_draw_number_double(bill, money_x0, money_y0, BLACK);
	lcd_draw_color(btn->x0, btn->y0, btn->width, btn->height, BLACK);
	for(int i = 0; i < 2; i++)
		lcd_draw_word(w_pay[i], sizeof(w_pay[i]), 32, btn->x0 + 10 + i * 32, btn->y0 + 10, GREEN);
}

//播放广告
int lcd_draw_kun()
{
	for(int i = 0;;i++) {
		if(i == KUN_COUNT)i = 0;

		char file[20] = {};
		snprintf(file, sizeof(file), "jun/kun/%03dkun.bmp", i + 1);
		printf("%s\n", file);

		if(lcd_draw_bmp(file, LCD_WIDTH, LCD_HEIGHT, 0, 0) < 0) {
			fprintf(stderr, "lcd_draw_bmp error\n");
			return -1;
		}
	}

	return 0;
}

//主界面右边那个小广告
int lcd_draw_adve()
{
	for(int i = 0;;i++) {
		if(i == ADVE_COUNT)i = 0;

		char file[20] = {};
		snprintf(file, sizeof(file), "jun/adve/%02d.bmp", i + 1);
		printf("%s\n", file);

		if(lcd_draw_bmp(file, ADVE_WIDTH, ADVE_HEIGHT, 0, 0) < 0) {
			fprintf(stderr, "lcd_draw_bmp error\n");
			return -1;
		}

		usleep(1000 * 1000);
	}


	return 0;
}