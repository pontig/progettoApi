#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char const *argv[]) {
    tree_info ins = {.name = "test"};
    tree albero = NULL;
    for (int i = 0; i < 10; i++) {
        rb_insert(&albero, i, &ins);
        printf("%d \n", i);
    }
    inorder_tree_walk(albero);
    printf("\n");
    print_tree(albero);
    return 0;
}
