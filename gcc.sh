. /opt/fsl-imx-x11/4.1.15-2.0.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi

$CC main.c \
    lcd_draw.c \
    lcd_draw.h \
    lcd_char.c \
    lcd_char.h \
    lcd_shop.c \
    lcd_shop.h \
    lcd_thread.c \
    lcd_thread.h \
-pthread -o main
