app-objs-y = otzapp.o
app1-objs-y = otz_tee_app.o
app2-objs-y = otz_boot_ack.o
app3-objs-y = otz_virtual_keyboard.o
app4-objs-y = otz_play_media.o
app5-objs-$(CONFIG_NEON_SUPPORT) = otz_neon_app.o
