/* asmhead.nas */
/* 
   如果存在结构体是被某个魔数地址带来的数据直接赋值，
   该结构体保存为操作系统信息
*/
#ifndef _BOOKPACK_H
#define _BOOKPACK_H

#define NULL ((void*)0)                        /* 自定义NULL NULL即为空地址0 */

typedef struct { 							   /* 0x0ff0-0x0fff */
	char cyls; 								  /* 启动区读磁盘读到此为止 */
	char leds; 								  /* 启动时键盘的LED的状态 */
	char vmode; 							  /* 显卡模式 */
	char reserve;
	short scrnx, scrny; 					  /* 画面分辨率 */
	char *vram;                               /* 显卡内存地址 */
}BOOTINFO;

#define ADR_BOOTINFO	0x00000ff0

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);

/* graphic.c */
typedef struct{
	unsigned char Red;	       // 颜色的红色部分
	unsigned char Green;	   // 颜色的绿色部分
	unsigned char Blue;	       // 颜色的蓝色部分
} RGB;

typedef struct{
	int width;                 
	int height;                
} RECT;

typedef struct{
	int xsize,ysize;
	unsigned char *back;
} SCREEN;

#define FILL_COLOR		     7      // 填充默认色号
#define BORDER_COLOR	     1      // 边框默认色号
#define FONT_COLOR		     0      // 字体默认色号
#define MOUSE_FILL_COLOR     7      // 鼠标填充默认色号
#define SCREEN_COLOR	     6      // 屏幕背景默认色号
#define MOUSE_BORDER_COLOR   0      // 鼠标边框默认色号
#define TRANSPARENT_COLOR    255    // 透明色号

#define MOUSE_SIZE           16     // 鼠标大小
#define FONT_SIZE            16     // 字体大小

#define PALETTE_SIZE         255    // 调色板大小
#define PALETTE_INIT_SIZE    16     // 调色板初始颜色数量

#define WHITE 7
#define DarkCyan 12
#define LightGrey 8
#define BLACK 0
#define DarkGrey 15

void set_fill_color(RGB *rgb);
void fillrectangle( int x0, int y0, int x1, int y1);
void putfont(int x, int y, char *font);
void putfonts8_asc(int x, int y,unsigned char *s);
void init_mouse_cursor();
void init_screen(RGB *rgb);
void set_border_color(RGB *rgb);
void set_font_color(RGB *rgb);
void set_screen_color(RGB *rgb);
RECT* get_screen_src(void);
void refresh_mouse(int mx,int my);
int get_key_leds();
void set_fill_color_index(int index);
void set_font_color_index(int index);
void buf_switch(unsigned char *buf,int xsize,int ysize);
void ret_buf();
/* dsctbl.c */

/* 屏幕信息结构体 */
typedef struct {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
} SEGMENT_DESCRIPTOR;

typedef struct{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
} GATE_DESCRIPTOR;

void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

/* int.c */
void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);
int get_keyboard_status(void);
int get_mouse_status(void);
int get_key(void);
int get_mouse(void);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

/* fifo.c */
typedef struct{
	int *buf;
	int p, q, size, free, flags;
} FIFO32;
void fifo32_init(FIFO32  *fifo, int size, int *buf);
int fifo32_put(FIFO32  *fifo, int data);
int fifo32_get(FIFO32  *fifo);
int fifo32_status(FIFO32  *fifo);
int get_status();

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

/* keyboard.c */
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(void);
void put_key(int data);
char key_decode(char data,char model);
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064
#define PORT_KEYSTA						0x0064
#define KEYSTA_SEND_NOTREADY	        0x02
#define KEYCMD_WRITE_MODE			    0x60
#define KBC_MODE					    0x47

/* mouse.c */
typedef struct {
	unsigned char phase;
	int buf[3];
	int x, y, btn;
} MOUSE_DEC;

#define KEYCMD_SENDTO_MOUSE     0xd4
#define MOUSECMD_ENABLE         0xf4
void inthandler2c(int *esp);
void enable_mouse();
RECT *mouse_decode();
void init_mouse(void);

/* memory.c */
#define MEMMAN_FREES 4090 /* 大约是32KB*/
#define MEMMAN_ADDR			0x003c0000

typedef struct { /* 可用信息 */
	unsigned int addr, size;
}FREEINFO ;

typedef struct { /* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	FREEINFO free[MEMMAN_FREES];
} MEMMAN;

unsigned int memtest(unsigned int start, unsigned int end);
void memman_init();
unsigned int memman_total();
unsigned int memman_alloc( unsigned int size);
int memman_free( unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k( unsigned int size);
int memman_free_4k( unsigned int addr, unsigned int size);

/* sheet.c */
#define MAX_SHEETS		256
#define SHEET_USE  1

typedef struct sheet{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, height, flags;
	struct sheet *next;
} SHEET ;

typedef struct {
	unsigned char *vram;
	int xsize, ysize, top;
	SHEET *sheets[MAX_SHEETS];
	SHEET sheets0[MAX_SHEETS];
} SHTCTL ;

void shtctl_init(unsigned char *vram, int xsize, int ysize);
SHEET *sheet_alloc();
void sheet_setbuf(SHEET *sht, unsigned char *buf, int xsize, int ysize);
void sheet_updown(SHEET *sht, int height);
void sheet_refresh(SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(SHEET *sht, int vx0, int vy0);
void sheet_free(SHEET *sht);
int get_top_level();

/* timer.c */
#define MAX_TIMER		500

typedef struct timer{
	struct timer *next;
	unsigned int timeout, flags;
	FIFO32 *fifo;
	int data;
} TIMER;

typedef struct {
	unsigned int count, next;
	TIMER *t0;
	TIMER timers0[MAX_TIMER];
} TIMERCTL;

extern  TIMERCTL timerctl;
void init_pit(void);
TIMER *timer_alloc(void);
void timer_free( TIMER *timer);
void timer_init( TIMER *timer,  FIFO32 *fifo, int data);
void timer_settime( TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);

/* mtask.c */
#define MAX_TASKS 1000	            /*最大任务数量*/
#define TASK_GDT0 3			        /*定义从GDT的几号开始分配给TSS */
#define MAX_TASKS_LV    100
#define MAX_TASKLEVELS  10
typedef struct {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
} TSS32;
typedef struct{
	int sel, flags;		 /* sel用来存放GDT的编号*/
	int level, priority; /* 优先级 */
	FIFO32 fifo;
	TSS32 tss;
} TASK;

typedef struct{
	int running; /*正在运行的任务数量*/
	int now; /*这个变量用来记录当前正在运行的是哪个任务*/
	TASK *tasks[MAX_TASKS_LV];
} TASKLEVEL;
typedef struct {
	int now_lv; /*现在活动中的LEVEL */
	char lv_change; /*在下次任务切换时是否需要改变LEVEL */
	TASKLEVEL level[MAX_TASKLEVELS];
	TASK tasks0[MAX_TASKS];
} TASKCTL;

extern TIMER *task_timer;
void task_init();
TASK *task_alloc(void);
void task_run(TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(TASK *task);
TASK *task_now();

/* window.c */
typedef struct{
   TASK *task;
   SHEET *sht;
}HANDLE;

void make_window(HANDLE *handle,int x,int y,int xsize, int ysize, char *title, char act);
void close_window(HANDLE *handle);
void set_top(HANDLE *window,int px0,int py0);
#endif
