#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"


/**
 * 定义用户的数据结构，包含rb_node
*/
struct info_t {
    struct rb_node node; // 红黑树节点
    int key;             // 键值
    //......                自定义其他数据结构
};

/**
 * 根据key值在红黑树中搜索，
*/
struct info_t* rbtree_search(struct rb_root *root, int key)
{
    struct rb_node *rbnode = root->rb_node;
    struct info_t *pinfo=NULL;

    while (rbnode!=NULL)
    {
        pinfo = container_of(rbnode, struct info_t, node);//获取节点信息

        if (key < pinfo->key)       //key值小于当前节点key,往左边节点走
            rbnode = rbnode->rb_left;
        else if (key > pinfo->key)  //key值大于当前节点key,往右边节点走
            rbnode = rbnode->rb_right;
        else                        //找到key返回
            return pinfo;
    }

    return NULL;                    //未找到，返回null
}

/**
 * 往红黑树中插入一个节点
*/
int rbtree_insert(struct rb_root *root,  struct info_t *new)
{
    struct info_t *mynode, *pinfo; // 新建结点
    struct rb_node **tmp = &(root->rb_node), *parent = NULL;
    int key = new->key;

    //1.找到插入节点位置
    while (*tmp)
    {
        pinfo = container_of(*tmp, struct info_t, node);

        parent = *tmp;
        if (key < pinfo->key)
            tmp = &((*tmp)->rb_left);
        else if (key > pinfo->key)
            tmp = &((*tmp)->rb_right);
        else
            return -1;
    }

    //2.插入节点
    rb_link_node(&new->node, parent, tmp);
    //3.调整并重新着色
    rb_insert_color(&new->node, root);

    return 0;
}

/**
 * 删除一个节点
 * 注意：只是从红黑树移除节点，并未free内存，需要外部free内存
*/
struct info_t* rbtree_delete(struct rb_root *root, int key)
{
    struct info_t *tmp=NULL;

    // 在红黑树中查找key对应的节点tmp
    if ((tmp = rbtree_search(root, key)) == NULL)
        return NULL;
    // 从红黑树中删除节点tmp
    rb_erase(&tmp->node, root);
    
    return tmp;
}

/**
 * 这个函数只是我们用来打印看节点信息的
*/
static void rbtree_print(struct rb_node *tree, int key, int direction)
{
    if(tree != NULL)
    {
        if(direction==0)    // tree是根节点
            printf("%2d(B) is root\n", key);
        else                // tree是分支节点
            printf("%2d(%s) is %2d's %6s child\n", key, rb_is_black(tree)?"B":"R", key, direction==1?"right" : "left");

        if (tree->rb_left)
            rbtree_print(tree->rb_left, rb_entry(tree->rb_left, struct info_t, node)->key, -1);
        if (tree->rb_right)
            rbtree_print(tree->rb_right,rb_entry(tree->rb_right, struct info_t, node)->key,  1);
    }
}

void show(struct rb_root *root)
{
    if (root!=NULL && root->rb_node!=NULL)
        rbtree_print(root->rb_node, rb_entry(root->rb_node, struct info_t, node)->key,  0);
}


void main()
{
    int a[] = {10, 40, 30, 60, 90, 70, 20, 50, 80};
    int i, ilen=sizeof(a)/sizeof(a[0]);
    struct rb_root mytree = RB_ROOT;
    struct info_t *pinfo=NULL;

    printf("== 原始数据: ");
    for(i=0; i<ilen; i++)
        printf("%d ", a[i]);
    printf("\n");

    for (i=0; i < ilen; i++)
    {
        pinfo = (struct info_t*)malloc(sizeof(struct info_t));
        pinfo->key = a[i];
        rbtree_insert(&mytree, pinfo);

        printf("== 添加节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        show(&mytree);
        printf("\n");
    }

    for (i=0; i<ilen; i++) {
        struct info_t *pinfo = rbtree_delete(&mytree, a[i]);
        if(!pinfo) continue;

        printf("== 删除节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        show(&mytree);
        printf("\n");
        free(pinfo);
    }
}