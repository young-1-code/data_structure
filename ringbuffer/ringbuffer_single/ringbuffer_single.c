/**
 * 简介：测试循环buffer
 * 时间：2023/9/14
 * 作者：(c)小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/

#include <string.h> /* memcpy memset */
#include <stdlib.h> /* malloc free */
#include "ringbuffer_single.h"

#define _max(x, y) ( x>y ? x : y )
#define _min(x, y) ( x<y ? x : y )
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


/**
 * @brief 向循环buffer空间写入数据
 * @param pthis 循环buffer句柄
 * @param buffer 要写入的数据
 * @param len 要写入数据的长度
 * @return -1:参数错误 -2:可写空间不够 0:写入成功
*/
static int skb_write(void* pthis, const uint8_t* buffer, int len)
{
    s_ringbuffer_t* skb = (s_ringbuffer_t*)pthis;
    int free_len, right_len, rpos, wpos; 

    if( unlikely(!pthis || !buffer || len<1 ) ) return -1;

    rpos = skb->r_pos, wpos=skb->w_pos;
    free_len = (rpos - wpos + skb->skb_size)&skb->skb_size_mask;

    if( unlikely(rpos==wpos && 0==free_len) ) free_len=skb->skb_size;

    if( unlikely(free_len <= len) ){ /* 空间不够，记录 */
        ++skb->skb_overflow_cnt;
        return -2;
    }

    right_len = skb->skb_size - wpos;
    right_len = _min(right_len, len); 
    memcpy(skb->srb_buffer+wpos, buffer, right_len);
    memcpy(skb->srb_buffer, buffer+right_len, len-right_len);

    /* 更新写指针位置 */
    skb->w_pos = (skb->w_pos+len)&skb->skb_size_mask;

    return 0;
}

/**
 * @brief 从循环buffer空间读取数据
 * @param pthis 循环buffer句柄
 * @param buffer 要的数据指针的地址
 * @param len 要读取数据的长度
 * @return -1:参数错误 -2:可读空间不够 len:读取的数据长度
*/
static int skb_read(void* pthis, uint8_t **buffer, int len)
{
    s_ringbuffer_t* skb = (s_ringbuffer_t*)pthis;
    int free_len, right_len, tmp_rpos, wpos; 

    if( unlikely(!pthis || !buffer || len<1) ) return -1;

    wpos = skb->w_pos;
    tmp_rpos = (skb->r_pos + skb->r_lastlen)&skb->skb_size_mask;
    free_len = (wpos - tmp_rpos+ skb->skb_size)&skb->skb_size_mask;

    if( unlikely(free_len < len) ) return -2;
    
    skb->r_pos = tmp_rpos;/* 更新上次的读指针 */

    right_len = skb->skb_size - tmp_rpos;
    right_len = _min(right_len, len);
    
    *buffer = (skb->srb_buffer + tmp_rpos);
    
    skb->r_lastlen = right_len; /* 记录本次读写的长度 */

    return right_len;
}

/**
 * @brief 处理数的值为2的次幂
 * @param num 数组的数字
 * @return 返回最接近num的2的次幂数
*/
int _pow_num(int num)
{
    int cnt=0, retn=1;

    while(num){
        num>>=1;
        ++cnt;
    }

    cnt = cnt>31 ? 31 : cnt;

    while(cnt--) retn *= 2;

    return retn;
}

/**
 * @brief 创建一个循环buffer句柄
 * @param buf_size缓冲区大小
 * @return 循环buffer句柄
*/
s_ringbuffer_t* create_single_ringbuffer(int buf_size)
{
    s_ringbuffer_t* skb;
    
    if(buf_size<1) return NULL;

    skb = (s_ringbuffer_t*)malloc(sizeof(s_ringbuffer_t));
    if(!skb) return NULL;

    memset(skb, 0, sizeof(s_ringbuffer_t));
    skb->skb_size = _pow_num(buf_size);
    skb->skb_size_mask = skb->skb_size - 1;
    skb->srb_buffer = (uint8_t*)malloc( skb->skb_size );
    if(!skb->srb_buffer){
        free(skb);
        return NULL;
    }
    skb->read = skb_read;
    skb->write = skb_write;

    return skb;
}

/**
 * @brief 销毁循环buffer
 * @param srb循环buffer句柄
 * @return -1:err 0:ok
*/
int destory_single_ringbuffer(s_ringbuffer_t* srb)
{
    if(!srb) return -1;
    if(srb->srb_buffer) free(srb->srb_buffer);
    free(srb);
    return 0;
}