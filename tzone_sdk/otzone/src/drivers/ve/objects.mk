drivers-objs-y= pl011_uart.o
drivers-objs-y+= sp804_timer.o
drivers-objs-y+= board.o
drivers-objs-$(CONFIG_GUI_SUPPORT)+= sw_fb.o


