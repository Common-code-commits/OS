/* FIFO */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init( FIFO32 *fifo, int size, int *buf)
/* FIFO缓冲区的初始化*/
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /*空*/
	fifo->flags = 0;
	fifo->p = 0; /*写入位置*/
	fifo->q = 0; /*读取位置*/
	return;
}

int fifo32_put( FIFO32 *fifo, int data)
/*向FIFO写入数据并累积起来*/
{
	if (fifo->free == 0) {
		/*没有空余空间，溢出*/
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo32_get( FIFO32 *fifo)
/*从FIFO取得一个数据*/
{
	int data;
	if (fifo->free == fifo->size) {
	/*当缓冲区为空的情况下返回-1*/
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status( FIFO32 *fifo)
/*报告已经存储了多少数据*/
{
	return fifo->size - fifo->free;
}

int get_status()
{
	return get_keyboard_status() + get_mouse_status();
}