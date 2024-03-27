
#include <string.h>
#include "slist.h"

typedef struct info_t
{
    int age;
    char name[32];
}info_t;

int main(void)
{
    slist_t *node, *tmp, *prev, *head = slist_new_node(info_t);
    slist_get_elem(head, info_t)->age = -1;
    snprintf(slist_get_elem(head, info_t)->name, 32, "tom");

    for(int i=0; i<10; ++i){
        node = slist_new_node(info_t);
        
        slist_get_elem(node, info_t)->age = 10+i;
        snprintf(slist_get_elem(node, info_t)->name, 32, "tom%d", i);

        slist_insert_back(head, node);
    }

    printf("----------split-------------\n");
    slist_foreach(head, node){
        printf("name=%s age=%d\n", slist_get_elem(node, info_t)->name, slist_get_elem(node, info_t)->age);
    }

    printf("----------split-------------\n");
    slist_foreach_safe(head, prev, node){
        if(slist_get_elem(node, info_t)->age<15){
            tmp = slist_erase_node(prev);
            
            free(tmp);
            tmp = NULL;

            slist_erase_reset(prev, node);
        }
    }

    printf("----------split-------------\n");
    slist_foreach(head, node){
        printf("name=%s age=%d\n", slist_get_elem(node, info_t)->name, slist_get_elem(node, info_t)->age);
    }

    return 0;
}
