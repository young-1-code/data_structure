/**
 * 简介：测试循环buffer
 * 时间：2023/9/14
 * 作者：(c)小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/

#ifndef __RINGBUFFER_SINGLE_H___
#define __RINGBUFFER_SINGLE_H___

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_ringbuffer
{
    volatile int w_pos;          /* 写入数据指针位置 */
    volatile int r_pos;          /* 读取数据指针位置 */
    volatile int r_lastlen;      /* 记录上一次读取数据的长度 */
    int skb_overflow_cnt;        /* 记录缓冲区溢出的次数     */
    int skb_size, skb_size_mask; /* 缓冲区空间大小  */
    uint8_t*     srb_buffer;     /* 缓存数据空间    */
    int (*write)(void* pthis, const uint8_t* buffer, int len); /* 写数据函数 */
    int (*read)(void* pthis, uint8_t **buffer, int len);       /* 读数据函数 */
}s_ringbuffer_t;


/**
 * @brief 创建一个循环buffer句柄
 * @param buf_size缓冲区大小
 * @return 循环buffer句柄
*/
s_ringbuffer_t* create_single_ringbuffer(int buf_size);

/**
 * @brief 销毁循环buffer
 * @param srb循环buffer句柄
 * @return -1:err 0:ok
*/
int destory_single_ringbuffer(s_ringbuffer_t* srb);

#ifdef __cplusplus 
}
#endif

#endif // !__RINGBUFFER_SINGLE_H___
