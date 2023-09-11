#ifndef __LITE_SDH_H__
#define __LITE_SDH_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct LiteQueue
{
    void**  head;     //管理头节点
    void**  tail;     //管理尾节点
    int     offset;   //偏移即承载数据的大小
    int     max_node; //链表中的最大节点数
    int     curr_num; //当前链表中的节点数
    size_t  node;     //节点头位置

    int (*insert_head)(struct LiteQueue* queue, void* elem); //往头部插入数据
    int (*insert_tail)(struct LiteQueue* queue, void* elem); //往尾部插入数据
    void*(*front)(struct LiteQueue* queue); //取头节点
    void (*free)(struct LiteQueue* queue);  //释放内存
    int (*empty)(struct LiteQueue* queue);  //是否为空
}LiteQueue;

LiteQueue* create_lite_queue(int max, int offset);

#define foreach_lite_queue(queue, msg) \
    void** _node; \
    for(_node=queue->head,msg=(char*)*_node-queue->offset; \
        *_node; \
        _node=*_node,msg=(char*)*_node-queue->offset)

#endif // !__LITE_SDH_H__
