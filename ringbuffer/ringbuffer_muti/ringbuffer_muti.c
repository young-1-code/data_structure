/**
 * 简介：测试循环buffer
 * 时间：2024/4/25
 * 作者：(c)小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/

#include <string.h> /* memcpy memset */
#include <stdlib.h> /* malloc free */
#include "ringbuffer_muti.h"

#define _max(x, y) ( x>y ? x : y )
#define _min(x, y) ( x<y ? x : y )
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

inline int __write_data(void* pthis, const uint8_t* buffer, int len);

int __write_data(void* pthis, const uint8_t* buffer, int len)
{
    m_ringbuffer_t* mrb = (m_ringbuffer_t*)pthis;
    int right_len, wpos=mrb->w_pos; 

    right_len = mrb->rb_size - wpos;
    right_len = _min(right_len, len); 
    memcpy(mrb->rb_buffer+wpos, buffer, right_len);
    memcpy(mrb->rb_buffer, buffer+right_len, len-right_len);

    /* 更新写指针位置 */
    mrb->w_pos = (mrb->w_pos+len)&mrb->rb_size_mask;

    return 0;
}

/**
 * @brief 向循环buffer空间写入数据
 * @param pthis 循环buffer句柄
 * @param buffer 要写入的数据
 * @param len 要写入数据的长度
 * @return -1:参数错误 -2:可写空间不够 0:写入成功
*/
int mrb_write(void* pthis, const uint8_t* buffer, int len)
{
    int i, n, wpos, rpos, free_len;
    m_ringbuffer_t* mrb = (m_ringbuffer_t*)pthis;

    if( unlikely(!pthis || !buffer || len<1) ) return -1;
    n = mrb->read_cnt;
    wpos = mrb->w_pos;

    for(i=0; i<n; ++i){
        rpos = mrb->arr_pos[i]->r_pos;
        free_len = (rpos-wpos+mrb->rb_size)&mrb->rb_size_mask;
        if( unlikely(free_len <= len && rpos!=wpos) ){ /* 可写空间不够 */
            ++(mrb->rb_overflow_cnt);
            return -2;
        }
    }

    return __write_data(pthis, buffer, len);
}



/**
 * @brief 从循环buffer空间读取数据
 * @param pthis 循环buffer句柄
 * @param readid 读者的id,区分读者
 * @param buffer 要的数据指针的地址
 * @param len 要读取数据的长度
 * @return -1:参数错误 -2:可读空间不够 len:读取的数据长度
*/
static int mrb_read(void* pthis, int readid, uint8_t **buffer, int len)
{
    int free_len, right_len, rpos, last_len, wpos; 
    m_ringbuffer_t* mrb = (m_ringbuffer_t*)pthis;

    if( unlikely(readid<0 || readid>mrb->read_cnt) ) return -1;
    if( unlikely(!pthis || !buffer || len<1) ) return -1;

    wpos = mrb->w_pos;
    rpos = mrb->arr_pos[readid]->r_pos;
    last_len = mrb->arr_pos[readid]->r_last_len;

    rpos = (rpos + last_len)&mrb->rb_size_mask;
    free_len = (wpos - rpos + mrb->rb_size)&mrb->rb_size_mask;

    if( unlikely(free_len < len) ) return -2;
    
    mrb->arr_pos[readid]->r_pos = rpos;/* 更新上次的读指针 */

    right_len = mrb->rb_size - rpos;
    right_len = _min(right_len, len);
    
    *buffer = (mrb->rb_buffer + rpos);
    
    mrb->arr_pos[readid]->r_last_len = right_len; /* 记录本次读写的长度 */

    return right_len;
}

/**
 * @brief 向循环buffer添加读者
 * @param pthis 循环buffer句柄
 * @return -1:添加失败  id:读者ID
*/
static int mrb_add(void* pthis)
{
    int index;
    m_ringbuffer_t* mrb = (m_ringbuffer_t*)pthis;
    if(!pthis) return -1;
    index = mrb->read_cnt;
    mrb->arr_pos[index] = (struct readpos_t*)malloc(sizeof(struct readpos_t));
    if(!mrb->arr_pos[index]) return -1;

    mrb->arr_pos[index]->r_pos = 0;
    mrb->arr_pos[index]->r_last_len = 0;
    ++mrb->read_cnt;

    return index;
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
m_ringbuffer_t* create_muti_ringbuffer(int buf_size)
{
    m_ringbuffer_t* mrb;
    
    if(buf_size<1) return NULL;

    mrb = (m_ringbuffer_t*)malloc(sizeof(m_ringbuffer_t));
    if(!mrb) return NULL;

    memset(mrb, 0, sizeof(m_ringbuffer_t));
    mrb->rb_size = _pow_num(buf_size);
    mrb->rb_size_mask = mrb->rb_size - 1;
    mrb->rb_buffer = (uint8_t*)malloc( mrb->rb_size );
    if(!mrb->rb_buffer){
        free(mrb);
        return NULL;
    }
    mrb->add = mrb_add;
    mrb->read = mrb_read;
    mrb->write = mrb_write;

    return mrb;
}

/**
 * @brief 销毁循环buffer
 * @param srb循环buffer句柄
 * @return -1:err 0:ok
*/
int destory_muti_ringbuffer(m_ringbuffer_t* mrb)
{
    int i=0;

    if(!mrb) return -1;
    if(mrb->rb_buffer) free(mrb->rb_buffer);
    for(i=0; i<mrb->read_cnt; ++i){
        free(mrb->arr_pos[i]);
    }
    free(mrb);
    return 0;
}