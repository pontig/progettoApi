#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PROMPTALPH
//#define PROMPTREMOVE
//#define PROMPTREPLACE

// =======================================================
// TYPE DEFINITION
// =======================================================

// Red-black tree
typedef enum {
    RED,
    BLACK
} rb_color;

typedef struct N {
    struct N *left, *right, *parent;
    rb_color color;
    char *key;
} rb_node;

typedef struct {
    struct N *root;
} rb_tree;

// Alphabet:  bst_node, bst
typedef struct FM {
    int n;  // Position
    int f;  // Flag: 1 for mandatory, 0 for forbidden
    struct FM *next;
} Track;
typedef Track *Positions;

typedef struct E {
    char c;         // The character itself
    int memb;       // If the char is NOT a member of ref
    Positions pos;  // Mandatory and Forbidden positions
    int app;        // Appearances:
                    // if it is <0 it is minimum,
                    // else it is exact
    struct E *left;
    struct E *right;
    struct E *parent;
} bst_node;
typedef bst_node *bst_tree;

// Word ==> char + position
typedef struct W {
    char *c;   // The character
    int *pos;  // The position in the unordered string
} Word;

typedef struct T {
    char c;     // The char
    int type;   /* Type of the information:
                0 - membership
                1 - mandatory pposition
                2 - forbidden position
                3 - minimum number of appearances
                4 - exact number of appearances
                */
    int value;  // Value of the information
    struct T *next;
} Three;
typedef Three *Check;

// Linked list of words
typedef struct L {
    char *key;
    struct L *next;
} linked_element;

// =======================================================
// GLOBAL VARIABLES
// =======================================================

int k;               // Length of the words
int e;               // Number of eligible words remaining
int emax;            // Maximum number of eligible words
rb_node *Tnil;       // Nil node
bst_tree alphabet;   // Tree of filters
rb_tree eligibiles;  // Tree of eligible words
rb_tree forbidden;   // Tree of forbidden words
char *toAdd;         // The word to add

// =======================================================
// RED BLACK TREE FUNCTIONS
// =======================================================

void generate_nil() {
    Tnil = malloc(sizeof(rb_node));
    Tnil->left = NULL;
    Tnil->right = NULL;
    Tnil->parent = NULL;
    Tnil->color = BLACK;
    return;
}

void inorder(rb_node *root) {
    if (root != Tnil) {
        inorder(root->left);
        printf("%s\n", root->key);
        inorder(root->right);
    }
    return;
}

rb_node *rb_search(rb_tree *t, char *key) {
    rb_node *x = t->root;
    while (x != Tnil) {
        if (strcmp(x->key, key) == 0) {
            return x;
        }
        if (strcmp(x->key, key) < 0) {
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
void rb_insert(rb_tree *t, char *key) {
    rb_node *z = malloc(sizeof(rb_node) /* + (k + 1) * sizeof(char) */);
    z->key = (char *)malloc(sizeof(char) * (k + 1));
    strcpy(z->key, key);
    z->left = Tnil;
    z->right = Tnil;
    z->parent = NULL;
    z->color = RED;

    rb_node *y = t->root;
    rb_node *x = t->root;

    while (x != Tnil) {
        y = x;
        if (strcmp(x->key, key) < 0) {
            x = x->right;
        } else {
            x = x->left;
        }
    }

    z->parent = y;
    if (y == Tnil) {
        t->root = z;
    } else if (strcmp(y->key, key) < 0) {
        y->right = z;
    } else {
        y->left = z;
    }
    rb_insert_fixup(t, z);

    return;
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
    free(z->key);
    free(z);
    return;
}

// =======================================================
// SORTING
// =======================================================

// char[]: quick_sort
void swapc(char *a, int pos1, int pos2) {
    char aux = a[pos1];
    a[pos1] = a[pos2];
    a[pos2] = aux;
}
int partition(char *a, int p, int r) {
    char x = a[r];
    int i = p - 1;
    int j = p;
    while (j < r) {
        if (a[j] <= x) {
            i++;
            swapc(a, i, j);
        }
        j++;
    }
    swapc(a, i + 1, r);
    return i + 1;
}
void quick_sort(char *a, int p, int r) {
    if (p < r) {
        int q = partition(a, p, r);
        quick_sort(a, p, q - 1);
        quick_sort(a, q + 1, r);
    }
}
void sortChar(char *a) {
    quick_sort(a, 0, k - 1);
}

// Word[]: heap sort
void swap(Word *a, int i, int j) {
    int n;
    char c;

    n = a->pos[i];
    a->pos[i] = a->pos[j];
    a->pos[j] = n;

    c = a->c[i];
    a->c[i] = a->c[j];
    a->c[j] = c;

    return;
}
void max_heapify(Word *a, int n, int i) {
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int largest;
    if (l < n && (a->c[l] > a->c[i] || (a->c[l] == a->c[i] && a->pos[l] > a->pos[i]))) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < n && (a->c[r] > a->c[largest] || (a->c[r] == a->c[largest] && a->pos[r] > a->pos[largest]))) {
        largest = r;
    }
    if (largest != i) {
        swap(a, i, largest);
        max_heapify(a, n, largest);
    }
}
void build_max_heap(Word *a, int n) {
    int i;
    for (i = n / 2 - 1; i >= 0; i--) {
        max_heapify(a, n, i);
    }
}
void heap_sort(Word *a, int n) {
    build_max_heap(a, n);
    int i;
    for (i = n - 1; i > 0; i--) {
        swap(a, 0, i);
        max_heapify(a, i, 0);
    }
}
void sortWord(Word *a) {
    heap_sort(a, k);
}

// =======================================================
// BINARY SEARCH TREE FUNCTIONS
// =======================================================

void bst_insert(bst_tree *a, bst_node *z) {
    bst_node *y = NULL;
    bst_node *x = *a;
    while (x != NULL) {
        y = x;
        if (z->c < x->c) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL) {
        *a = z;
    } else if (z->c < y->c) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = NULL;
    z->right = NULL;
    return;
}
bst_node *bst_search(bst_tree *a, char c) {
    bst_node *x = *a;
    while (x != NULL) {
        if (c < x->c) {
            x = x->left;
        } else if (c > x->c) {
            x = x->right;
        } else {
            return x;
        }
    }
    return NULL;
}
bst_node *bst_minimum(bst_tree a) {
    bst_node *x = a;
    if (a == NULL) {
        return NULL;
    }

    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}
bst_node *bst_successor(bst_tree a) {
    bst_node *x = a;
    if (x->right != NULL) {
        return bst_minimum(x->right);
    }
    bst_node *y = x->parent;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

// =======================================================
// UTILITIES
// =======================================================

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

// Insert a new node in the Position list ordered whthout duplicates
// Flag 1 for mandatory, 0 for forbidden
// Return 1 if it is a new node, 0 if it is a duplicate
int pos_insert(Positions *l, int n, int flag) {
    Track *new = (Track *)malloc(sizeof(Track));
    new->n = n;
    new->f = flag;
    new->next = NULL;
    if (*l == NULL) {
        *l = new;
    } else {
        Positions aux = *l;
        if (aux->n > n) {
            new->next = aux;
            *l = new;
            return 1;
        } else if (aux->n == n) {
            free(new);
            return 0;
        }
        while (aux->next != NULL && aux->next->n < n) {
            if (aux->n == n || aux->next->n == n) {
                free(new);
                return 0;
            }
            aux = aux->next;
        }
        if (aux->next == NULL) {
            aux->next = new;
        } else if (aux->next->n > n) {
            new->next = aux->next;
            aux->next = new;
        }
    }
    return 1;
}

// Return the position of the filter characters, and viceversa
int h(char x) {
    if (x == 45) return 0;
    if (x >= 48 && x <= 57) return x - 47;
    if (x >= 65 && x <= 90) return x - 54;
    if (x == 95) return 37;
    if (x >= 97 && x <= 122) return x - 59;
    return -1;
}
char hm1(int x) {
    if (x == 0) return 45;
    if (x >= 1 && x <= 10) return x + 47;
    if (x >= 11 && x <= 36) return x + 54;
    if (x == 37) return 95;
    if (x >= 38 && x <= 63) return x + 59;
    return -1;
}

// Ignore the entire line in stdin, using getline and freeing the memory
void ignoreLine() {
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) return;
    free(line);
    return;
}

// Get the appearance of every character in a string
void getApp(int *a, char *str) {
    for (int i = 0; i < k; i++) {
        a[h(str[i])]++;
    }
}

void new_info_insert(Check *l, char c, int type, int value) {
    // Insert a new node in the head of the list
    Three *new = (Three *)malloc(sizeof(Three));
    new->c = c;
    new->type = type;
    new->value = value;
    new->next = NULL;
    if (*l == NULL) {
        *l = new;
    } else {
        new->next = (*l);
        *l = new;
    }
}

// Head insert and remove from the list of linked elements
void list_head_insert(linked_element **l, char *s) {
    linked_element *new = (linked_element *)malloc(sizeof(linked_element));
    new->key = s;
    new->next = NULL;
    if (*l == NULL) {
        *l = new;
    } else {
        new->next = *l;
        *l = new;
    }
}
void list_head_remove(linked_element **l) {
    if (*l == NULL) return;
    linked_element *aux = *l;
    *l = (*l)->next;
    free(aux);
}

// =======================================================
// ENDING FUNCTIONS
// =======================================================

void freeAlphabet(bst_tree t) {
    if (t != NULL) {
        freeAlphabet(t->left);
        freeAlphabet(t->right);
        free(t);
    }
    return;
}

void freeCheck(Check l) {
    if (l != NULL) {
        freeCheck(l->next);
        free(l);
    }
    return;
}

void freeRBtree(rb_node *x) {
    if (x != Tnil) {
        freeRBtree(x->left);
        freeRBtree(x->right);
        free(x->key);
        free(x);
    }
}

void resetEligibiles() {
    rb_tree source, destination;

    if (e > emax / 2) {
        source = forbidden;
        destination = eligibiles;
    } else {
        source = eligibiles;
        destination = forbidden;
    }
    // merge_trees(destination, source.root);
    if (eligibiles.root == Tnil) {
        eligibiles.root = forbidden.root;
    } else {
        rb_node *i = rb_minimum(source.root);
        // rb_node *prec;
        while (i != Tnil) {
            rb_insert(&destination, i->key);
#ifdef PROMPTREPLACE
            printf("Re-inserting %s\n", i->key);
#endif
            // prec = i;
            i = rb_successor(i);
            // free(prec->key);
            // free(prec);
        }
        freeRBtree(source.root);
        eligibiles.root = destination.root;
    }

    forbidden.root = Tnil;
}

// =======================================================
// PLAY LOGIC
// =======================================================

void fillEligibiles() {
    bst_node *min = bst_minimum(alphabet);
    do {
        char c = getchar();
        ungetc(c, stdin);

        if (c != '+') {
            if (fgets(toAdd, k + 1, stdin) == NULL) break;
            getchar();  // \n
            emax++;
            if (alphabet == NULL) {
                // It is the first time we add eligibile words
                rb_insert(&eligibiles, toAdd);
                e++;
            } else {
                // We have some constraints, so we need to check if the word is valid
                int app[64] = {0};
                getApp(app, toAdd);
                int flag = 1;  // If we can move on the next char in the bst
                for (bst_node *i = min; i != NULL; i = bst_successor(i)) {
                    if (i->memb == 1 && app[h(i->c)] != 0) {
                        // The char appears but it shouldn't
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because the char %c appears but it should not\n", toAdd, i->c);
#endif
                    } else if (i->memb == 0) {
                        if (app[h(i->c)] == 0) {
                            // The char doesn't appear but it should
                            flag = 0;
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear but it should\n", toAdd, i->c);
#endif
                        } else if (i->app < 0 && (-1 * app[h(i->c)]) > i->app) {
                            // The char does not appear the minimum number of times
                            flag = 0;
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear the exactly %d times but %d\n", toAdd, i->c, -1 * i->app, app[h(i->c)]);
#endif
                        } else if (i->app > 0 && app[h(i->c)] != i->app) {
                            // The char does no appear the exact number of times
                            flag = 0;
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear at least %d times but %d\n", toAdd, i->c, i->app, app[h(i->c)]);
#endif
                        }
                        if (flag) {
                            // We have to check positions
                            for (Positions j = i->pos; j != NULL; j = j->next) {
                                if (j->f == 0 && toAdd[j->n] == i->c) {
                                    // The char appear in a forbidden position
                                    flag = 0;
#ifdef PROMPTREMOVE
                                    printf("Removing %s because the char %c appears in the forbidden position %d\n", toAdd, i->c, j->n);
#endif
                                    break;
                                }
                                if (j->f == 1 && toAdd[j->n] != i->c) {
                                    // The char does not appear in a mandatory position
                                    flag = 0;
#ifdef PROMPTREMOVE
                                    printf("Removing %s because the char %c does not appear in the mandatory position %d\n", toAdd, i->c, j->n);
#endif
                                    break;
                                }
                            }
                        }
                    }
                    if (flag) {
                        // We can move on the next char in the bst
                    } else {
                        // The word is not eligibile
                        rb_insert(&forbidden, toAdd);
                        break;
                    }
                }
                if (flag) {
                    // The word is eligibile
                    rb_insert(&eligibiles, toAdd);
                    e++;
                }
            }
        } else
            return;
    } while (1);
}

void excludeOthers(rb_node *t, Check c) {
    rb_node *j = rb_minimum(t);
    linked_element *forbiddenList = NULL, *aux;

nw:;
    while (j != Tnil) {
        int app[64] = {0};
        getApp(app, j->key);
        int flag = 1;  // If we can move on the next char in the check

        for (Check i = c; i != NULL; i = i->next) {
            int pos = h(i->c);
            switch (i->type) {
                case 0:
                    if (i->value == 1 && app[pos] != 0) {
                        // The char is present in the word but it should not be
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should not appear\n", j->key, i->c);
#endif
                    } else if (i->value == 0 && app[pos] == 0) {
                        // The char is not present in the word but it should be
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should appear\n", j->key, i->c);
#endif
                    }
                    break;

                case 1:
                    if (j->key[i->value] != i->c) {
                        // The char is not in a mandatory position
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should be in position %d, instead there is %c\n", j->key, i->c, i->value, t->key[i->value]);
#endif
                    }
                    break;

                case 2:
                    if (j->key[i->value] == i->c) {
                        // The char is in a forbidden position
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should not be in position %d\n", j->key, i->c, i->value);
#endif
                    }
                    break;

                case 3:
                    if (app[pos] < i->value) {
                        // The char does not appear enough times
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should appear at least %d times, instead it appears %d times\n", j->key, i->c, i->value, app[pos]);
#endif
                    }
                    break;

                case 4:
                    if (app[pos] != i->value) {
                        // The char does not appear the exact number of times
                        flag = 0;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should appear exactly %d times, instead it appears %d times\n", j->key, i->c, i->value, app[pos]);
#endif
                    }
                    break;

                default:
                    break;
            }

            if (!flag) {
                // The word is not valid
                rb_node *prec = j;
                j = rb_successor(j);
                rb_insert(&forbidden, prec->key);
                //rb_delete(&eligibiles, prec);
                list_head_insert(&forbiddenList, prec->key);
                e--;
                // return;

                goto nw;
            }
        }
        // The word is valid
        j = rb_successor(j);
    }

while (forbiddenList != NULL) {
        aux = forbiddenList;
        forbiddenList = forbiddenList->next;
        rb_delete(&eligibiles, rb_search(&eligibiles, aux->key));
        free(aux);
    }

    return;
}

void play(int max, char *ref) {
#ifdef PROMPTALPH
    FILE *fpalphabet = fopen("debug/alphabet.txt", "a");
    fprintf(fpalphabet, "\n\n==============\nPartita ref %s\n==============\n\n", ref);
#endif

    int attempts = 0;  // Number of attempts done until now
    Word input;        // The input word, read every time from stdin
    // char *orderRef = (char *)malloc(sizeof(char) * (k + 1));  // Reference word in ascii order
    char *output = (char *)malloc(sizeof(char) * (k + 1));  // Output string
    char *tildes = (char *)malloc(sizeof(char) * (k + 1));  // Reference word to edit
    int tempFlag;                                           // Meh

    // strcpy(orderRef, ref);
    // sortChar(orderRef);
    output[k] = '\0';
    input.c = (char *)malloc(sizeof(char) * (k + 1));
    input.pos = (int *)malloc(sizeof(int) * (k + 1));
#ifdef PROMPTALPH
    char *cpy = (char *)malloc(sizeof(char) * (k + 1));
#endif

    do {
        if (attempts >= max) {
            printf("ko\n");
            goto exit;
        } else {
            char c = getchar();
            ungetc(c, stdin);
            if (c == '+') {
                // +inserisci_inizio || +stampa_filtrate
                getchar();
                c = getchar();
                ignoreLine();
                if (c == 'i') {
                    // +inserisci_inizio
                    fillEligibiles();
                    ignoreLine();
                } else if (c == 's') {
                    // +stampa_filtrate
                    inorder(eligibiles.root);
                }
            } else {
                // It is a word
                for (int i = 0; i < k; i++) {
                    input.c[i] = getchar();
                    input.pos[i] = i;
                }
                input.c[k] = '\0';
                getchar();  // \n

#ifdef PROMPTALPH
                strcpy(cpy, input.c);
#endif

                rb_node *node = rb_search(&eligibiles, input.c);

                if (node == NULL) {
                    node = rb_search(&forbidden, input.c);
                } else {
                    e--;
                    rb_delete(&eligibiles, node);
                    rb_insert(&forbidden, input.c);
                }

                if (node == NULL) {
                    printf("not_exists\n");
                } else {
                    attempts++;
                    Check newInfo = NULL;   // New info to be added to the check
                    int hi = 0;             // Head on the input word
                    int hr = 0;             // Head on the reference word
                    int counter = 0;        // Number of '+' to print
                    int minApps[64] = {0};  // Minimum number of appearances of each char
                    strcpy(tildes, ref);

                    for (int i = 0; i < k; i++) {
                        if (input.c[i] == ref[i]) {
                            // Right char in right position
                            bst_node *alreadyKnow = bst_search(&alphabet, input.c[i]);
                            int posOfI = h(input.c[i]);
                            output[i] = '+';
                            counter++;
                            minApps[posOfI]++;

                            if (alreadyKnow == NULL) {
                                // We don't know anything about this char yet
                                bst_node *new = (bst_node *)malloc(sizeof(bst_node));
                                new->c = input.c[i];
                                new->app = 0;
                                new->app = -1;
                                new->pos = NULL;
                                tempFlag = pos_insert(&new->pos, input.pos[i], 1);

                                bst_insert(&alphabet, new);

                                // the only thing that we have to check about this char
                                // for the next excludeOther is to check if the char is in a mandatory position
                                new_info_insert(&newInfo, input.c[i], 1, input.pos[i]);
                            } else {
                                // We have to check what we already know about this char
                                if (alreadyKnow->app < 0 && -1 * minApps[posOfI] < alreadyKnow->app) {
                                    // It appears more times than we knew and it is ok
                                    new_info_insert(&newInfo, input.c[i], 3, minApps[posOfI]);
                                    alreadyKnow->app = -1 * minApps[posOfI];
                                }
                                tempFlag = pos_insert(&alreadyKnow->pos, input.pos[i], 1);
                                if (tempFlag) {
                                    // We have found a new mandatory position for this char
                                    new_info_insert(&newInfo, input.c[i], 1, input.pos[i]);
                                }
                            }

                            tildes[i] = '~';
                            input.c[i] = '~';
                        }
                    }

                    if (counter == k) {
                        printf("ok\n");
                        freeAlphabet(alphabet);
                        goto exit;
                    }

                    sortWord(&input);
                    sortChar(tildes);

                    do {
                        int posOfHi = h(input.c[hi]);
                        if (hr < k - counter && input.c[hi] > tildes[hr] /* || tildes[hr] == '~' */)
                            hr++;

                        else if (hr < k - counter && input.c[hi] == tildes[hr]) {
                            bst_node *alreadyKnow = bst_search(&alphabet, input.c[hi]);

                            // Right char in wrong position
                            output[input.pos[hi]] = '|';
                            minApps[posOfHi]++;

                            if (alreadyKnow == NULL) {
                                // We don't know anything about this char yet
                                bst_node *new = (bst_node *)malloc(sizeof(bst_node));
                                new->c = input.c[hi];
                                new->memb = 0;
                                new->app = -1;
                                new->pos = NULL;
                                tempFlag = pos_insert(&new->pos, input.pos[hi], 0);

                                bst_insert(&alphabet, new);

                                // this time we have to check another thing than in the previous case
                                new_info_insert(&newInfo, input.c[hi], 3, minApps[posOfHi]);
                                new_info_insert(&newInfo, input.c[hi], 2, input.pos[hi]);
                            } else {
                                // We have to check what we already know about this char
                                if (alreadyKnow->app < 0 && -1 * minApps[posOfHi] < alreadyKnow->app) {
                                    // It appears more times than we knew and it is ok
                                    new_info_insert(&newInfo, input.c[hi], 3, minApps[posOfHi]);
                                    alreadyKnow->app = -1 * minApps[posOfHi];
                                }
                                tempFlag = pos_insert(&alreadyKnow->pos, input.pos[hi], 0);
                                if (tempFlag) {
                                    // We have found a new forbidden position for this char
                                    new_info_insert(&newInfo, input.c[hi], 2, input.pos[hi]);
                                }
                            }

                            hi++;
                            hr++;
                        }

                        else if (hr >= k - counter || input.c[hi] < tildes[hr]) {
                            // Char not in the reference word or char appeared too many times
                            bst_node *alreadyKnow = bst_search(&alphabet, input.c[hi]);
                            output[input.pos[hi]] = '/';

                            if (minApps[posOfHi] == 0) {
                                // Definitely not in the reference word
                                if (alreadyKnow == NULL) {
                                    // We didn't know it
                                    bst_node *new = (bst_node *)malloc(sizeof(bst_node));
                                    new->c = input.c[hi];
                                    new->memb = 1;
                                    new->app = 0;
                                    new->pos = NULL;

                                    bst_insert(&alphabet, new);
                                    // The only thing that we have to check about this char is that it shouldn't appear at all
                                    new_info_insert(&newInfo, input.c[hi], 0, 1);
                                } else {
                                    // We already knew that, we don't have to do anything
                                }
                            } else {
                                // It has appeared too many times
                                // alreadyKnow should not be NULL
                                if (alreadyKnow->app < 0) {
                                    // Now we know exactly how many times it appeared
                                    alreadyKnow->app = minApps[posOfHi];
                                    new_info_insert(&newInfo, input.c[hi], 4, minApps[posOfHi]);
                                }
                                tempFlag = pos_insert(&alreadyKnow->pos, input.pos[hi], 0);
                                if (tempFlag) {
                                    // We have found a new forbidden position for this char
                                    new_info_insert(&newInfo, input.c[hi], 2, input.pos[hi]);
                                }
                            }
                            hi++;
                        }
                    } while (hi < k - counter);

                    printf("%s\n", output);
                    excludeOthers(eligibiles.root, newInfo);
#ifndef PROMPTALPH
                    freeCheck(newInfo);
#endif
                    printf("%d\n", e);

#ifdef PROMPTALPH
                    fprintf(fpalphabet, "Dopo la lettura della parola %s abbiamo le seguenti nuove info: %s\n", cpy, output);
                    for (Three *temp = newInfo; temp != NULL; temp = temp->next) {
                        switch (temp->type) {
                            case 0:
                                if (temp->value)
                                    fprintf(fpalphabet, "Il carattere %c non deve apparire\n", temp->c);
                                else
                                    fprintf(fpalphabet, "Il carattere %c deve apparire\n", temp->c);
                                break;
                            case 1:
                                fprintf(fpalphabet, "Il carattere %c deve apparire in posizione %d\n", temp->c, temp->value);
                                break;
                            case 2:
                                fprintf(fpalphabet, "Il carattere %c non deve apparire in posizione %d\n", temp->c, temp->value);
                                break;
                            case 3:
                                fprintf(fpalphabet, "Il carattere %c appare almeno %d volte\n", temp->c, temp->value);
                                break;
                            case 4:
                                fprintf(fpalphabet, "Il carattere %c appare esattamente %d volte\n", temp->c, temp->value);
                                break;
                            default:
                                break;
                        }
                    }
                    freeCheck(newInfo);
                    fprintf(fpalphabet, "\nPer cui l'alfabeto completo risulta:\n");
                    for (bst_node *i = bst_minimum(alphabet); i != NULL; i = bst_successor(i)) {
                        fprintf(fpalphabet, "Character %c:\n", i->c);
                        if (i->memb) {
                            fprintf(fpalphabet, "\tnot member\n");
                        } else {
                            fprintf(fpalphabet, "app: %d\n", i->app);
                            fprintf(fpalphabet, "Positions: ");
                            for (Positions j = i->pos; j != NULL; j = j->next) {
                                fprintf(fpalphabet, "%d%c, ", j->n, j->f ? 'M' : 'F');
                            }
                        }
                        fprintf(fpalphabet, "\n\n\n");
                    }

#endif
                }
            }
        }
    } while (1);
    freeAlphabet(alphabet);
exit:;
    free(tildes);
    free(output);
    free(input.c);
    free(input.pos);
    resetEligibiles();

#ifdef PROMPTALPH
    fclose(fpalphabet);
#endif

    return;
}

int main(int argc, char const *argv[]) {
    generate_nil();
    eligibiles.root = Tnil;
    forbidden.root = Tnil;

    if (scanf("%d\n", &k) == EOF) return -1;
    char *ref = (char *)malloc(sizeof(char) * (k + 1));  // Reference word
    int max;                                             // Maximum number of attempts allowed
    toAdd = (char *)malloc(sizeof(char) * (k + 1));

#ifdef PROMPTALPH
    FILE *fpalphabet = fopen("debug/alphabet.txt", "w");
    fprintf(fpalphabet, "Alphabet file\n");
    fclose(fpalphabet);
#endif

    fillEligibiles();

    char flag = getchar();

    while (flag != EOF && flag != '$') {
        char command = getchar();
        ignoreLine();

        switch (command) {
            case 'n':
                // +nuova_partita
                if (fgets(ref, k + 1, stdin) == NULL) break;
                if (scanf("%d\n", &max) == EOF) break;
                e = emax;
                play(max, ref);
                alphabet = NULL;
                break;
            case 'i':
                // +inserisci_inizio
                fillEligibiles(&eligibiles, NULL);
                ignoreLine();
                break;
            default:
                break;
        }
        flag = getchar();
    }

    //freeRBtree(eligibiles.root);
    //freeRBtree(forbidden.root);

    return 0;
}
