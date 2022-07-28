#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int memb;   // If the char is NOT a member of ref
    int *cert;  // Positions where the character is
    int icert;
    int *forb;  // Positions where the character must not be
    int iforb;
    int minApp;    // Minimum number of appearances
    int exactApp;  // Exact number of appearances
} Filter;

// Typedef to assign \, |
typedef struct {
    char w;
    int pos;
} Word;

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
    int used;
    rb_color color;
} tree_node;

// Typedef red-black tree
typedef tree_node *tree;

int k;      // lenght of the words
int e = 0;  // Number of eligibles words

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

// Red-black tree insert
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
void rb_insert(tree *t, char *key) {
    tree_node *y = NULL;
    tree_node *x = *t;
    tree_node *z = (tree)malloc(sizeof(tree_node));
    z->key = key;
    z->used = 0;
    e++;
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
        if (!t->used) {
            printf("%s\n", t->key);
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

// Reset all used flags to 0
void reset_used(tree t) {
    if (t != NULL) {
        reset_used(t->left);
        if (t->used) e++;
        t->used = 0;
        reset_used(t->right);
    }
}

// Heap sort an array of Word type
void swap(Word *a, int pos1, int pos2) {
    Word aux = a[pos1];
    a[pos1] = a[pos2];
    a[pos2] = aux;
}
void max_heapify(Word *a, int i) {
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int largest;
    if (l < e && a[l].w > a[i].w) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < e && a[r].w > a[largest].w) {
        largest = r;
    }
    if (largest != i) {
        swap(a, i, largest);
        max_heapify(a, largest);
    }
}
void build_max_heap(Word *a, int n) {
    int i;
    for (i = n / 2; i >= 0; i--) {
        max_heapify(a, n);
    }
}
void heap_sort(Word *a, int n) {
    build_max_heap(a, n);
    for (int i = n - 1; i > 0; i--) {
        swap(a, 0, i);
        max_heapify(a, 0);
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
    if (getline(&line, &len, stdin) == -1) return;
    free(line);
    return;
}

void fillEligibles(tree *t) {
    do {
        char *i = (char *)malloc(sizeof(char) * k);
        char c = getchar();
        ungetc(c, stdin);
        if (c != '+') {
            if (fgets(i, k + 1, stdin) == NULL) break;
            getchar();
            rb_insert(t, i);
            e++;
        } else {
            return;
        }
    } while (1);
}

void excludeOthers(tree t, Filter *f) {
    if (t != NULL) {
        excludeOthers(t->left, f);
        if (t->used == 0) {
            int flag2 = 1;                                    // If the eligible word respects the filter
            int c;                                            // Counts the number of the same character in the word
            Word *actual = (Word *)malloc(sizeof(Word) * k);  // The word to be checked, in Word type
            char letter;
            for (int i = 0; i < k; i++) {
                letter = t->key[i];
                actual[i].pos = i;
                actual[i].w = letter;
            }
            heap_sort(actual, k);
            for (int i = 0; i < k && flag2; i++) {
                // Check if the character cannot appear
                if (f[h(actual[i].w)].memb) {
                    flag2 = 0;
                    e--;
                    t->used = 1;
                    return;
                }

                // Check if the character isn't in positions where it shouldn't appear
                for (int j = 0; j < f[h(actual[i].w)].icert; j++) {
                    if (actual[i].pos == f[h(actual[i].w)].cert[j]) {
                        flag2 = 0;
                        e--;
                        t->used = 1;
                        return;
                    }
                }

                // Check if the character does not appear in position where it should appear
                for (int j = 0; j < f[h(actual[i].w)].iforb; j++) {
                    if (actual[i].pos != f[h(actual[i].w)].cert[j]) {
                        flag2 = 0;
                        e--;
                        t->used = 1;
                        return;
                    }
                }

                // Check minimum appearances
                c = 0;
                do {
                    c++;
                    i++;
                } while (actual[i].w == actual[i - 1].w);
                if (c < f[h(actual[i].w)].minApp) {
                    flag2 = 0;
                    e--;
                    t->used = 1;
                    return;
                }

                // Check exact appearances
                if (f[h(actual[i].w)].minApp != -1 && c != f[h(actual[i].w)].exactApp) {
                    flag2 = 0;
                    e--;
                    t->used = 1;
                    return;
                }
            }
        }
        excludeOthers(t->right, f);
    }
}

void play(int max, char *ref, tree *elig) {
    int attempts = 0;  // Number of attempts done until now
    int flag = 1;      // If the word must be compared or not

    Filter alphabet[64];  // Array of filters, one for each character

    // Set every minappears to 0
    for (int i = 0; i < 64; i++) {
        alphabet[i].minApp = 0;
        alphabet[i].icert = 0;
        alphabet[i].iforb = 0;
        alphabet[i].exactApp = -1;
    }

    do {
        // Begin the comparisons
        char c = getchar();
        ungetc(c, stdin);
        if (c == EOF || attempts >= max) {
            flag = 0;
            if (attempts >= max) {
                printf("ko\n");
            }
            return;
        } else {
            if (c == '+') {
                // inserisci_inizio || stampa_filtrate
                getchar();
                c = getchar();
                if (c == 'i') {
                    // inserisci_inizio
                    ignoreLine();
                    fillEligibles(elig);
                    ignoreLine();
                    excludeOthers(*elig, alphabet);
                } else if (c == 's') {
                    inorder(*elig);
                }
            } else {
                // It is a word
                Word *orderRefMod = (Word *)malloc(sizeof(Word) * k);  // Temporary array of reference word
                char *orderRef = (char *)malloc(sizeof(char) * k);     // The reference word in the order of the alphabet
                int hw = 0;                                            // Head word
                int hr = 0;                                            // Head reference

                // Fill the temporary array of reference word, put it in the order of the alphabet and put it in the orderRef array
                for (int i = 0; i < k; i++) {
                    orderRefMod[i].w = ref[i];
                }
                heap_sort(orderRefMod, k);
                for (int i = 0; i < k; i++) {
                    orderRef[i] = orderRefMod[i].w;
                }
                free(orderRefMod);
                char *output = (char *)malloc(sizeof(char) * k);   // String of comparison
                Word *ordWord = (Word *)malloc(sizeof(Word) * k);  // Word to be compared ordered
                char *unWord = (char *)malloc(sizeof(char) * k);   // Word to be compared unordered
                int counter;                                       // Number of characters found in the right place
                for (int i = 0; i < k; i++) {
                    if (scanf("%c", &ordWord[i].w) == EOF) break;
                    unWord[i] = ordWord[i].w;
                    ordWord[i].pos = i;
                }
                getchar();
                tree_node *node = rb_search(*elig, unWord);
                if (node == NULL) {
                    printf("not_exists");
                } else {
                    counter = 0;
                    node->used = 1;
                    e--;
                    attempts++;
                    heap_sort(ordWord, k);
                    do {
                        if (ordWord[hw].w == orderRef[hr]) {
                            if (unWord[ordWord[hw].pos] == orderRef[hr]) {
                                output[ordWord[hw].pos] = '+';
                                alphabet[h(orderRef[hr])].cert[alphabet[h(orderRef[hr])].icert++] = ordWord[hw].pos;
                                counter++;
                            } else {
                                output[ordWord[hw].pos] = '|';
                                alphabet[h(orderRef[hr])].forb[alphabet[h(orderRef[hr])].iforb++] = ordWord[hw].pos;
                            }
                            alphabet[h(orderRef[hr])].memb = 0;
                            alphabet[h(orderRef[hr])].minApp++;
                            hw++;
                            hr++;
                        } else {
                            if (ordWord[hw].w > orderRef[hr])
                                hr++;
                            else if (ordWord[hw].w < orderRef[hr]) {
                                // La lettera in word non compare in reference oppure è apparsa troppe volte
                                output[ordWord[hw].pos] = '/';
                                alphabet[h(ordWord[hr].w)].exactApp = alphabet[h(ordWord[hr].w)].minApp;
                                if (alphabet[h(ordWord[hr].w)].exactApp == 0) {
                                    alphabet[h(ordWord[hr].w)].memb = 1;
                                }
                                hw++;
                            }
                        }
                        if (hr == k) {
                            // The remaining characters in word are not in reference
                            for (hw = hw; hw < k; hw++) {
                                output[ordWord[hw].pos] = '/';
                                alphabet[h(ordWord[hw].w)].exactApp = alphabet[h(ordWord[hw].w)].minApp;
                                if (alphabet[h(ordWord[hw].w)].exactApp == 0) {
                                    alphabet[h(ordWord[hw].w)].memb = 1;
                                }
                            }
                        }
                    } while (hw < k && hr < k);
                    if (counter == k) {
                        printf("ok\n");
                    } else {
                        printf("%s\n", output);
                        printf("%d\n", e);
                    }
                }
                excludeOthers(*elig, alphabet);
            }
        }
    } while (flag);
}

int main(int argc, char const *argv[]) {
    tree eligibiles = NULL;
    if (scanf("%d\n", &k) == EOF) return -1;
    char *ref = (char *)malloc(sizeof(char) * k);  // Reference word
    int max;                                       // Maximum number of attempts allowed

    fillEligibles(&eligibiles);

    char flag = getchar();
    while (flag != EOF) {
        char command = getchar();
        ignoreLine();

        switch (command) {
            case 'n':  // nuova_partita
                if (fgets(ref, k + 1, stdin) == NULL) break;
                if (scanf("%d\n", &max) == EOF) break;
                reset_used(eligibiles);
                play(max, ref, &eligibiles);
                break;
            case 'i':  // inserisci_inizio
                fillEligibles(&eligibiles);
                ignoreLine();
                break;
            default:
                break;
        }
        flag = getchar();
    }
    return 0;
}