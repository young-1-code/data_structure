/**
 * 简介：测试循环buffer
 * 时间：2024/4/25
 * 作者：(c)小C
 * Note: 欢迎关注微信公众号[Linux编程用C]
*/

#ifndef __RINGBUFFER_MUTI_H___
#define __RINGBUFFER_MUTI_H___

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_READ_NUM (64) /* 最大读者数 */

struct readpos_t
{
    int r_pos;
    int r_last_len;
};

typedef struct m_ringbuffer
{
    struct readpos_t *arr_pos[MAX_READ_NUM];/* 读者信息数组    */  
    volatile int w_pos;          /* 写入数据指针位置 */
    int          read_cnt;       /* 读者数量       */
    int rb_overflow_cnt;         /* 记录缓冲区溢出的次数     */
    int rb_size, rb_size_mask;   /* 缓冲区空间大小  */
    uint8_t*      rb_buffer;     /* 缓存数据空间    */
    int (*add)(void* pthis);     /* 向循环buffer添加一个读者 */
    int (*write)(void* pthis, const uint8_t* buffer, int len); /* 写数据函数 */
    int (*read)(void* pthis, int readid, uint8_t **buffer, int len); /* 读数据函数 */
}m_ringbuffer_t;


/**
 * @brief 创建一个循环buffer句柄
 * @param buf_size缓冲区大小
 * @return 循环buffer句柄
*/
m_ringbuffer_t* create_muti_ringbuffer(int buf_size);

/**
 * @brief 销毁循环buffer
 * @param srb循环buffer句柄
 * @return -1:err 0:ok
*/
int destory_muti_ringbuffer(m_ringbuffer_t* mrb);

#ifdef __cplusplus 
}
#endif

#endif // !__RINGBUFFER_SINGLE_H___
