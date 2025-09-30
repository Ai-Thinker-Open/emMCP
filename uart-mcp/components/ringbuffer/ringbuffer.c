/**
 * @file ringbuffer.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ringbuffer.h"
// 关闭/开启总中断的宏（需根据目标单片机修改）
// 示例：STC51用EA（总中断允许位），STM32用__disable_irq()/__enable_irq()
#ifdef STC51
#define INTERRUPT_DISABLE() \
	do                      \
	{                       \
		EA = 0;             \
	} while (0)
#define INTERRUPT_ENABLE() \
	do                     \
	{                      \
		EA = 1;            \
	} while (0)
#elif defined(STM32)
#include "stm32f1xx_hal.h"
#define INTERRUPT_DISABLE() \
	do                      \
	{                       \
		__disable_irq();    \
	} while (0)
#define INTERRUPT_ENABLE() \
	do                     \
	{                      \
		__enable_irq();    \
	} while (0)
#else
// 其他平台默认关闭中断（若不支持，可注释此宏，牺牲线程安全）
#define INTERRUPT_DISABLE()
#define INTERRUPT_ENABLE()
#endif

void ring_buffer_init(RingBuffer *rb, uint8_t *buf, uint16_t size)
{
	if (rb == NULL || buf == NULL || size == 0)
		return;
	rb->buffer = buf;
	rb->buffer_size = size;
	rb->head = 0;
	rb->tail = 0;
	rb->overflow_flag = false;
}

bool ring_buffer_write(RingBuffer *rb, uint8_t data)
{
	if (rb == NULL || rb->buffer == NULL)
		return false;

	INTERRUPT_DISABLE(); // 关闭中断，确保指针操作原子性

	// 计算下一个写入位置
	uint16_t next_head = (rb->head + 1) % rb->buffer_size;
	bool is_full = (next_head == rb->tail);

	if (!is_full)
	{
		rb->buffer[rb->head] = data; // 写入数据
		rb->head = next_head;		 // 更新写入指针
		INTERRUPT_ENABLE();
		return true;
	}
	else
	{
		rb->overflow_flag = true; // 标记溢出
		INTERRUPT_ENABLE();
		return false;
	}
}

bool ring_buffer_read(RingBuffer *rb, uint8_t *data)
{
	if (rb == NULL || data == NULL || rb->buffer == NULL)
		return false;

	INTERRUPT_DISABLE(); // 关闭中断，确保指针操作原子性

	bool is_empty = (rb->head == rb->tail);
	if (!is_empty)
	{
		*data = rb->buffer[rb->tail];				 // 读取数据
		rb->tail = (rb->tail + 1) % rb->buffer_size; // 更新读取指针
		INTERRUPT_ENABLE();
		return true;
	}

	INTERRUPT_ENABLE();
	return false;
}

bool ring_buffer_is_empty(RingBuffer *rb)
{
	if (rb == NULL)
		return true;
	return (rb->head == rb->tail);
}

bool ring_buffer_is_full(RingBuffer *rb)
{
	if (rb == NULL)
		return false;
	uint16_t next_head = (rb->head + 1) % rb->buffer_size;
	return (next_head == rb->tail);
}

uint16_t ring_buffer_used(RingBuffer *rb)
{
	if (rb == NULL)
		return 0;
	INTERRUPT_DISABLE();
	uint16_t used = (rb->head - rb->tail + rb->buffer_size) % rb->buffer_size;
	INTERRUPT_ENABLE();
	return used;
}

void ring_buffer_clear(RingBuffer *rb)
{
	if (rb == NULL)
		return;
	INTERRUPT_DISABLE();
	rb->head = 0;
	rb->tail = 0;
	INTERRUPT_ENABLE();
}

bool ring_buffer_overflow_occurred(RingBuffer *rb)
{
	if (rb == NULL)
		return false;
	return rb->overflow_flag;
}

void ring_buffer_clear_overflow(RingBuffer *rb)
{
	if (rb == NULL)
		return;
	rb->overflow_flag = false;
}