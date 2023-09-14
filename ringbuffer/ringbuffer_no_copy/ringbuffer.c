#include "ringbuffer.h"

#define _min(x, y) ((x) < (y)) ? (x) : (y)

#define smp_mb() __asm__ __volatile__ ("" : : : "memory") //内存屏障

static int ringbuffer_write(struct ringbuffer_t *rb, uint8_t *buffer, int len)
{
    int space_len=0, left_len=0;

    if(!rb || !buffer || len>rb->reserve_size) return -1;

    space_len = ((rb->rpos - rb->wpos)+rb->buffer_size)%rb->buffer_size; //计算长度
    if(rb->rpos==rb->wpos) space_len=rb->buffer_size;
    if(space_len <= len) return -1; //避免读写指针相遇
    
    left_len = rb->buffer_size - rb->wpos; //右边的长度

    left_len = _min(len, left_len); //求出最小值

    memcpy(rb->buffer+rb->wpos, buffer, left_len); //若是最小值等于len则不执行下面的拷贝
    memcpy(rb->buffer, buffer+left_len, len-left_len);//过边界拷贝，将剩下的拷贝到最前面

    smp_mb();
    
    rb->wpos = (rb->wpos+len)%rb->buffer_size;//取余

    return len;
}

static uint8_t* ringbuffer_read(struct ringbuffer_t *rb, int len)
{
    int space_len=0, left_len=0;

    if(!rb || len>rb->reserve_size) return NULL;
    space_len = ((rb->wpos - rb->rpos)+rb->buffer_size)%rb->buffer_size; //求出当前可读取的空间长度
    if(space_len <= len) return NULL; //判断加上等于条件为了避免读写指针相遇

    left_len = rb->buffer_size - rb->rpos;//求出右边可读长度

    if(left_len<len){ //需要将右边的拷贝到前面预留空间
        memcpy(rb->buffer-left_len, rb->buffer+rb->rpos, left_len);
        return rb->buffer-left_len;
    }

    return rb->buffer + rb->rpos;
}

static void ringbuffer_refreash(struct ringbuffer_t *rb, int len)
{
    if(!rb || len>rb->reserve_size) return;
    rb->rpos = (rb->rpos+len)%rb->buffer_size; //刷新指针位置
}

static void ringbuffer_free(struct ringbuffer_t *rb)
{
    uint8_t* buffer=NULL;
    
    if(!rb) return;
    buffer = rb->buffer-rb->reserve_size; //将指针偏移空间起始地址
    free(buffer);
    free(rb);
}

ringbuffer_t* create_ringbuffer(int size, int reserve_size)
{
    ringbuffer_t* rb=NULL;
    uint8_t* allbuffer=NULL;

    if(size < 0 || reserve_size<0) return NULL;
    rb = (ringbuffer_t*)malloc(sizeof(ringbuffer_t));
    allbuffer = (uint8_t*)malloc(size+reserve_size);
    rb->buffer = allbuffer+reserve_size; //偏移预留大小
    rb->buffer_size = size;
    rb->reserve_size = reserve_size;
    rb->rpos = 0; 
    rb->wpos = 0;    
    rb->rb_write = ringbuffer_write;
    rb->rb_read = ringbuffer_read;
    rb->rb_refreash = ringbuffer_refreash;
    rb->rb_free = ringbuffer_free;

    return rb;
}