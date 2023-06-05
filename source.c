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

typedef struct BSNode {
    struct BSNode *left, *right, *father;
    int autonomy;
    int numberOfCars;
} bs_node;

typedef bs_node *bs_tree;
typedef struct LRBNode {
    struct LRBNode *left, *right, *parent;
    rb_color color;
    int autonomy;
    int numberOfCars;
} little_rb_node;

typedef struct {
    little_rb_node *root;
} little_rb_tree;

typedef struct {
    int max;
    little_rb_tree *cars;
} Parking;


typedef struct RBNode {
    struct RBNode *left, *right, *parent;
    rb_color color;
    int key;
    Parking *parking;
} rb_node;

typedef struct {
    rb_node *root;
} rb_tree;


// =======================================================
// Global variables
// =======================================================

rb_node *Tnil;
little_rb_node * little_Tnil;
rb_tree stations;

// =======================================================
// Red Bkack Tree functions
// =======================================================

void free_parking(bs_tree p) {
    if (p != NULL) {
        free_parking(p->left);
        free_parking(p->right);
        free(p);
    }
}

void inorder(rb_node *root) {
    if (root != Tnil) {
        inorder(root->left);
        printf("%u\n", root->key);
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

    //free_parking(z->parking->cars);
    free(z->parking);
    free(z);
    z->parking = NULL;
    z = NULL;
    return;
}


// =======================================================
// Little red-black tree functions
// =======================================================
void little_inorder(little_rb_node *root) {
    if (root != little_Tnil) {
        little_inorder(root->left);
        printf("%u\n", root->autonomy);
        little_inorder(root->right);
    }
    return;
}

little_rb_node *little_rb_search(little_rb_tree *t, int key) {
    little_rb_node *x = t->root;
    while (x != little_Tnil) {
        int com = x->autonomy - key;
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

void little_left_rotate(little_rb_tree *t, little_rb_node *x) {
    little_rb_node *y = x->right;
    x->right = y->left;
    if (y->left != little_Tnil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == little_Tnil) {
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
void little_right_rotate(little_rb_tree *t, little_rb_node *y) {
    little_rb_node *x = y->left;
    y->left = x->right;
    if (x->right != little_Tnil) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == little_Tnil) {
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

little_rb_node *little_rb_minimum(little_rb_node *x) {
    while (x->left != little_Tnil) {
        x = x->left;
    }
    return x;
}
little_rb_node *little_rb_successor(little_rb_node *x) {
    if (x->right != little_Tnil) {
        return little_rb_minimum(x->right);
    }
    little_rb_node *y = x->parent;
    while (y != little_Tnil && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void little_rb_transplant(little_rb_tree *t, little_rb_node *u, little_rb_node *v) {
    if (u->parent == little_Tnil) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
    return;
}
void little_rb_insert_fixup(little_rb_tree *t, little_rb_node *z) {
    if (z == t->root)
        z->color = BLACK;
    else {
        little_rb_node *y;
        little_rb_node *x = z->parent;

        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    little_rb_insert_fixup(t, x->parent);
                } else {
                    if (z == x->right) {
                        z = x;
                        little_left_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    little_right_rotate(t, x->parent);
                }
            } else {
                y = x->parent->left;
                if (y->color == RED) {
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    little_rb_insert_fixup(t, x->parent);
                } else {
                    if (z == x->left) {
                        z = x;
                        little_right_rotate(t, z);
                        x = z->parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    little_left_rotate(t, x->parent);
                }
            }
        }
    }
}

little_rb_node *little_rb_insert(little_rb_tree *t, int key) {
    little_rb_node *z = malloc(sizeof(little_rb_node));

    z->autonomy = key;
    z->left = little_Tnil;
    z->right = little_Tnil;
    z->parent = NULL;
    z->color = RED;
    z->numberOfCars = 1;

    little_rb_node *y = t->root;
    little_rb_node *x = t->root;

    while (x != little_Tnil) {
        y = x;
        if (x->autonomy - key < 0) {
            x = x->right;
        } else if (x->autonomy - key > 0) {
            x = x->left;
        } else {
            return NULL;
        }
    }

    z->parent = y;
    if (y == little_Tnil) {
        t->root = z;
    } else if (y->autonomy - key < 0) {
        y->right = z;
    } else {
        y->left = z;
    }
    little_rb_insert_fixup(t, z);

    return z;
}
void little_rb_delete_fixup(little_rb_tree *t, little_rb_node *z) {
    little_rb_node *w;

    while (z != t->root && z->color == BLACK) {
        if (z == z->parent->left) {
            w = z->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                z->parent->color = RED;
                little_left_rotate(t, z->parent);
                w = z->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                z = z->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    little_right_rotate(t, w);
                    w = z->parent->right;
                }
                w->color = z->parent->color;
                z->parent->color = BLACK;
                w->right->color = BLACK;
                little_left_rotate(t, z->parent);
                z = t->root;
            }
        } else {
            w = z->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                z->parent->color = RED;
                little_right_rotate(t, z->parent);
                w = z->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                z = z->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    little_left_rotate(t, w);
                    w = z->parent->left;
                }
                w->color = z->parent->color;
                z->parent->color = BLACK;
                w->left->color = BLACK;
                little_right_rotate(t, z->parent);
                z = t->root;
            }
        }
    }
    z->color = BLACK;
    return;
}
void little_rb_delete(little_rb_tree *t, little_rb_node *z) {
    little_rb_node *x;
    little_rb_node *y = z;
    rb_color y_original_color = y->color;

    if (z->left == little_Tnil) {
        x = z->right;
        little_rb_transplant(t, z, z->right);
    } else if (z->right == little_Tnil) {
        x = z->left;
        little_rb_transplant(t, z, z->left);
    } else {
        y = little_rb_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            little_rb_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        little_rb_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_original_color == BLACK) {
        little_rb_delete_fixup(t, x);
    }

    free(z);
    return;
}

// =======================================================
// BSTs function
// =======================================================

bs_node *bs_search(bs_tree t, int autonomy) {
    if (t == NULL || t->autonomy == autonomy) return t;
    if (t->autonomy < autonomy)
        return bs_search(t->right, autonomy);
    else
        return bs_search(t->left, autonomy);
}

bs_node *bs_minimum(bs_tree a) {
    bs_node *x = a;
    if (a == NULL) {
        return NULL;
    }

    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}
bs_node *bs_successor(bs_tree a) {
    bs_node *x = a;
    if (x->right != NULL) {
        return bs_minimum(x->right);
    }
    bs_node *y = x->father;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->father;
    }
    return y;
}

void bs_insert(bs_tree *t, int autonomy) {
    bs_node *y = NULL;
    bs_node *x = *t;

    while (x != NULL) {
        y = x;
        if (autonomy < x->autonomy) {
            x = x->left;
        } else if (autonomy > x->autonomy) {
            x = x->right;
        } else {
            // this car is already present
            x->numberOfCars++;
            return;
        }
    }

    bs_node *z = malloc(sizeof(bs_node));
    z->autonomy = autonomy;
    z->numberOfCars = 1;
    z->father = y;
    if (y == NULL) {
        *t = z;
    } else if (z->autonomy < y->autonomy) {
        y->left = z;
    } else {
        y->right = z;
    }
}

void bs_delete(bs_tree *t, bs_node *z) {
    bs_node *y, *x;

    if (z->left == NULL || z->right == NULL)
        y = z;
    else
        y = bs_successor(z);

    if (y->left != NULL) {
        x = y->left;
    } else {
        x = y->right;
    }
    if (x != NULL) {
        x->father = y->father;
    }
    if (y->father == NULL) {
        *t = x;
    } else if (y == y->father->left) {
        y->father->left = x;
    } else {
        y->father->right = x;
    }
    if (y != z) {
        z->autonomy = y->autonomy;
    }

    free(y);
    y = NULL;
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

int remove_car(rb_node *station, int autonomy) {

    little_rb_node *car = station->parking->cars->root;
    while (car != little_Tnil) {
        if (car->autonomy == autonomy)
            break;
        else if (car->autonomy < autonomy)
            car = car->right;
        else
            car = car->left;
    }

    if (car == little_Tnil) return 0;
    if (car->numberOfCars > 1) {
        --(car->numberOfCars);
    } else {
        little_rb_delete(station->parking->cars, car);
    }

    return 1;
}

// Returns the number of black nodes in a subtree of the given node
// or -1 if it is not a red black tree.
int computeBlackHeight(rb_node *currNode) {
    // For an empty subtree the answer is obvious
    if (currNode == Tnil)
        return 0;
    // Computes the height for the left and right child recursively
    int leftHeight = computeBlackHeight(currNode->left);
    int rightHeight = computeBlackHeight(currNode->right);
    int add = currNode->color == BLACK ? 1 : 0;
    // The current subtree is not a red black tree if and only if
    // one or more of current node's children is a root of an invalid tree
    // or they contain different number of black nodes on a path to a null node.
    if (leftHeight == -1 || rightHeight == -1 || leftHeight != rightHeight)
        return -1;
    else
        return leftHeight + add;
}

void printbst(bs_node *n, int prof) {
    if (n == NULL) {
        printf("NULL");
        return;
    }
    printf("%d:%d\n", n->autonomy, n->numberOfCars);
    printf("\n");
    for (int i = 0; i < prof; i++) {
        printf("  ");
    }

    printf("l: ");
    printbst(n->left, prof + 1);
    printf("\n");
    for (int i = 0; i < prof; i++) {
        printf("  ");
    }
    printf("r: ");
    printbst(n->right, prof + 1);
}

int main(int argc, char const *argv[]) {
    unsigned int tmpInt;  // Temporary integer to use as fast as possible
    rb_node *tmpNode;     // RBNode that olds the node of the new station, to be used to fill the cars
    int i;                // Iterator
    char tmpBuff[20];     // The buffer to put the ignored lines
    int line = 0;         // Line of the input

    // Generate Tnil
    Tnil = malloc(sizeof(rb_node));
    Tnil->left = NULL;
    Tnil->right = NULL;
    Tnil->parent = NULL;
    Tnil->color = BLACK;
    little_Tnil = malloc(sizeof(little_rb_node));
    little_Tnil->left = NULL;
    little_Tnil->right = NULL;
    little_Tnil->parent = NULL;
    little_Tnil->color = BLACK;

    stations.root = Tnil;

    for (char char_input = getchar(); char_input != EOF; char_input = getchar()) {
        line++;
        switch (char_input) {
            case 'a':
                // aggiungi
                fgets(tmpBuff, 9, stdin);

                char what = getchar();
                getchar();  // space
                switch (what) {
                    case 's':

                        fgets(tmpBuff, 8, stdin);
                        scanf("%u ", &tmpInt);
                        tmpNode = rb_insert(&stations, tmpInt);
                        if (tmpNode == NULL) {
                            printf("non aggiunta\n");
                            ignoreLine();
                        } else {
                            int numCars;
                            scanf("%u", &numCars);
                            if (numCars != 0) {
                                tmpNode->parking = malloc(sizeof(Parking));
                                tmpNode->parking->cars = malloc(sizeof(little_rb_tree));
                                tmpNode->parking->cars->root = little_Tnil;
                                tmpNode->parking->max = 0;
                            }
                            for (i = 0; i < numCars; i++) {
                                scanf(" %u", &tmpInt);
                                if (tmpInt > tmpNode->parking->max)
                                    tmpNode->parking->max = tmpInt;
                                little_rb_insert((tmpNode->parking->cars), tmpInt);
                            }
                            printf("aggiunta\n");
                            getchar();  // \n
                        }
                        break;

                    case 'a':
                        getchar();  // u
                        getchar();  // t
                        getchar();  // o
                        scanf("%u ", &tmpInt);
                        tmpNode = rb_search(&stations, tmpInt);
                        if (tmpNode == NULL) {
                            printf("non aggiunta\n");
                            ignoreLine();
                        } else {
                            if (tmpNode->parking == NULL) {
                                tmpNode->parking = malloc(sizeof(Parking));
                                tmpNode->parking->cars = malloc(sizeof(little_rb_tree));
                                tmpNode->parking->cars->root = little_Tnil;
                                tmpNode->parking->max = 0;
                                tmpNode->parking->max = 0;
                            }
                            scanf("%u\n", &tmpInt);
                            if (tmpInt > tmpNode->parking->max)
                                tmpNode->parking->max = tmpInt;
                            little_rb_insert((tmpNode->parking->cars), tmpInt);
                            printf("aggiunta\n");
                        }
                        break;

                    default:
                        break;
                }
                break;

            case 'd':
                fgets(tmpBuff, 18, stdin);  // emolisci-stazione [space]
                scanf("%u\n", &tmpInt);
                tmpNode = rb_search(&stations, tmpInt);
                if (tmpNode == NULL)
                    printf("non demolita\n");
                else {
                    rb_delete(&stations, tmpNode);
                    printf("demolita\n");
                }
                break;

            case 'r':
                fgets(tmpBuff, 12, stdin);  // ottama-auto [space]
                scanf("%u ", &tmpInt);
                tmpNode = rb_search(&stations, tmpInt);
                if (tmpNode == NULL) {
                    printf("non rottamata\n");
                    ignoreLine();
                } else {
                    scanf("%u\n", &tmpInt);
                    tmpInt = remove_car(tmpNode, tmpInt);
                    if (tmpInt)
                        printf("rottamata\n");
                    else
                        printf("non rottamata\n");
                }
                break;

            default:
                ignoreLine();
                printf("Test inserimenti\n");
                break;
        }
    }

    return 0;
}
