lib-objs-y = sw_debug.o
lib-objs-y+= sw_div.o
lib-objs-y+= sw_heap.o
lib-objs-y+= sw_mem_functions.o
lib-objs-y+= sw_string_functions.o
lib-objs-y+= sw_buddy.o
lib-objs-y+= sw_semaphores.o
lib-objs-y+= sw_timer_functions.o
lib-objs-y+= sw_wait.o
lib-objs-y+= libc_sos.o
lib-objs-y+= sw_user_mgmt.o
lib-objs-y+= otz_tee_arith_api.o
lib-objs-y+= otz_tee_mem_mgmt.o
lib-objs-y+= otz_tee_internal_storage_api.o
lib-objs-y+= sw_filelib.o
lib-objs-y+= sw_syslibc.o


lib-boot-objs-y+= sw_debug.o
lib-boot-objs-y+= sw_div.o
lib-boot-objs-y+= sw_mem_functions.o
lib-boot-objs-y+= sw_string_functions.o
