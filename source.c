#include <stdio.h>
#include <stdlib.h>

// =======================================================
// Type definitions
// =======================================================

// Red-black tree
typedef enum {
    RED,
    BLACK
} rb_color;

typedef struct Node {
    struct Node *left, *right, *parent;
    rb_color color;
    int key;
    Parking *parking;
} rb_node;

typedef struct {
    rb_node *root;
} rb_tree;

typedef struct {
    int max;
    
} Parking;

// =======================================================
// Global variables
// =======================================================

rb_node *Tnil;
rb_tree stations;

// =======================================================
// RED BLACK TREE FUNCTIONS
// =======================================================

void inorder(rb_node *root) {
    if (root != Tnil) {
        inorder(root->left);
        printf("%s\n", root->key);
        inorder(root->right);
    }
    return;
}

rb_node *rb_search(rb_tree *t, int key) {
    rb_node *x = t->root;
    while (x != Tnil) {
        int com = x->key - key;
        if (com == 0) {
            return x;
        }
        if (com < 0) {
            x = x->right;
        } else {
            x = x->left;
        }
    }
    return NULL;
}

void left_rotate(rb_tree *t, rb_node *x) {
    rb_node *y = x->right;
    x->right = y->left;
    if (y->left != Tnil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == Tnil) {
        t->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    return;
}
void right_rotate(rb_tree *t, rb_node *y) {
    rb_node *x = y->left;
    y->left = x->right;
    if (x->right != Tnil) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == Tnil) {
        t->root = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
    return;
}

rb_node *rb_minimum(rb_node *x) {
    while (x->left != Tnil) {
        x = x->left;
    }
    return x;
}
rb_node *rb_successor(rb_node *x) {
    if (x->right != Tnil) {
        return rb_minimum(x->right);
    }
    rb_node *y = x->parent;
    while (y != Tnil && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void rb_transplant(rb_tree *t, rb_node *u, rb_node *v) {
    if (u->parent == Tnil) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
    return;
}
void rb_insert_fixup(rb_tree *t, rb_node *z) {
    if (z == t->root)
        z->color = BLACK;
    else {
        rb_node *y;
        rb_node *x = z->parent;

        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if (z == x->right) {
                        z = x;
                        left_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    right_rotate(t, x->parent);
                }
            } else {
                y = x->parent->left;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    rb_insert_fixup(t, x->parent);
                } else {
                    if (z == x->left) {
                        z = x;
                        right_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    left_rotate(t, x->parent);
                }
            }
        }
    }
}

rb_node *rb_insert(rb_tree *t, int key) {
    rb_node *z = malloc(sizeof(rb_node));

    z->key = key;
    z->left = Tnil;
    z->right = Tnil;
    z->parent = NULL;
    z->color = RED;
    z->parking = NULL;

    rb_node *y = t->root;
    rb_node *x = t->root;

    while (x != Tnil) {
        y = x;
        if (x->key - key < 0) {
            x = x->right;
        } else if (x->key - key > 0) {
            x = x->left;
        } else {
            return NULL;
        }
    }

    z->parent = y;
    if (y == Tnil) {
        t->root = z;
    } else if (y->key - key < 0) {
        y->right = z;
    } else {
        y->left = z;
    }
    rb_insert_fixup(t, z);

    return z;
}
void rb_delete_fixup(rb_tree *t, rb_node *z) {
    rb_node *w;

    while (z != t->root && z->color == BLACK) {
        if (z == z->parent->left) {
            w = z->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                z->parent->color = RED;
                left_rotate(t, z->parent);
                w = z->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                z = z->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(t, w);
                    w = z->parent->right;
                }
                w->color = z->parent->color;
                z->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(t, z->parent);
                z = t->root;
            }
        } else {
            w = z->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                z->parent->color = RED;
                right_rotate(t, z->parent);
                w = z->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                z = z->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(t, w);
                    w = z->parent->left;
                }
                w->color = z->parent->color;
                z->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(t, z->parent);
                z = t->root;
            }
        }
    }
    z->color = BLACK;
    return;
}
void rb_delete(rb_tree *t, rb_node *z) {
    rb_node *x;
    rb_node *y = z;
    rb_color y_original_color = y->color;

    if (z->left == Tnil) {
        x = z->right;
        rb_transplant(t, z, z->right);
    } else if (z->right == Tnil) {
        x = z->left;
        rb_transplant(t, z, z->left);
    } else {
        y = rb_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_original_color == BLACK) {
        rb_delete_fixup(t, x);
    }
    return;
}

// =======================================================
// Utility functions
// =======================================================

void ignoreLine() {
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) return;
    free(line);
    return;
}

void new_car(rb_node *station, int autonomy) {
    
}

int main(int argc, char const *argv[]) {
    unsigned int tmpInt;  // Temporary integer to use as fast as possible
    rb_node *tmpNode;     // Node that olds the node of the new station, to be used to fill the cars
    int i;                // Iterator

    // Generate Tnil
    Tnil = malloc(sizeof(rb_node));
    Tnil->left = NULL;
    Tnil->right = NULL;
    Tnil->parent = NULL;
    Tnil->color = BLACK;

    stations.root = Tnil;

    for (char char_input = getchar(); char_input != EOF; char_input = getchar()) {
        switch (char_input) {
            case 'a':
                getchar();  // g
                getchar();  // g
                getchar();  // i
                getchar();  // u
                getchar();  // n
                getchar();  // g
                getchar();  // i
                getchar();  // -

                char what = getchar();
                getchar();  // space
                switch (what) {
                    case 's':
                        getchar();  // t
                        getchar();  // a
                        getchar();  // z
                        getchar();  // i
                        getchar();  // o
                        getchar();  // n
                        getchar();  // e
                        getchar();  // space
                        scanf("%u ", &tmpInt);
                        tmpNode = rb_insert(&stations, tmpInt);
                        if (tmpNode == NULL) {
                            printf("non aggiunta");
                            ignoreLine();
                        } else {
                            scanf("%u", &tmpInt);
                            for (i = 0; i < tmpInt; i++) {
                                scanf(" %u", &tmpInt);
                                new_car(tmpNode, tmpInt);
                            }
                            getchar();  // \n
                        }
                        break;

                    case 'a':
                        getchar();  // u
                        getchar();  // t
                        getchar();  // o
                        getchar();  // space
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }

    return 0;
}
