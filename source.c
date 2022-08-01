#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG
//#define OPEN

// Typedef for a linked list node
typedef struct EL {
    int n;
    struct EL *next;
} Element;
typedef Element *List;

typedef struct {
    int memb;      // If the char is NOT a member of ref
    List cert;     // Positions where the character is
    List forb;     // Positions where the character must not be
    int minApp;    // Minimum number of appearances
    int exactApp;  // Exact number of appearances
} Filter;

// Typedef to assign \, |
typedef struct {
    char *w;
    int *pos;
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
int faffa = 0;

// Head insert a new node in the list, type void
void head_insert(List *l, int n) {
    Element *new = (Element *)malloc(sizeof(Element));
    new->n = n;
    new->next = *l;
    *l = new;
}

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

//

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
    if (l < n && a->w[l] > a->w[i]) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < n && a->w[r] > a->w[largest]) {
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
#ifdef DEBUG
    printf("%s", line);
#endif
    free(line);
    return;
}

void fillEligibles(tree *t) {
    do {
        char *i = (char *)malloc(sizeof(char) * (k + 1));
        char c = getchar();
        ungetc(c, stdin);
        if (c != '+') {
            if (fgets(i, k + 1, stdin) == NULL) break;
            getchar();
            rb_insert(t, i);
        } else {
            free(i);
            return;
        }
    } while (1);
}

void excludeOthers(tree t, Filter *f) {
    if (t != NULL) {
        excludeOthers(t->left, f);

        if (!t->used) {
            char *str = (char *)malloc(sizeof(char) * (k + 1));  // The word taken from the tree, in alphabetical order
            strcpy(str, t->key);
            quick_sort(str, 0, k - 1);

            int i = k - 1;          // Position in the string t->key
            int app = -1;           // Number of appearances of a certain character
            List mandatory = NULL;  // List of mandatory positions
            List forbidden = NULL;  // List of forbidden positions
            do {
                char tempChar = str[i];
                int tempPos = h(tempChar);
                if (i == k - 1 || tempChar != str[i + 1]) {
                    // It is a new character
                    // Previous character: exact or minimum appearances
                    if (i != k - 1 && f[h(str[i + 1])].memb != -1) {
                        if (f[h(str[i + 1])].exactApp != -1) {
                            if (app != f[h(str[i + 1])].exactApp) {
                                t->used = 1;
                                e--;
#ifdef DEBUG
                                printf("Removed %s because %c does not appear %d times\n", t->key, tempChar, f[h(str[i + 1])].exactApp);
#endif
                                return;
                            }
                        } else if (app < f[h(str[i + 1])].minApp) {
                            t->used = 1;
                            e--;
#ifdef DEBUG
                            printf("Removed %s because %c does not appear at least %d times\n", t->key, tempChar, f[h(str[i + 1])].minApp);
#endif
                            return;
                        }
                    }

                    // Now the "new" character
                    if (f[tempPos].memb == -1) {
                        // We haven't meet yet this char; therefore we cannot filter it
                        i--;
                        continue;
                    }
                    if (f[tempPos].memb == 1) {
                        // The char should not appear
                        t->used = 1;
                        e--;
#ifdef DEBUG
                        printf("Removed %s because %c souldn't appear in the word\n", t->key, tempChar);
#endif
                        return;
                    }
                    app = 1;
                    mandatory = f[tempPos].cert;
                    forbidden = f[tempPos].forb;

                    while (forbidden != NULL) {
                        if (tempChar == t->key[forbidden->n]) {
                            t->used = 1;
                            e--;
#ifdef DEBUG
                            printf("Removed %s because %c souldn't appear in position %d\n", t->key, tempChar, forbidden->n);
#endif
                            return;
                        }
                        forbidden = forbidden->next;
                    }

                    while (mandatory != NULL) {
                        if (tempChar != t->key[mandatory->n]) {
                            t->used = 1;
                            e--;
#ifdef DEBUG
                            printf("Removed %s because %c should appear in position %d\n", t->key, tempChar, mandatory->n);
#endif
                            return;
                        }
                        mandatory = mandatory->next;
                    }

                } else
                    app++;
                i--;
            } while (i >= 0);
            free(str);
        }
        excludeOthers(t->right, f);
    }
}

void play(int max, char *ref, tree *elig) {
    int attempts = 0;     // Number of attempts done until now
    int flag = 1;         // If the word must be compared or not
    Filter alphabet[64];  // Array of filters, one for each character

#ifdef OPEN
    FILE *fp = fopen("alphabet.txt", "w");
#endif

    // Set every minappears to 0
    for (int i = 0; i < 64; i++) {
        alphabet[i].minApp = 0;
        alphabet[i].cert = NULL;
        alphabet[i].forb = NULL;
        alphabet[i].exactApp = -1;
        alphabet[i].memb = -1;
    }

    do {
        char c = getchar();
        ungetc(c, stdin);
        if (c == '$') return;
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
                    ignoreLine();
                    inorder(*elig);
                }
            } else {
                // It is a word
                char *orderRef = (char *)malloc(sizeof(char) * (k + 1));  // The reference word in the order of the alphabet
                char *output = (char *)malloc(sizeof(char) * k);          // String of comparison
                char *unWord = (char *)malloc(sizeof(char) * k);          // Input word unordered
                Word ordWord;                                             // Input word ordered
                int hw = 0;                                               // Head word
                int hr = 0;                                               // Head reference
                int counter;                                              // Number of characters found in the right place

                // Copy reference word in the order of the alphabet in orderRef
                strcpy(orderRef, ref);
                ordWord.w = malloc(sizeof(char) * k);
                ordWord.pos = malloc(sizeof(int) * k);

                // Scan the input word
                for (int i = 0; i < k; i++) {
                    if (scanf("%c", &ordWord.w[i]) == EOF) break;
                    unWord[i] = ordWord.w[i];
                    ordWord.pos[i] = i;
                }
                getchar();

                tree_node *node = rb_search(*elig, unWord);

                if (node == NULL) {
                    printf("not_exists\n");
                } else {
                    counter = 0;
                    node->used = 1;
                    e--;
                    attempts++;

                    // Update the filter
                    for (int i = 0; i < k; i++) {
                        if (ordWord.w[i] == orderRef[i]) {
                            counter++;
                            alphabet[h(ordWord.w[i])].memb = 0;
                            alphabet[h(ordWord.w[i])].minApp++;
                            head_insert(&alphabet[h(ordWord.w[i])].cert, ordWord.pos[i]);

                            output[i] = '+';
                            ordWord.w[i] = '~';
                            orderRef[i] = '~';
                        }
                    }
                    if (counter == k) {
                        printf("ok\n");
                    } else {
                        quick_sort(orderRef, 0, k - 1);
                        qs_mod(&ordWord, 0, k - 1);
                        do {
                            // Filters for / , |
                            if (orderRef[hr] == ordWord.w[hw]) {
                                // Right caracter in the wrong position
                                output[ordWord.pos[hw]] = '|';
                                alphabet[h(ordWord.w[hw])].memb = 0;
                                alphabet[h(ordWord.w[hw])].minApp++;
                                head_insert(&alphabet[h(ordWord.w[hw])].forb, ordWord.pos[hw]);
                                hr++;
                                hw++;
                            } else if (orderRef[hr] > ordWord.w[hw]) {
                                // Wrong character
                                output[ordWord.pos[hw]] = '/';
                                head_insert(&alphabet[h(ordWord.w[hw])].forb, ordWord.pos[hw]);
                                alphabet[h(ordWord.w[hw])].exactApp = alphabet[h(ordWord.w[hw])].minApp;
                                if (alphabet[h(ordWord.w[hw])].minApp == 0) alphabet[h(ordWord.w[hw])].memb = 1;
                                hw++;
                            } else {
                                hr++;
                            }
                            if (hr == k || orderRef[hr] == '~') {
                                for (; hw < k && ordWord.w[hw] != '~'; hw++) {
                                    output[ordWord.pos[hw]] = '/';
                                    head_insert(&alphabet[h(ordWord.w[hw])].forb, ordWord.pos[hw]);
                                    alphabet[h(ordWord.w[hw])].exactApp = alphabet[h(ordWord.w[hw])].minApp;
                                    if (alphabet[h(ordWord.w[hw])].minApp == 0) alphabet[h(ordWord.w[hw])].memb = 1;
                                }
                            }
                        } while (hw < k && hr < k && orderRef[hr] != '~' && ordWord.w[hw] != '~');
                        printf("%s\n", output);
                        excludeOthers(*elig, alphabet);
                        printf("%d\n", e);
                    }

#ifdef OPEN
                    fprintf(fp, "Dopo la lettura della parola %s abbiamo appreso che:\n", unWord);
                    char a[64] = {'-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
                    // faffa++;
                    for (int i = 0; i < 64; i++) {
                        if (alphabet[i].memb != -1) {
                            fprintf(fp, "Character %c:\n", a[i]);
                            if (alphabet[i].memb)
                                fprintf(fp, "    not member\n");
                            else {
                                fprintf(fp, "Member: %d\n", alphabet[i].memb);
                                fprintf(fp, "MinApp: %d\n", alphabet[i].minApp);
                                fprintf(fp, "ExactApp: %d\n", alphabet[i].exactApp);
                                fprintf(fp, "Cert: ");
                                for (List j = alphabet[i].cert; j != NULL; j = j->next)
                                    fprintf(fp, "%d, ", j->n);
                                fprintf(fp, "\nForb: ");
                                for (List j = alphabet[i].forb; j != NULL; j = j->next)
                                    fprintf(fp, "%d, ", j->n);
                            }
                            fprintf(fp, "\n\n\n");
                        }
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
}

void freeMemory(tree root) {
    if (root != NULL) {
        freeMemory(root->left);
        freeMemory(root->right);
        free(root->key);
        free(root);
    }
}

int main(int argc, char const *argv[]) {
    tree eligibiles = NULL;
    if (scanf("%d\n", &k) == EOF) return -1;
    char *ref = (char *)malloc(sizeof(char) * (k + 1));  // Reference word
    int max;                                             // Maximum number of attempts allowed

    fillEligibles(&eligibiles);

    char flag = getchar();
    while (flag != '$') {
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
    freeMemory(eligibiles);
    free(ref);
    return 0;
}