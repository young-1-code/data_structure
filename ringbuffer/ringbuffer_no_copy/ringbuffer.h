/**
 * 简介：测试循环buffer
 * 时间：2023/9/14
 * 作者：小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//循环buffer结构体
typedef struct ringbuffer_t
{
    uint8_t     *buffer;                                        //缓冲区
    int         buffer_size;                                    //缓冲区大小
    int         reserve_size;                                   //预留空间大小
    volatile    int rpos;                                       //读指针
    volatile    int wpos;                                       //写指针
    int      (*rb_write)(struct ringbuffer_t *rb, uint8_t *buffer, int len);//写数据函数指针
    uint8_t* (*rb_read)(struct ringbuffer_t *rb, int len);      //读数据函数指针
    void     (*rb_refreash)(struct ringbuffer_t *rb, int len);  //刷新读指针位置
    void     (*rb_free)(struct ringbuffer_t *rb);               //释放内存
}ringbuffer_t;

ringbuffer_t* create_ringbuffer(int size, int reserve_size);

#endif // !__RINGBUFFER_H__
