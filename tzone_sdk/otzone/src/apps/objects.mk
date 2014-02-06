apps-objs-y=dispatcher_task.o
apps-objs-y+=echo_task.o
apps-objs-y+=drm_task.o
apps-objs-y+=rc4_algorithm.o
apps-objs-y+=task_control.o
apps-objs-y+=crypto_task.o
apps-objs-y+=crypto_tests.o
apps-objs-y+=otz_tee_arith_test.o
apps-objs-$(CONFIG_GP_TEST) += gp_internal_api_test.o
apps-objs-$(CONFIG_GP_TEST) += test_storage_api.o
apps-objs-$(CONFIG_NEON_SUPPORT) += neon_app.o
apps-objs-y+=elf_loader_app.o

ifneq ($(CONFIG_SW_ELF_LOADER_SUPPORT), y)
apps-objs-$(CONFIG_GUI_SUPPORT)+=virtual_keyboard_task.o
apps-objs-y+=mutex_test_task.o
else
modules-objs-$(CONFIG_GUI_SUPPORT)=virtual_keyboard_task.o
modules-objs-y+=mutex_test_task.o
endif


