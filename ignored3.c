#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPTALPH
#define PROMPTREMOVE

typedef struct ELM {
    char *key;
    struct ELM *next;
} LinkedWord;
typedef LinkedWord *ListWord;

// Linked list node
typedef struct EL {
    int n;
    struct EL *next;
} Element;
typedef Element *List;

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
    int minApp;     // Minimum number of appearances
    int exactApp;   // Exact number of appearances
    struct E *left;
    struct E *right;
    struct E *parent;
} Filter;
typedef Filter *Alph;

typedef struct CH {
    char c;    // The char
    int app;   // How many times it appears
    List pos;  // Positions where it appears
    struct CH *next;
} Record;
typedef Record *CharFilter;

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

// Typedef to assign \, |
typedef struct {
    char *w;   // The character
    int *pos;  // The position in the unordered string
} Word;

// Typedef red black tree
typedef enum {
    RED,
    BLACK
} rb_color;
typedef struct tree_node {
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *parent;
    rb_color color;
    char *key;
} tree_node;
typedef tree_node *tree;

int k;          // lenght of the words
int e = 0;      // Number of eligibles words
int emax = 0;   // Number of elements in the tree
int match = 0;  // Number of the match in progress

// Head insert in a LinkedWord list
void add_excluded(ListWord *l, char *key) {
    ListWord new = (ListWord)malloc(sizeof(LinkedWord));
    new->key = key;
    new->next = *l;
    *l = new;
}
// Remove head of a LinkedWord list
void decapitate(ListWord *l) {
    ListWord aux = *l;
    *l = (*l)->next;
    free(aux);
}

// Insert a new node  in the list ordered, void and without duplicates
void order_insert(List *l, int n) {
    List new = malloc(sizeof(Element));
    new->n = n;
    new->next = NULL;
    if (*l == NULL) {
        *l = new;
    } else {
        List aux = *l;
        if (aux->n > n) {
            new->next = aux;
            *l = new;
            return;
        } else if (aux->n == n) {
            free(new);
            return;
        }
        while (aux->next != NULL && aux->next->n < n) {
            if (aux->n == n || aux->next->n == n) {
                free(new);
                return;
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

// Head insert a new node in the list, type void
void head_insert(CharFilter *l, Record *n) {
    n->next = NULL;
    if (*l == NULL) {
        *l = n;
    } else {
        n->next = (*l);
        *l = n;
    }
    return;
}
void h_ins(List *l, int n) {
    // head insert in List l
    Element *new = (Element *)malloc(sizeof(Element));
    new->n = n;
    new->next = NULL;
    if (*l == NULL) {
        *l = new;
    } else {
        new->next = (*l);
        *l = new;
    }
    return;
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

// Red-black tree rotations
void left_rotate(tree *root, tree_node *x) {
    tree_node *y = x->right;
    x->right = y->left;
    if (strcmp(y->left->key, "+") != 0) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (strcmp(x->parent->key, "+") == 0) {
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
    if (strcmp(x->right->key, "+") != 0) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (strcmp(x->parent->key, "+") == 0) {
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

// Inorder tree walk
void inorder(tree t) {
    if (strcmp(t->key, "+") != 0) {
        inorder(t->left);
        // if (t->used < match) {
        printf("%s\n", t->key);
        //}
        inorder(t->right);
    }
}

// Tree search
tree_node *rb_search(tree t, char *key) {
    while (strcmp(t->key, "+") != 0) {
        if (strcmp(key, t->key) < 0) {
            t = t->left;
        } else if (strcmp(key, t->key) > 0) {
            t = t->right;
        } else {
            return t;
        }
    }
    return NULL;
}

// Reset all used flags to 0
/* void reset_used(tree t) {
    if (t != NULL) {
        reset_used(t->left);
        if (t->used == match) {
            e++;
        }
        t->used = 0;
        reset_used(t->right);
    }
} */

// Heap sort an array of Word type
void swap(Word *a, int i, int j) {
    int n;
    char c;

    n = a->pos[i];
    a->pos[i] = a->pos[j];
    a->pos[j] = n;

    c = a->w[i];
    a->w[i] = a->w[j];
    a->w[j] = c;

    return;
}
void max_heapify(Word *a, int n, int i) {
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int largest;
    if (l < n && (a->w[l] > a->w[i] || (a->w[l] == a->w[i] && a->pos[l] > a->pos[i])))
        largest = l;
    else
        largest = i;

    if (r < n && (a->w[r] > a->w[largest] || (a->w[r] == a->w[largest] && a->pos[r] > a->pos[largest])))
        largest = r;

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

// Quick sort an array of char
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
int partW(Word *a, int p, int r) {
    char x = a->w[r];
    int i = p - 1;
    int j = p;
    while (j < r) {
        if (a->w[j] <= x) {
            i++;
            swap(a, i, j);
        }
        j++;
    }
    swap(a, i + 1, r);
    return i + 1;
}
void qs_mod(Word *a, int p, int r) {
    if (p < r) {
        int q = partW(a, p, r);
        qs_mod(a, p, q - 1);
        qs_mod(a, q + 1, r);
    }
}
// Returns the number of black nodes in a subtree of the given node
// or -1 if it is not a red black tree.
int computeBlackHeight(tree_node *currNode) {
    // For an empty subtree the answer is obvious
    if (strcmp(currNode->key, "+") == 0)
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
// Red-black tree insert
void rb_insert_fixup(tree *t, tree_node *z) {
    tree_node *x, *y;
    if (strcmp(z->parent->key, "+") == 0)
        z->color = BLACK;
    else {
        x = z->parent;
        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (/* y != NULL && */ y->color == RED) {
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
                if (/* y != NULL && */ y->color == RED) {
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
tree_node *create_tnil(tree_node *parent) {
    tree_node *tnil = (tree_node *)malloc(sizeof(tree_node));
    tnil->key = "+";
    tnil->color = BLACK;
    tnil->parent = parent;
    tnil->left = NULL;
    tnil->right = NULL;
    return tnil;
}
void rb_insert(tree *t, char *key) {
    tree_node *y = create_tnil(NULL);
    tree_node *x = *t;
    tree_node *z = (tree)malloc(sizeof(tree_node));
    z->key = key;
#ifdef PROMPTREMOVE
    // printf("Inserting %s (e=%d)\n", key, e);
#endif
    z->left = create_tnil(z);
    z->right = create_tnil(z);
    z->color = RED;
    while (strcmp(x->key, "+") != 0) {
        y = x;
        if (strcmp(z->key, x->key) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (strcmp(y->key, "+") == 0) {
        *t = z;
    } else if (strcmp(z->key, y->key) < 0) {
        free(y->left);
        y->left = z;
    } else {
        free(y->right);
        y->right = z;
    }
    rb_insert_fixup(t, z);
}
tree_node *rb_minimum(tree_node *x) {
    while (strcmp(x->left->key, "+") != 0) {
        x = x->left;
    }
    return x;
}
tree_node *rb_successor(tree_node *x) {
    tree_node *y;
    if (strcmp(x->right->key, "+") != 0) {
        return rb_minimum(x->right);
    }
    y = x->parent;
    while (strcmp(y->key, "+") != 0 && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}
void rb_transplant(tree *t, tree_node *u, tree_node *v) {
    if (strcmp(u->parent->key, "+") == 0) {
        *t = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}
void rb_delete_fixup(tree *t, tree_node *x) {
    if (x->color == RED || strcmp(x->parent->key, "+") == 0) {
        x->color = BLACK;
        return;
    }
    tree_node *w;
    if (x == x->parent->left) {
        w = x->parent->right;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            left_rotate(t, x->parent);
            w = x->parent->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(t, x->parent);

        } else {
            if (w->right->color == BLACK) {
                w->left->color = BLACK;
                w->color = RED;
                right_rotate(t, w);
                w = x->parent->right;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->right->color = BLACK;
            left_rotate(t, x->parent);
            x = *t;
        }
    } else {
        w = x->parent->left;
        if (w->color == RED) {
            w->color = BLACK;
            x->parent->color = RED;
            right_rotate(t, x->parent);
            w = x->parent->left;
        }
        if (w->right->color == BLACK && w->left->color == BLACK) {
            w->color = RED;
            rb_delete_fixup(t, x->parent);
        } else {
            if (w->left->color == BLACK) {
                w->right->color = BLACK;
                w->color = RED;
                left_rotate(t, w);
                w = x->parent->left;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->left->color = BLACK;
            right_rotate(t, x->parent);
            x = *t;
        }
    }
}
void rb_delete(tree_node *z) {
    tree t = z;
    while (strcmp(t->parent->key, "+") != 0) {
        t = t->parent;
    }
    tree_node *y, *x;
    if (strcmp(z->left->key, "+") == 0 || strcmp(z->right->key, "+") == 0) {
        y = z;
    } else {
        y = rb_successor(z);
    }
    if (strcmp(y->left->key, "+") != 0) {
        x = y->left;
    } else {
        x = y->right;
    }
    x->parent = y->parent;
    if (strcmp(y->parent->key, "+") == 0) {
        t = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    if (y != z) {
        z->key = y->key;
    }
    if (y->color == BLACK) {
        rb_delete_fixup(&t, x);
    }
}
void rb_delete_old(tree_node *z) {
    tree t = z;
    tree_node *y = z, *x;
    rb_color y_original_color = y->color;
    while (strcmp(t->parent->key, "+") != 0) {
        t = t->parent;
    }
    if (strcmp(z->left->key, "+") == 0) {
        x = z->right;
        rb_transplant(&t, z, z->right);
    } else if (strcmp(z->right->key, "+") == 0) {
        x = z->left;
        rb_transplant(&t, z, z->left);
    } else {
        y = rb_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (z == y->parent) {
            x->parent = y;
        } else {
            rb_transplant(&t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(&t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        if (y_original_color == BLACK) {
            rb_delete_fixup(&t, x);
        }
    }
}

// BST Alph
void bst_insert(Alph *a, Filter *z) {
    Filter *y = NULL;
    Filter *x = *a;
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
Filter *bst_search(Alph *a, char c) {
    Filter *x = *a;
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
Filter *bst_minimum(Alph a) {
    Filter *x = a;
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}
Filter *bst_successor(Alph a) {
    Filter *x = a;
    if (x->right != NULL) {
        return bst_minimum(x->right);
    }
    Filter *y = x->parent;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

// Return the position of the filter characters
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
#ifdef PROMPTREMOVE
    printf("--%s", line);
#endif
    free(line);
    return;
}

void exclude(tree_node *t, tree *l) {
    e--;
    rb_insert(l, t->key);
    rb_delete(t);
    return;
}
void fillEligibles(tree *t, Alph f, tree *l) {
    Filter *x = f == NULL ? NULL : bst_minimum(f);
    do {
    next:;
        char *i = (char *)malloc(sizeof(char) * (k + 1));
        char c = getchar();
        ungetc(c, stdin);
        if (c != '+') {
            if (fgets(i, k + 1, stdin) == NULL) break;
            getchar();
            emax++;
            if (f != NULL) {
                // We need to check the filter for i
                int app[64] = {0};
                for (int j = 0; j < k; j++) app[h(i[j])]++;
                for (Filter *j = x; j != NULL; j = bst_successor(j)) {
                    if (j->memb == 1 && app[h(j->c)] != 0) {
                        // The char appears but it should not
                        rb_insert(l, i);
#ifdef PROMPTREMOVE
                        printf("Removing %s because the char %c appears but it should not\n", i, j->c);
#endif
                        goto next;
                    } else if (j->memb == 0) {
                        if (app[h(j->c)] == 0) {
                            // The char does not appear but it should
                            rb_insert(l, i);
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear but it should\n", i, j->c);
#endif
                            goto next;
                        }
                        if (j->exactApp != -1 && app[h(j->c)] != j->exactApp) {
                            // The char does not appear the exact number of times it should
                            rb_insert(l, i);
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear the exactly %d times but %d\n", i, j->c, j->exactApp, app[h(j->c)]);
#endif
                            goto next;
                        } else if (j->exactApp == -1 && app[h(j->c)] < j->minApp) {
                            // The char does not appear the minimum number of times it should
                            rb_insert(l, i);
#ifdef PROMPTREMOVE
                            printf("Removing %s because the char %c does not appear at least %d times but %d\n", i, j->c, j->minApp, app[h(j->c)]);
#endif
                            goto next;
                        }
                        for (Positions aux = j->pos; aux != NULL; aux = aux->next) {
                            if (aux->f == 0 && i[aux->n] == j->c) {
                                // The char appear in a forbidden position
                                rb_insert(l, i);
#ifdef PROMPTREMOVE
                                printf("Removing %s because the char %c appears in the forbidden position %d\n", i, j->c, aux->n);
#endif
                                goto next;
                            }
                            if (aux->f == 1 && i[aux->n] != j->c) {
                                // The char does not appear in an mandatory position
                                rb_insert(l, i);
#ifdef PROMPTREMOVE
                                printf("Removing %s because the char %c does not appear in the mandatory position %d\n", i, j->c, aux->n);
#endif
                                goto next;
                            }
                        }
                    }
                }
            }
            rb_insert(t, i);
            e++;

        } else {
            free(i);
            return;
        }
    } while (1);
}

int isNIL(tree_node *t) {
    return strcmp(t->key, "+") == 0;
}

void getApp(int *arr, char *str) {
    for (int i = 0; i < k; i++) arr[h(str[i])]++;
}

void excludeOthers(tree t, Check f, tree *l) {
    if (strcmp(t->key, "+") != 0) {
        // /* if (strcmp(t->left->key, "+") != 0)  */excludeOthers(t->left, f, l);
        // /* if (strcmp(t->right->key, "+") != 0) */ excludeOthers(t->right, f, l);
        // if (t->used < match) {

        tree_node *x = rb_minimum(t);

    jump:
        while (strcmp(x->key, "+") != 0) {
            int flag = 0;  // If the word must be excluded
            int app[64] = {0};
            getApp(app, x->key);
            for (Check i = f; i != NULL; i = i->next) {
                int pos = h(i->c);
                switch (i->type) {
                    case 0:
                        if (i->value == 1 && app[pos] != 0) {
                            // The char is present in the word but it should not be
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should not appear\n", x->key, i->c);
#endif

                            flag = 1;
                        }
                        if (i->value == 0 && app[pos] == 0) {
                            // The char is not present in the word but it should be
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should appear\n", x->key, i->c);
#endif

                            flag = 1;
                        }
                        break;
                    case 1:
                        if (x->key[i->value] != i->c) {
                            // The char is not in a mandatory position
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should be in position %d, instead there is %c\n", x->key, i->c, i->value, t->key[i->value]);
#endif

                            flag = 1;
                        }
                        break;
                    case 2:
                        if (x->key[i->value] == i->c) {
                            // The char is in a forbidden position
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should not be in position %d\n", x->key, i->c, i->value);
#endif

                            flag = 1;
                        }
                        break;
                    case 3:
                        if (app[pos] < i->value) {
                            // The char does not appear enough times
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should appear at least %d times, instead it appears %d times\n", x->key, i->c, i->value, app[pos]);
#endif

                            flag = 1;
                        }
                        break;
                    case 4:
                        if (app[pos] != i->value) {
                            // The char does not appear the exact number of times
#ifdef PROMPTREMOVE
                            printf("Removing %s because %c should appear exactly %d times, instead it appears %d times\n", x->key, i->c, i->value, app[pos]);
#endif

                            flag = 1;
                        }
                        break;

                    default:
                        break;
                }
                if (flag) {
                    tree_node *prec = x;
                    x = rb_successor(x);
                    exclude(prec, l);
                    goto jump;
                }
            }
            x = rb_successor(x);
        }
        //}
        return;
    }
}

// Hope this is enough to prevent memory leaks
void freeList(List head) {
    if (head != NULL) {
        free(head->next);
        free(head);
    }
}
void freeCharFilter(CharFilter head) {
    if (head != NULL) {
        freeCharFilter(head->next);
        freeList(head->pos);
        free(head);
    }
}
void freeTree(tree root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root->key);
        // free(root->ordered);
        free(root);
    }
}
void freePos(Positions l) {
    if (l != NULL) {
        freePos(l->next);
        free(l);
    }
}

void bst_inorder(Alph a) {
    if (a != NULL) {
        bst_inorder(a->left);
        printf("%d ", a->c);
        bst_inorder(a->right);
    }
}

void freeAlphabet(Alph a) {
    if (a != NULL) {
        freeAlphabet(a->left);
        freeAlphabet(a->right);
        freePos(a->pos);
        free(a);
        return;
    }
}

void freeCheck(Check c) {
    if (c != NULL) {
        freeCheck(c->next);
        free(c);
    }
}

tree mergeTrees(tree elig, tree excluded) {
    tree source, destination;
    if (e > (emax / 2)) {
        source = excluded;
        destination = elig;
    } else {
        source = elig;
        destination = excluded;
    }
    tree_node *i = rb_minimum(source), *prec;
    while (strcmp(i->key, "+") != 0) {
        rb_insert(&destination, i->key);
        if (strcmp(i->left->key, "+") == 0) {
            free(i->left);
        }
        if (strcmp(i->right->key, "+") == 0) {
            free(i->right);
        }
        prec = i;
        i = rb_successor(i);
        free(prec);
    }

    return destination;
}

void play(int max, char *ref, tree *elig) {
    int attempts = 0;                    // Number of attempts done until now
    int flag = 1;                        // If the word must be compared or not
    Alph alphabet = NULL;                // Tree of filters, one for each character
    tree surelyNot = create_tnil(NULL);  // Tree of words that cannot be the solution
#ifdef PROMPTALPH
    FILE *fpalphabet = fopen("debug/alphabet.txt", "a");
    fprintf(fpalphabet, "\n\n==============\nPartita numero %d: ref %s\n==============\n\n", match, ref);
    printf("\n\n==============\nPartita numero %d: ref %s\n==============\n\n", match, ref);
#endif

    do {
        if (attempts >= max) {
            printf("ko\n");
            flag = 0;
        } else {
            char c = getchar();
            ungetc(c, stdin);
            if (c == '+') {
                // +inserisci_inizio || +stampa_filtrate
                getchar();
                c = getchar();
                if (c == 'i') {
                    // +inserisci_inizio
                    ignoreLine();
                    fillEligibles(elig, alphabet, &surelyNot);
                    ignoreLine();
                } else if (c == 's') {
                    // +stampa_filtrate
                    ignoreLine();
                    inorder(*elig);
                }
            } else {
                // It is a word
                Check newToCheck = NULL;                                  // New list of filters to check
                char *orderRef = (char *)malloc(sizeof(char) * (k + 1));  // The reference word in the order of the alphabet
                char *output = (char *)malloc(sizeof(char) * (k + 1));    // String of comparison
                char *unWord = (char *)malloc(sizeof(char) * (k + 1));    // Input word unordered
                Word ordWord;                                             // Input word ordered
                int hw = 0;                                               // Head word
                int hr = 0;                                               // Head reference
                int counter;                                              // Number of characters found in the right place
                int garbage;                                              // No one really cares about this
                output[k] = '\0';

                // Copy reference word in the order of the alphabet in orderRef
                strcpy(orderRef, ref);
                ordWord.w = malloc(sizeof(char) * (k + 1));
                ordWord.pos = malloc(sizeof(int) * (k + 1));

                // Scan the input word
                for (int i = 0; i < k; i++) {
                    if (scanf("%c", &ordWord.w[i]) == EOF) break;
                    unWord[i] = ordWord.w[i];
                    ordWord.pos[i] = i;
                }
                unWord[k] = '\0';
                ordWord.w[k] = '\0';
                getchar();

                tree_node *node = rb_search(*elig, unWord);
                if (node == NULL) {
                    node = rb_search(surelyNot, unWord);
                } else {
                    exclude(node, &surelyNot);
                }

                if (node == NULL) {
                    printf("not_exists\n");
                } else {
                    counter = 0;

                    attempts++;

                    // Update the filter
                    int tempMinApps[64] = {0};  // Appearences of each char in this string

                    for (int i = 0; i < k; i++) {
                        int tempCharPos = h(ordWord.w[i]);
                        if (ordWord.w[i] == orderRef[i]) {
                            // Right character in the right place
                            counter++;
                            tempMinApps[tempCharPos]++;
                            Filter *charInfo = bst_search(&alphabet, ordWord.w[i]);
                            if (charInfo == NULL) {
                                // We don't know anything about this char yet
                                Filter *new = (Filter *)malloc(sizeof(Filter));
                                new->c = ordWord.w[i];
                                new->memb = 0;
                                new->minApp = 1;
                                new->exactApp = -1;
                                new->pos = NULL;
                                garbage = pos_insert(&new->pos, ordWord.pos[i], 1);
                                bst_insert(&alphabet, new);
                                new_info_insert(&newToCheck, orderRef[i], 1, i);
                                new_info_insert(&newToCheck, ordWord.w[i], 3, 1);
                            } else {
                                // We have to check what we already know
                                if (tempMinApps[tempCharPos] > charInfo->minApp) {
                                    charInfo->minApp = tempMinApps[tempCharPos];
                                    new_info_insert(&newToCheck, orderRef[i], 3, tempMinApps[tempCharPos]);
                                }
                                garbage = pos_insert(&charInfo->pos, ordWord.pos[i], 1);
                                if (garbage) new_info_insert(&newToCheck, ordWord.w[i], 1, i);
                            }
                            output[i] = '+';
                            ordWord.w[i] = '~';
                            orderRef[i] = '~';
                        }
                    }
                    if (counter == k) {
                        printf("ok\n");
                        flag = 0;
                    } else {
                        quick_sort(orderRef, 0, k - 1);
                        // qs_mod(&ordWord, 0, k - 1);
                        heap_sort(&ordWord, k);

                        do {
                            // Filters for / , |
                            int tempCharPos = h(ordWord.w[hw]);
                            Filter *charInfo = bst_search(&alphabet, ordWord.w[hw]);
                            int isNull = charInfo == NULL ? 1 : 0;
                            if (orderRef[hr] == ordWord.w[hw]) {
                                // Right character in the wrong position
                                output[ordWord.pos[hw]] = '|';
                                tempMinApps[tempCharPos]++;

                                if (isNull) {
                                    // We don't know anything about this char yet
                                    Filter *new = (Filter *)malloc(sizeof(Filter));
                                    new->c = ordWord.w[hw];
                                    new->memb = 0;
                                    new->minApp = 1;
                                    new->exactApp = -1;
                                    new->pos = NULL;
                                    garbage = pos_insert(&new->pos, ordWord.pos[hw], 0);
                                    bst_insert(&alphabet, new);
                                    new_info_insert(&newToCheck, ordWord.w[hw], 2, ordWord.pos[hw]);
                                    new_info_insert(&newToCheck, ordWord.w[hw], 3, 1);
                                } else {
                                    // We have to check what we already know
                                    if (tempMinApps[tempCharPos] > charInfo->minApp) {
                                        charInfo->minApp = tempMinApps[tempCharPos];
                                        new_info_insert(&newToCheck, orderRef[hr], 3, tempMinApps[tempCharPos]);
                                    }
                                    garbage = pos_insert(&charInfo->pos, ordWord.pos[hw], 0);
                                    if (garbage) new_info_insert(&newToCheck, ordWord.w[hw], 2, ordWord.pos[hw]);
                                }
                                hr++;
                                hw++;
                            } else if (orderRef[hr] > ordWord.w[hw]) {
                                // Wrong character or character appeared too many times
                                output[ordWord.pos[hw]] = '/';
                                if (isNull) {
                                    // Surely wrong character
                                    Filter *new = (Filter *)malloc(sizeof(Filter));
                                    new->c = ordWord.w[hw];
                                    new->memb = 1;
                                    new->pos = NULL;
                                    new_info_insert(&newToCheck, ordWord.w[hw], 0, 1);
                                    bst_insert(&alphabet, new);
                                } else {
                                    if (charInfo->memb == 1) {
                                        // We already know this character shouldn't appear
                                    } else {
                                        // The character appeared too many times
                                        garbage = pos_insert(&charInfo->pos, ordWord.pos[hw], 0);
                                        if (garbage) new_info_insert(&newToCheck, ordWord.w[hw], 2, ordWord.pos[hw]);
                                        charInfo->exactApp = charInfo->minApp;
                                        new_info_insert(&newToCheck, ordWord.w[hw], 4, charInfo->exactApp);
                                    }
                                }
                                hw++;
                            } else {
                                hr++;
                            }
                            if (hr == k || orderRef[hr] == '~') {
                                // Remaining characters
                                for (; hw < k && ordWord.w[hw] != '~'; hw++) {
                                    Filter *charInfo = bst_search(&alphabet, ordWord.w[hw]);
                                    output[ordWord.pos[hw]] = '/';
                                    if (charInfo == NULL) {
                                        Filter *new = (Filter *)malloc(sizeof(Filter));
                                        new->c = ordWord.w[hw];
                                        new->memb = 1;
                                        new->pos = NULL;
                                        new_info_insert(&newToCheck, ordWord.w[hw], 0, 1);
                                        bst_insert(&alphabet, new);
                                    } else {
                                        if (charInfo->memb == 1) {
                                            // We already know this character shouldn't appear
                                        } else {
                                            // The character appeared too many times
                                            garbage = pos_insert(&charInfo->pos, ordWord.pos[hw], 0);
                                            if (garbage) new_info_insert(&newToCheck, ordWord.w[hw], 2, ordWord.pos[hw]);
                                            charInfo->exactApp = charInfo->minApp;
                                            new_info_insert(&newToCheck, ordWord.w[hw], 4, charInfo->exactApp);
                                        }
                                    }
                                }
                            }
                        } while (hw < k && hr < k && orderRef[hr] != '~' && ordWord.w[hw] != '~');
                        printf("%s\n", output);
                        excludeOthers(*elig, newToCheck, &surelyNot);
                        while (strcmp((*elig)->parent->key, "+") != 0) {
                            elig = &((*elig)->parent);
                        }
#ifndef PROMPTALPH
                        freeCheck(newToCheck);
#endif
                        printf("%d\n", e);
                    }

#ifdef PROMPTALPH
                    fprintf(fpalphabet, "Dopo la lettura della parola %s abbiamo le seguenti nuove info:\n", unWord);
                    for (Three *temp = newToCheck; temp != NULL; temp = temp->next) {
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
                    freeCheck(newToCheck);
                    fprintf(fpalphabet, "\nPer cui l'alfabeto completo risulta:\n");
                    for (Filter *i = bst_minimum(alphabet); i != NULL; i = bst_successor(i)) {
                        fprintf(fpalphabet, "Character %c:\n", i->c);
                        if (i->memb) {
                            fprintf(fpalphabet, "\tnot member\n");
                        } else {
                            fprintf(fpalphabet, "minApp: %d\n", i->minApp);
                            fprintf(fpalphabet, "ExactApp: %d\n", i->exactApp);
                            fprintf(fpalphabet, "Positions: ");
                            for (Positions j = i->pos; j != NULL; j = j->next) {
                                fprintf(fpalphabet, "%d%c, ", j->n, j->f ? 'M' : 'F');
                            }
                        }
                        fprintf(fpalphabet, "\n\n\n");
                    }

#endif
                }
                // free all malloc created in the else block
                free(output);
                free(unWord);
                free(orderRef);
                free(ordWord.w);
                free(ordWord.pos);
            }
        }
    } while (flag);
    // Free dynamically allocated memory
    freeAlphabet(alphabet);
    *elig = mergeTrees(*elig, surelyNot);
#ifdef PROMPTALPH
    fclose(fpalphabet);
#endif
    return;
}

int main(int argc, char const *argv[]) {
    tree eligibiles = create_tnil(NULL);

    if (scanf("%d\n", &k) == EOF) return -1;
    char *ref = (char *)malloc(sizeof(char) * (k + 1));  // Reference word
    int max;                                             // Maximum number of attempts allowed

    fillEligibles(&eligibiles, NULL, NULL);

#ifdef PROMPTALPH
    FILE *fpalphabet = fopen("debug/alphabet.txt", "w");
    fprintf(fpalphabet, "Alphabet file\n");
    fclose(fpalphabet);
#endif

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
                match++;
                play(max, ref, &eligibiles);
                break;
            case 'i':
                // +inserisci_inizio
                fillEligibles(&eligibiles, NULL, NULL);
                ignoreLine();
                break;
            default:
                break;
        }
        flag = getchar();
    }

    // freeTree(eligibiles);
    // free(ref);
    return 0;
}