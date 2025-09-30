/**
 * @file ringbuffer.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-30
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// 循环缓冲结构体（用户无需修改）
typedef struct
{
	uint8_t *buffer;	  // 数据存储缓冲区（静态数组）
	uint16_t buffer_size; // 缓冲区大小（建议为2的幂，如64/128/256）
	uint16_t head;		  // 写入指针（下一个待写入位置）
	uint16_t tail;		  // 读取指针（下一个待读取位置）
	bool overflow_flag;	  // 溢出标志（1：发生溢出）
} RingBuffer;

/**
 * @brief 初始化循环缓冲
 * @param rb：循环缓冲结构体指针
 * @param buf：用户提供的静态数组（用于存储数据）
 * @param size：数组大小（需>0，建议为2的幂）
 */
void ring_buffer_init(RingBuffer *rb, uint8_t *buf, uint16_t size);

/**
 * @brief 向缓冲中写入1字节（中断安全）
 * @param rb：循环缓冲结构体指针
 * @param data：待写入的数据
 * @return 成功：true；失败（缓冲满）：false
 */
bool ring_buffer_write(RingBuffer *rb, uint8_t data);

/**
 * @brief 从缓冲中读取1字节（中断安全）
 * @param rb：循环缓冲结构体指针
 * @param data：读取到的数据（输出参数）
 * @return 成功：true；失败（缓冲空）：false
 */
bool ring_buffer_read(RingBuffer *rb, uint8_t *data);

/**
 * @brief 检查缓冲是否为空
 * @param rb：循环缓冲结构体指针
 * @return 空：true；非空：false
 */
bool ring_buffer_is_empty(RingBuffer *rb);

/**
 * @brief 检查缓冲是否已满
 * @param rb：循环缓冲结构体指针
 * @return 满：true；未满：false
 */
bool ring_buffer_is_full(RingBuffer *rb);

/**
 * @brief 获取缓冲中已使用的字节数
 * @param rb：循环缓冲结构体指针
 * @return 已使用字节数
 */
uint16_t ring_buffer_used(RingBuffer *rb);

/**
 * @brief 清空缓冲（重置指针）
 * @param rb：循环缓冲结构体指针
 */
void ring_buffer_clear(RingBuffer *rb);

/**
 * @brief 检查是否发生溢出
 * @param rb：循环缓冲结构体指针
 * @return 溢出：true；未溢出：false
 */
bool ring_buffer_overflow_occurred(RingBuffer *rb);

/**
 * @brief 清除溢出标志
 * @param rb：循环缓冲结构体指针
 */
void ring_buffer_clear_overflow(RingBuffer *rb);

#endif // RING_BUFFER_H
#endif // !__RINGBUFFER_H__