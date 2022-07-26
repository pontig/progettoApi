#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int memb;    // If the char is NOT a member of ref
    int *cert;   // Positions where the character is
    int *forb;   // Positions where the character must not be
    int minApp;  // Minimum number of appearances
} Filter;

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
    char *key;
    rb_color color;
} tree_node;

// Typedef red-black tree
typedef tree_node *tree;

// Red-black tree rotations
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

// Red-black tree insert fixup
void rb_insert_fixup(tree *t, tree_node *z) {
    tree_node *x, *y;
    if (z->parent == NULL)
        z->color = BLACK;
    else {
        x = z->parent;
        if (x->color == RED) {
            if (x == x->parent->left) {
                y = x->parent->right;
                if (y != NULL && y->color == RED) {
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
                if (y != NULL && y->color == RED) {
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

// Red-black tree insert
void rb_insert(tree *t, char *key) {
    tree_node *y = NULL;
    tree_node *x = *t;
    tree_node *z = (tree)malloc(sizeof(tree_node));
    z->key = key;
    z->left = NULL;
    z->right = NULL;
    z->color = RED;
    while (x != NULL) {
        y = x;
        if (strcmp(z->key, x->key) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL) {
        *t = z;
    } else if (strcmp(z->key, y->key) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }
    rb_insert_fixup(t, z);
}

// Inorder tree walk
void inorder(tree t) {
    if (t != NULL) {
        inorder(t->left);
        printf("\n%s ", t->key);
        printf("color %d", t->color);
        if (t->parent != NULL) {
            printf("(father %s)", t->parent->key);
            if (t == t->parent->right)
                printf("(right)");
            else
                printf("(left)");
        }
        inorder(t->right);
    }
}

// Tree search
tree_node *rb_search(tree t, char *key) {
    while (t != NULL) {
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

int k;      // lenght of the words
int e = 0;  // Number of eligibles words

// Return the position of the filter characters
int h(int x) {
    if (x == 45) return 0;
    if (x >= 48 && x <= 57) return x - 47;
    if (x >= 65 && x <= 90) return x - 54;
    if (x == 95) return 37;
    if (x >= 97 && x <= 122) return x - 59;
    return -1;
}

// Ignore the entire line in stdin, using getline and freeing the memory
void ignoreLine() {
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    free(line);
}

void fillEligibles(char **arr) {
    do {
        char c = getchar();
        ungetc(c, stdin);
        if (c != '+') {
            arr[e] = (char *)malloc(sizeof(char) * k);
            fgets(arr[e], k + 1, stdin);
            getchar();
            e++;
        } else {
            return;
        }
    } while (1);
}

void play(int max, char *ref, char **elig) {
    int attempts = 0;  // Number of attempts done until now
    int flag = 1;      // If the word must be compared or not

    Filter alphabet[64];   // Array of filters, one for each character
    tree remained = NULL;  // Tree of words that are not eliminated

    // Put the words in the tree
    for (int i = 0; i < e; i++) {
        rb_insert(&remained, elig[i]);
    }

    do {
        char c = getchar();
        ungetc(c, stdin);
        if (c == EOF)
            flag = 0;
        else {
            if (c == '+') {
                // inserisci_inizio || stampa_filtrate
                getchar();
                c = getchar();
                if (c == 'i') {
                    ignoreLine();
                    fillEligibles(&ref);
                    ignoreLine();
                } else if (c == 's') {
                    // siamo nella cacca di stampa_filtrate
                }
            } else {
                // It is a word
                char *output = (char *)malloc(sizeof(char) * k);

            }
        }
    } while (flag);
}

int main(int argc, char const *argv[]) {
    char **eligibiles = (char **)malloc(sizeof(char *));  // Dinamic array of elegibles words
    scanf("%d\n", &k);
    char *ref = (char *)malloc(sizeof(char) * k);  // Reference word
    int max;                                       // Maximum number of attempts allowed

    fillEligibles(eligibiles);

    char flag = getchar();
    while (flag != EOF) {
        char command = getchar();
        ignoreLine();

        switch (command) {
            case 'n':  // nuova_partita
                fgets(ref, k + 1, stdin);
                scanf("%d\n", &max);
                play(max, ref, eligibiles);
                break;
            case 'i':  // inserisci_inizio
                fillEligibles(eligibiles);
                ignoreLine();
                break;
            default:
                break;
        }
        flag = EOF;
    }
    return 0;
}