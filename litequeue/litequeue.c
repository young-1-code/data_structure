#include "litequeue.h"


static int insert_tail_lite_queue(LiteQueue* queue, void* elem)
{
    if(!queue || !elem || queue->curr_num > queue->max_node) return -1;

    void **node = (void**)((char*)elem+queue->offset);
    *queue->tail = node;
    queue->tail = node;
    *queue->tail = NULL;
    
    ++queue->curr_num;

    return 0;
}

static int insert_head_lite_queue(LiteQueue* queue, void* elem)
{
    if(!queue || !elem || queue->curr_num > queue->max_node) return -1;
    void **node = (void**)((char*)elem+queue->offset);
    
    void** tmp = *queue->head;
    *queue->head = node;
    *node = tmp;

    ++queue->curr_num;

    return 0;
}

static void* front_lite_queue(LiteQueue* queue)
{
    void** head=NULL;
    void* msg=NULL;

    if(!queue) return NULL;
    head = queue->head;

    if(!(*head)) return NULL;
    msg = (char*)*head - queue->offset;

    //删除节点
    head=*head;
    *queue->head = *head;

    --queue->curr_num;

    return msg;
}

static void free_lite_queue(LiteQueue* queue)
{
    if(queue) free(queue);
    queue=NULL;
}

static int empty_lite_queue(LiteQueue* queue)
{
    if(*queue->head==NULL) return 1;
    return 0;
}

LiteQueue* create_lite_queue(int max, int offset)
{
    LiteQueue* queue = (LiteQueue*)malloc(sizeof(LiteQueue));
    if(!queue) return NULL;
    queue->curr_num = 0;
    queue->max_node = max;
    queue->offset = offset;
    queue->head = (void**)&queue->node;
    queue->tail = queue->head;
    *queue->tail = NULL;

    queue->front = front_lite_queue;
    queue->insert_head = insert_head_lite_queue;
    queue->insert_tail = insert_tail_lite_queue;
    queue->free = free_lite_queue;
    queue->empty = empty_lite_queue;
    return queue;
}
