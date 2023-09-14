/**
 * 简介：测试循环buffer
 * 时间：2023/3/1
 * 作者：小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/
#ifndef  __RINGBUFFER_H__
#define __RINGBUFFER_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//循环buffer结构体
typedef struct ringbuffer_t
{
    int (*read)(struct ringbuffer_t *rb, uint8_t *buffer, int len);    //读数据函数指针
    int (*write)(struct ringbuffer_t *rb, uint8_t *buffer, int len);   //写数据函数指针
    uint8_t     *buffer;                                        //缓冲区
    int         buffer_size;                                    //缓冲区大小
    uint8_t     is_init;                                        //是否初始化
    volatile    int rpos;                                       //读指针
    volatile    int wpos;                                       //写指针
}ringbuffer_t;

ringbuffer_t* creat_ringbuffer(int buf_size);                   //创建一个结构体
int destory_ringbuffer(ringbuffer_t* rb);                       //销毁申请的缓冲区


#endif // ! __RINGBUFFER_H__