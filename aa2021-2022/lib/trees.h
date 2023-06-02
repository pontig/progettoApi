#include <stdio.h>
#include <stdlib.h>

typedef struct t_Info {
    char name[10];
    // TODO: Add your own data members
} tree_info;

//==================================================================
// Tree, oka red-black trees
//==================================================================

// Typedef enum red, black
typedef enum {
    RED,
    BLACK
} rb_color;

// Typedef red-black tree node
typedef struct tree_node {
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *parent;
    int key;
    tree_info info;
    rb_color color;
} tree_node;

// Typedef red-black tree
typedef tree_node *tree;

// Inorder tree walk
void inorder_tree_walk(tree root) {
    if (root == NULL) {
        return;
    }
    inorder_tree_walk(root->left);
    printf("%d ", root->key);
    // printf("\n");
    // if (root->parent != NULL) {
    //     printf("His father is %d\n", root->parent->key);
    // } else {
    //     printf("His father is NULL\n");
    // }
    // if (root->left != NULL) {
    //     printf("His left child is %d\n", root->left->key);
    // } else {
    //     printf("His left child is NULL\n");
    // }
    // if (root->right != NULL) {
    //     printf("His right child is %d\n", root->right->key);
    // } else {
    //     printf("His right child is NULL\n");
    // }
    // if (root->color == RED) {
    //     printf("He is Red\n");
    // } else {
    //     printf("He is Black\n");
    // }
    // printf("\n");
    inorder_tree_walk(root->right);
}

// Tree search
tree_node *tree_search(tree root, int key) {
    if (root == NULL) {
        return NULL;
    }
    if (root->key == key) {
        return root;
    }
    if (key < root->key) {
        return tree_search(root->left, key);
    } else {
        return tree_search(root->right, key);
    }
}

// Tree minimum and maximum
tree_node *tree_minimum(tree root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

tree_node *tree_maximum(tree root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->right != NULL) {
        root = root->right;
    }
    return root;
}

// Tree successor and predecessor
tree_node *tree_successor(tree root) {
    if (root == NULL) {
        return NULL;
    }
    if (root->right != NULL) {
        return tree_minimum(root->right);
    }
    tree_node *current = root->parent;
    while (current != NULL && root == current->right) {
        root = current;
        current = current->parent;
    }
    return current;
}

tree_node *tree_predecessor(tree *root) {
    if (*root == NULL) {
        return NULL;
    }
    if ((*root)->left != NULL) {
        return tree_maximum((*root)->left);
    }
    tree_node *current = (*root)->parent;
    while (current != NULL && *root == current->left) {
        *root = current;
        current = current->parent;
    }
    return current;
}

// Rotations
void left_rotate(tree *root, tree_node *x) {
    tree_node *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    return;
}

void right_rotate(tree *root, tree_node *y) {
    tree_node *x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
    return;
}

// Print tree with tree shape
void print_tree(tree root) {
    if (root == NULL) {
        return;
    }
    tree_node *current = root;
    int height = 0;
    while (current != NULL) {
        current = current->left;
        height++;
    }
    int i;
    for (i = 0; i < height; i++) {
        current = root;
        int j;
        for (j = 0; j < (2 ^ i); j++) {
            if (current != NULL) {
                printf("%d ", current->key);
                current = current->right;
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

// Red-Black tree fixup
void rb_insert_fixup(tree *root, tree_node *elm) {
    if (elm->parent == NULL) {
        elm->color = BLACK;
    } else {
        tree_node *father = elm->parent;
        if (father->color == RED) {
            if (father == father->parent->left) {
                tree_node *brother = elm->parent->right;
                if (brother != NULL && brother->color == RED) {
                    father->color = BLACK;
                    brother->color = BLACK;
                    father->parent->color = RED;
                    rb_insert_fixup(root, elm->parent);
                } else {
                    if (elm == father->right) {
                        elm = father;
                        left_rotate(root, elm);
                        father = elm->parent;
                    }
                    father->color = BLACK;
                    father->parent->color = RED;
                    right_rotate(root, father->parent);
                }
            } else {
                tree_node *brother = elm->parent->left;
                if (brother != NULL && brother->color == RED) {
                    father->color = BLACK;
                    brother->color = BLACK;
                    father->parent->color = RED;
                    rb_insert_fixup(root, elm->parent);
                } else {
                    if (elm == father->left) {
                        elm = father;
                        right_rotate(root, elm);
                        father = elm->parent;
                    }
                    father->color = BLACK;
                    father->parent->color = RED;
                    left_rotate(root, father->parent);
                }
            }
        }
    }
    return;
}

// Red-Black tree insert
void rb_insert(tree *root, int key, tree_info *info) {
    tree_node *parent = NULL;
    tree_node *current = *root;
    while (current != NULL) {
        parent = current;
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    tree_node *new_node = (tree_node *)malloc(sizeof(tree_node));
    new_node->key = key;
    new_node->info = *info;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = parent;
    new_node->color = RED;
    if (*root == NULL) {
        *root = new_node;
    } else if (key < parent->key) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
    rb_insert_fixup(root, new_node);
    return;
}

// Red-Black tree delete fixup
void rb_delete_fixup(tree *root, tree_node *elm) {
    if (elm->color == RED || elm->parent == NULL) {
        elm->color = BLACK;
    } else if (elm == elm->parent->left) {
        tree_node *brother = elm->parent->right;
        if (brother->color == RED) {
            brother->color = BLACK;
            elm->parent->color = RED;
            left_rotate(root, elm->parent);
            brother = elm->parent->right;
        }
        if (brother->left->color == BLACK && brother->right->color == BLACK) {
            brother->color = RED;
            rb_delete_fixup(root, elm->parent);
        } else {
            if (brother->right->color == BLACK) {
                brother->left->color = BLACK;
                brother->color = RED;
                right_rotate(root, brother);
                brother = elm->parent->right;
            }
            brother->color = elm->parent->color;
            elm->parent->color = BLACK;
            brother->right->color = BLACK;
            left_rotate(root, elm->parent);
        }
    } else {
        tree_node *brother = elm->parent->left;
        if (brother->color == RED) {
            brother->color = BLACK;
            elm->parent->color = RED;
            right_rotate(root, elm->parent);
            brother = elm->parent->left;
        }
        if (brother->right->color == BLACK && brother->left->color == BLACK) {
            brother->color = RED;
            rb_delete_fixup(root, elm->parent);
        } else {
            if (brother->left->color == BLACK) {
                brother->right->color = BLACK;
                brother->color = RED;
                left_rotate(root, brother);
                brother = elm->parent->left;
            }
            brother->color = elm->parent->color;
            elm->parent->color = BLACK;
            brother->left->color = BLACK;
            right_rotate(root, elm->parent);
        }
    }
}

// Red-Black tree delete
tree_node *rb_delete(tree *root, tree_node *elm) {
    tree_node *succ = (elm->left == NULL || elm->right == NULL) ? elm : tree_successor(elm);
    tree_node *child = (succ->left == NULL) ? succ->right : succ->left;

    child->parent = succ->parent;
    if (succ->parent == NULL) {
        *root = child;
    } else if (succ == succ->parent->left) {
        succ->parent->left = child;
    } else {
        succ->parent->right = child;
    }
    if (succ != elm) {
        elm->key = succ->key;
        elm->info = succ->info;
    }
    if (succ->color == BLACK) {
        rb_delete_fixup(root, child);  // a posto
    }
    return succ;
}