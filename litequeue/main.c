#include "litequeue.h"

void queue_test(void) //先进先出
{
    LiteQueue* queue = create_lite_queue(100, 32);
    char *msg=NULL;

    //1.队列插入元素
    for(int i=0; i<5; ++i)
    {
        msg = (char*)malloc(32);                //申请内存
        sprintf(msg, "queue node info = %d", i);//消息赋值
        queue->insert_tail(queue, msg);         //往尾部插入元素
    }

    //2.遍历队列中的元素
    printf("遍历队列中所有元素:\n");
    foreach_lite_queue(queue, msg){
        printf("%s\n", msg);
    }

    //3.取数据
    printf("从队列头取元素:\n");
    for(int i=0; i<5; ++i){
        msg = queue->front(queue);
        if(!msg) continue;
        printf("%s\n", msg);
        free(msg);
    }

    //4.状态
    printf("状态: %s\n", queue->empty(queue)==1 ? "空" : "非空");
}

void stack_test(void) //先进后出
{
    LiteQueue* stack = create_lite_queue(100, 32);
    char *msg=NULL;

    //1.栈插入元素
    for(int i=0; i<5; ++i)
    {
        msg = (char*)malloc(32);
        sprintf(msg, "stack node info = %d", i);
        stack->insert_head(stack, msg);
    }

    //2.遍历栈中的元素
    printf("遍历栈中所有元素:\n");
    foreach_lite_queue(stack, msg){
        printf("%s\n", msg);
    }

    //3.取数据
    printf("栈顶取元素:\n");
    for(int i=0; i<5; ++i){
        msg = stack->front(stack);
        if(!msg) continue;
        printf("%s\n", msg);
        free(msg);
    }

    //4.状态
    printf("状态: %s\n", stack->empty(stack)==1 ? "空" : "非空");
}

int main(void)
{
    queue_test();

    printf("\n");

    stack_test();

    return 0;
}