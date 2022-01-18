#include "bootpack.h"
#include <stdio.h>
#define KEYCMD_LED		0xed

void HariMain(void)
{
	char s[40];
	int mx, my, i;
	int keycmd_wait = -1;
	int memtotal;

	io_cli();
	init_gdtidt();
	init_pic();
	init_pit();
	io_sti();                                                    /* IDT/PIC/PIT的初始化已经完成，于是开放CPU的中断 */

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init();
	memman_free(0x00001000, 0x0009e000);                         /* 0x00001000 - 0x0009efff */
	memman_free(0x00400000, memtotal - 0x00400000);

	init_screen(NULL);
	init_keyboard();
	init_mouse();
	// task_init();

	HANDLE window;
	HANDLE window_1;
	make_window(&window,0,0,200,100,"window",1);
	make_window(&window_1,100,10,100,100,"window",1);
	// close_window(&window);
	// close_window(&window_1);
	/* sht_win */
	//make_window8(100,100,"win",'c');

	RECT *rect = get_screen_src();
	RECT *point;
	mx = ( rect->width  - MOUSE_SIZE ) / 2;     /* 按显示在画面中央来计算坐标 */
	my = ( rect->height - MOUSE_SIZE ) / 2;

	//ret_buf();
	for (;;) {
		if ( get_status() > 0 ) {
			io_cli();
			i = get_key();
			point = mouse_decode();
			io_sti();
			if ( point ) {
				/* 已经收集了3字节的数据，所以显示出来 */
				/* 移动光标 */
				mx += point->width;
				my += point->height;
				if (mx < 0) {
					mx = 0;
				}
				if (my < 0) {
					my = 0;
				}
				if (mx > rect->width - 1) {
					mx = rect->width - 1;
				}
				if (my > rect->height - 1) {
					my = rect->height - 1;
				}
				refresh_mouse(mx,my);
				// fillrectangle(0,0,100,100);	
				// sprintf(s,"%3d,%3d",mx,my);
				// putfonts8_asc(0,0,s);		
			}
		}
	}
}

