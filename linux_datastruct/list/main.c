#include <stdio.h>
#include <stdlib.h>
#include "list.h"

//定义用户结构体
struct info_t
{
    int id;
    char name[32];
    struct list_head node; //作为链表的一个节点
};

int main(int argc, char** argv)
{
    struct list_head hlist; //链表头节点
    struct info_t *pnode;

    INIT_LIST_HEAD(&hlist); //初始化链表头节点

    for(int i=0; i<10; ++i){
        //申请内存，创建节点
        pnode = (struct info_t*)malloc(sizeof(struct info_t));
        pnode->id=i;
        snprintf(pnode->name, 32, "Info-%d", i);
        list_add_tail(&pnode->node, &hlist);//将节点加入链表
    }

    struct list_head *pos, *n;
    struct info_t *pinfo;
    list_for_each(pos, &hlist){ //遍历链表
        pinfo = list_entry(pos, struct info_t, node);
        printf("get node: %s\n", pinfo->name);
    };

    list_for_each_safe(pos, n, &hlist){ //遍历并删除节点
        pinfo = list_entry(pos, struct info_t, node);
        printf("del safe node: %s\n", pinfo->name);
        list_del(pos);
        free(pinfo);
    }

    int is_empty = list_empty(&hlist);
    printf("链表状态: %s\n", is_empty?"空":"非空");

    return 0;
}