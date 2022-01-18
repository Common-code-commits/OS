/* 关于绘图部分的处理 */

#include "bootpack.h"

int palette_head       = PALETTE_INIT_SIZE;
int font_color_index   = FONT_COLOR;
int fill_color_index   = FILL_COLOR;
int border_color_index = BORDER_COLOR;
int screen_color_index = SCREEN_COLOR;

BOOTINFO *binfo = (BOOTINFO *) ADR_BOOTINFO;   /* 屏幕信息变量赋值 */
unsigned char *vram;                           /* 创建地址变量    */

SHEET  *sht_back, *sht_mouse;
unsigned char *buf_back, buf_mouse[256];
SCREEN *screen;

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	io_cli(); 					/* 将中断许可标志置为0,禁止中断 */
	io_out8(0x03c8, start);

	/* 根据VGA显卡要求，R、G、B三种颜色的值时只能用低6位来指定 */
	/* 除四操作就是算数右移两位，由于颜色分量为unsigned char，算数右移和逻辑右移是一样的 */
    /* 将颜色存入调色板 */
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4); 
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}

	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}

void init_palette(void)
{
	/* 设置调色盘默认颜色 */
	static unsigned char table_rgb[PALETTE_INIT_SIZE * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, PALETTE_INIT_SIZE - 1 , table_rgb);
	return;
	/* C语言中的static char语句只能用于数据，相当于汇编中的DB指令 */
}

void set_color(int *index,RGB *rgb)
{
	unsigned char temp_color[3];

	if ( rgb )
	{
		temp_color[0] = rgb->Red;
		temp_color[0] = rgb->Blue;
		temp_color[0] = rgb->Green;		

		if ( palette_head < PALETTE_SIZE - 1)
		{
			*index = ++palette_head;
		}
		else
		{
			palette_head = PALETTE_INIT_SIZE;
			*index = palette_head;
		}
		set_palette(*index,*index,temp_color);
	}

}

/* 设置填充颜色 */
void set_fill_color(RGB *rgb)
{
	set_color(&fill_color_index,rgb);
}

/* 设置边框颜色 */
void set_border_color(RGB *rgb)
{
	set_color(&border_color_index,rgb);
}

/* 设置字体颜色 */
void set_font_color(RGB *rgb)
{
	set_color(&font_color_index,rgb);
}

/* 设置屏幕背景颜色 */
void set_screen_color(RGB *rgb)
{
	set_color(&screen_color_index,rgb);
}

void set_fill_color_index(int index)
{
	fill_color_index = index;
}

void set_font_color_index(int index)
{
	font_color_index = index;
}

/* 画填充矩形 */
void fillrectangle(int x0, int y0, int x1, int y1)
{

	int x, y;
	int xsize = screen->xsize;
	/* 根据 0xa0000 + x + y * width 计算坐标 */
	for (y = y0; y < y1; y++) {
		for (x = x0; x < x1; x++)
			screen->back[y * xsize + x] = fill_color_index;
			// vram[y * xsize + x] = fill_color_index;
	}
	sheet_refresh(sht_back,x0,y0,x1,y1);
	return;
}

void init_screen(RGB *rgb)
{
	int x = 0;
	int y = binfo->scrny;
	int xsize = binfo->scrnx;
	vram = binfo->vram;

	init_palette(); /* 设定调色板 */

	set_screen_color(rgb);

	shtctl_init(vram,xsize,y);                                   /* 创建图层缓冲区表 */

	static SCREEN screen_alloc;
	screen = &screen_alloc;

	screen->xsize = xsize;
	screen->ysize = y;

	/* 背景色号99 */
	sht_back  = sheet_alloc();                                   /* 申请背景缓冲区结构 */
	sht_mouse = sheet_alloc();                                   /* 申请鼠标缓冲区结构 */
	buf_back  = (unsigned char *) memman_alloc_4k(xsize * y);    /* 开辟背景缓冲区     */       

	/* 透明色号-1 */
	sheet_setbuf(sht_back, buf_back, xsize, y);    
	sheet_setbuf(sht_mouse, buf_mouse, MOUSE_SIZE, MOUSE_SIZE);     

	/* 当前窗口背景缓冲区就是屏幕背景缓冲区 */ 
	screen->back = buf_back;
	
	for(y--;y>=0;y--)
	{
		for(x=xsize-1;x>=0;x--)
		{
			screen->back[ y * xsize + x ] = screen_color_index; 
		}
	}

	sheet_slide(sht_back,0,0);

	init_mouse_cursor();
	RECT *rect = get_screen_src();
	int px0 = ( rect->width  - MOUSE_SIZE ) / 2;     /* 按显示在画面中央来计算坐标 */
	int py0 = ( rect->height - MOUSE_SIZE ) / 2;
	sheet_slide(sht_mouse, px0, py0);
	sheet_updown(sht_back,  0);
	sheet_updown(sht_mouse, 1);
	return;
}

void putfont( int x, int y,char *font)
{
	int xsize = screen->xsize;
	char *p, d ;
	int i;
	for (i = 0; i < FONT_SIZE ; i++) {
		p = screen->back + (y + i) * xsize + x;
		// p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = font_color_index; }
		if ((d & 0x40) != 0) { p[1] = font_color_index; }
		if ((d & 0x20) != 0) { p[2] = font_color_index; }
		if ((d & 0x10) != 0) { p[3] = font_color_index; }
		if ((d & 0x08) != 0) { p[4] = font_color_index; }
		if ((d & 0x04) != 0) { p[5] = font_color_index; }
		if ((d & 0x02) != 0) { p[6] = font_color_index; }
		if ((d & 0x01) != 0) { p[7] = font_color_index; }
	}
	return;
}

void putfonts8_asc(int x, int y, unsigned char *s)
{
	extern char hankaku[4096];
	int x0 = x;
	/* C语言中，字符串都是以0x00结尾 */
	for (; *s != 0x00; s++) {
		putfont(x, y,hankaku + *s * FONT_SIZE);
		x += 8;
	}
	sheet_refresh(sht_back,x0,y,x,y + FONT_SIZE);
	return;
}

void init_mouse_cursor()
/* 准备鼠标光标（16x16） */
{
	char cursor[MOUSE_SIZE][MOUSE_SIZE] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < MOUSE_SIZE; y++) {
		for (x = 0; x < MOUSE_SIZE; x++) {
			switch (cursor[y][x])
			{
				case '*':
					buf_mouse[y * MOUSE_SIZE + x] = MOUSE_BORDER_COLOR;
					break;
				case 'O':
					buf_mouse[y * MOUSE_SIZE + x] = MOUSE_FILL_COLOR;
					break;
				default:
					buf_mouse[y * MOUSE_SIZE + x] = TRANSPARENT_COLOR;
					break;
			}
		}
	}

	return;
}

RECT* get_screen_src(void)
{
	static RECT rect;
	rect.width = binfo->scrnx;
	rect.height = binfo->scrny;
	return &rect;
}

void refresh_mouse(int mx,int my)
{
	sheet_slide(sht_mouse, mx,my);
}

int get_key_leds(void)
{
	return ( binfo->leds >> 4 ) & 7;
}

void buf_switch(unsigned char *buf,int xsize,int ysize)
{
	screen->back = buf;
	screen->xsize = xsize;
	screen->ysize = ysize;
}

void ret_buf()
{
	screen->back = buf_back;
	screen->xsize = binfo->scrnx;
	screen->ysize = binfo->scrny;
}