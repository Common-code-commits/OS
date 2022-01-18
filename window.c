#include "bootpack.h"

void set_top(HANDLE *window,int px0,int py0)
{
	sheet_slide(window->sht, px0, py0);
	sheet_updown(window->sht,  get_top_level());	
}

void make_wtitle(unsigned char *buf,int xsize, char *title, char act)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;
	if (act != 0) {
		tc = WHITE;
		tbc = DarkCyan;
	} else {
		tc  = LightGrey;
		tbc = DarkGrey;
	}
	set_fill_color_index(tbc);
	set_font_color_index(tc);
	fillrectangle(0, 0, xsize, 20);
	putfonts8_asc(0, 0, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = BLACK;
			} else if (c == '$') {
				c = DarkGrey;
			} else if (c == 'Q') {
				c = LightGrey;
			} else {
				c = WHITE;
			}
			buf[ (5 + y) * xsize + (xsize - 21 + x) ] = c;
		}
	}
	return;
}

void make_textbox()
{
	set_fill_color_index(WHITE);
	fillrectangle(0,20,200,100);
	return;
}

void console_task( SHEET *sheet)
{
	TIMER *timer;
	TASK *task = task_now();
}

void make_window(HANDLE *handle,int x,int y,int xsize, int ysize, char *title, char act)
{
	SHEET *sht_cons = sheet_alloc();
    handle->sht = sht_cons;
	unsigned char *buf = (unsigned char *) memman_alloc_4k( xsize * ysize );
	sheet_setbuf(sht_cons, buf, xsize, ysize); 
	buf_switch(buf,xsize,ysize);
	make_textbox();
	// TASK *task_cons = task_alloc();
    // handle->task = task_cons;
	// task_cons->tss.esp = memman_alloc_4k(64 * 1024) + 64 * 1024 - 8;
	// task_cons->tss.eip = (int) &console_task;
	// task_cons->tss.es = 1 * 8;
	// task_cons->tss.cs = 2 * 8;
	// task_cons->tss.ss = 1 * 8;
	// task_cons->tss.ds = 1 * 8;
	// task_cons->tss.fs = 1 * 8;
	// task_cons->tss.gs = 1 * 8;
	// *((int *) (task_cons->tss.esp + 4)) = (int) sht_cons;
	// task_run(task_cons, 1, 1);                                               /* level=1, priority=1 */
	make_wtitle(buf, xsize, title, act);
	set_top(handle,x,y);
	return;
}

void close_window(HANDLE *handle)
{
    sheet_free(handle->sht);
	//task_remove(handle->task);
	ret_buf();
}