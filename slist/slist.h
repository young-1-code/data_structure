#ifndef __SLIST_H__
#define __SLIST_H__

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 实现C语言泛型单链表
 * @author young  【微信公众号: Linux编程用C】
 * @mail estyoung71@gmail.com
*/


/**
 * @brief 定义链表节点
*/
typedef struct slist_s
{
    struct slist_s *next;
}slist_t;

/**
 * @brief 创建一个链表节点
 * @param _type 链表中元素的类型
*/
#define slist_new_node(_type) (slist_t*)malloc(sizeof(slist_t)+sizeof(_type))

/**
 * @brief 传入链表节点，获取节点的元素
 * @param _node 传入的链表节点指针
 * @param _type 链表元素的类型
 * @return 返回节点元素的指针
*/
#define slist_get_elem(_node, _type) ( (_type*)(((char*)_node)+sizeof(slist_t)) )

/**
 * @brief 在指定节点位置前插入一个节点
 * @param _pos 节点位置指针
 * @param _node 要插入的节点指针
*/
#define slist_insert_front(_pos, _node) ({ _node->next = _pos; _pos = _node; })

/**
 * @brief 在指定节点位置后插入一个节点
 * @param _pos 节点位置指针
 * @param _node 要插入的节点指针
*/
#define slist_insert_back(_pos, _node) ({ _node->next = _pos->next; _pos->next = _node; })

/**
 * @brief 删除链表节点
 * @param _prev 要删除的前一个节点
*/
#define slist_erase_node(_prev) ({ slist_t* tmp = _prev->next; _prev->next = tmp->next; tmp; })

/**
 * @brief 删除节点后重新复位
 * @param _prev 被删除节点前一个节点指针
 * @param _node 要删除节点的指针
*/
#define slist_erase_reset(_prev, _node) ({ _node = _prev; })

/**
 * @brief 遍历链表
 * @param head 链表头节点
 * @param node 当前节点指针
*/
#define slist_foreach(head, node) \
    for(node=head; node; node=node->next)

/**
 * @brief 遍历并删除节点
 * @param head 链表头节点
 * @param prev 保存节点的前一个节点
 * @param node 当前节点
*/
#define slist_foreach_safe(head, prev, node) \
    for(prev=head, node=prev->next; node; prev=node, node=node->next)

#endif /* end slist */
