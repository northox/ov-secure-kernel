drivers-common-objs-y= gic.o
drivers-common-objs-y+= uart_driver.o
#drivers-common-objs-y+= sp804_sleep_timer.o
drivers-common-objs-$(CONFIG_CACHE_L2X0) += cache-l2x0.o

