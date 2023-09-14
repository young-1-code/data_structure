/**
 * 简介：测试循环buffer
 * 时间：2023/3/1
 * 作者：小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/
#include "ringbuffer.h"

#define smp_mb() __asm__ __volatile__ ("" : : : "memory") //内存屏障
#define MIN(x, y) ((x)<(y) ? (x) : (y))

/**
 * @brief:从循环buffer中读取数据
 * @rb: 输入的循环buffer结构体
 * @buffer:存储读取到的数据buffer
 * @len:要读取的长度
 * @return: -1:失败 其他:读取到的长度
*/
int read_ringbuffer (ringbuffer_t *rb, uint8_t *buffer, int len)
{
    int left_len = 0;
    int buf_size = rb->buffer_size;
    int wpos = rb->wpos;
    int rpos = rb->rpos;
    int available_len = ((wpos-rpos)+buf_size)&(buf_size-1);
    
    if(available_len <= len) return -1;

    left_len = MIN(len, buf_size-rpos);
    memcpy(buffer, rb->buffer+rpos, left_len);
    memcpy(buffer+left_len, rb->buffer, len-left_len);

    smp_mb();

    rb->rpos += len;
    rb->rpos &= (buf_size-1);

    return len;
}

/**
 * @brief:向循环buffer中写入数据
 * @rb: 输入的循环buffer结构体
 * @buffer:写入循环buffer的数据
 * @len:要写入的长度
 * @return: -1:失败 其他:写入的长度
*/
int write_ringbuffer (ringbuffer_t *rb, uint8_t *buffer, int len)
{
    int left_len = 0;
    volatile int buf_size = rb->buffer_size;
    int wpos = rb->wpos;
    int rpos = rb->rpos;
    int available_len = ((rpos-wpos)+buf_size)&(buf_size-1);
    
    if(available_len <= len){
        if(!rb->is_init){
            rb->is_init = 1;
            goto run;
        }
        return -1;
    }

run:
    left_len = MIN(len, buf_size-wpos);
    memcpy(rb->buffer+wpos, buffer, left_len);
    memcpy(rb->buffer, buffer+left_len, len-left_len);

    smp_mb();

    rb->wpos += len;
    rb->wpos &= (buf_size-1);

    return len;
}

#undef smp_mb
#undef MIN

/**
 * @brief:创建一个循环buffer
 * @buf_size:循环buffer的空间大小
 * @return: NULL:失败  
*/
ringbuffer_t* creat_ringbuffer(int buf_size)
{
    if(buf_size <= 0) return NULL;

    ringbuffer_t* rb = (ringbuffer_t*)malloc(sizeof(ringbuffer_t));
    if(!rb){
        perror("error!");
        return NULL;
    }

    rb->buffer_size = 1;
    do{
        buf_size /= 2;
        rb->buffer_size *= 2;
    }while(buf_size); 
    printf("Round up buffer size=%d\n", rb->buffer_size);

    rb->buffer = (uint8_t*)malloc(rb->buffer_size);
    if(!rb->buffer){
        perror("error!");
        free(rb);
        return NULL;
    }

    rb->is_init = 0;
    rb->rpos = rb->wpos = 0;
    rb->read = read_ringbuffer;
    rb->write = write_ringbuffer;

    return rb;
}

/**
 * @brief:销毁循环buffer
 * @rb: 要销毁的循环buffer结构体
*/
int destory_ringbuffer(ringbuffer_t* rb)
{
    if(rb){
        if(rb->buffer) free(rb->buffer);
        free(rb);
    }
    rb=NULL;

    return 0;
}
