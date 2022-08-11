#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PROMPTALPH
//#define PROMPTREMOVE
//#define PROMPTTREE
//#define MINA
//#define PROMPTSORT

// Typedef for a linked list node
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

typedef struct {
    int memb;       // If the char is NOT a member of ref
    Positions pos;  // Mandatory and Forbidden positions
    int minApp;     // Minimum number of appearances
    int exactApp;   // Exact number of appearances
} Filter;
typedef struct CH {
    char c;    // The char
    int app;   // How many times it appears
    List pos;  // Positions where it appears
    struct CH *next;
} Record;
typedef Record *CharFilter;

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
    char *ordered;
    int used;
    CharFilter chars;
} tree_node;
typedef tree_node *tree;

int k;      // lenght of the words
int e = 0;  // Number of eligibles words

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
void pos_insert(Positions *l, int n, int flag) {
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
        if (t->used) {
            e++;
        }
        t->used = 0;
        reset_used(t->right);
    }
}

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
    if (l < n && (a->w[l] > a->w[i] || (a->w[l] == a->w[i] && a->pos[l] > a->pos[i]))) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < n && (a->w[r] > a->w[largest] || (a->w[r] == a->w[largest] && a->pos[r] > a->pos[largest]))) {
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
    Word ord;
    ord.pos = (int *)malloc(sizeof(int) * (k + 1));
    ord.w = (char *)malloc(sizeof(char) * (k + 1));
    for (int i = 0; i < k; i++) {
        ord.pos[i] = i;
        ord.w[i] = key[i];
    }
    ord.w[k] = '\0';
    heap_sort(&ord, k);
    z->ordered = (char *)malloc(sizeof(char) * (k + 1));
    strcpy(z->ordered, ord.w);
    z->chars = NULL;
    int a = 0;  // Number of appearances
    List l = NULL;
    for (int i = 0; i <= k; i++) {
        if ((i != 0 && ord.w[i] != ord.w[i - 1]) || i == k) {
            // New char
            Record *new = (Record *)malloc(sizeof(Record));
            new->c = ord.w[i - 1];
            new->app = a;
            new->pos = l;
            head_insert(&(z->chars), new);

            l = NULL;
            a = 0;
        }
        if (i != k) {
            a++;
            order_insert(&l, ord.pos[i]);
        }
    }

    free(ord.w);
    free(ord.pos);
    z->key = key;
    z->used = 0;
    e++;
#ifdef PROMPTREMOVE
    // printf("Inserting %s (e=%d)\n", key, e);
#endif
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

#ifdef PROMPTTREE
    FILE *fptree = fopen("../debug/tree.txt", "a");
    fprintf(fptree, "Parola %s (%d):\n", key, e);
    fprintf(fptree, "Colore: %s\n", z->color == RED ? "RED" : "BLACK");
    fprintf(fptree, "ordinata: %s\n", z->ordered);
    fprintf(fptree, "Chiavi:\n");
    for (CharFilter c = z->chars; c != NULL; c = c->next) {
        fprintf(fptree, "\t-%c apparsa nelle posizioni ", c->c);
        for (List l = c->pos; l != NULL; l = l->next) {
            fprintf(fptree, "%d, ", l->n);
        }
        fprintf(fptree, "\n");
    }
    fprintf(fptree, "\n");
    fclose(fptree);
#endif
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
        excludeOthers(t->right, f);

        if (!t->used) {
            int i = 63;
            for (CharFilter chars = t->chars; chars != NULL; chars = chars->next) {
                int hh = h(chars->c);
                while (i >= 0 && hh != i) {
                    if (f[i].memb == 0) {
                        // There is a char > chars->c that should appear but it doesn't
                        t->used = 1;
                        e--;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c does not appear but it should, > (e=%d)\n", t->key, hm1(i), e);
#endif
                        return;
                    }
                    i--;
                }
                if (f[i].memb == -1) continue;
                if (f[i].memb == 1) {
                    // chars->c should not appear
                    t->used = 1;
                    e--;
#ifdef PROMPTREMOVE
                    printf("Removing %s because %c should not appear but it does (e=%d)\n", t->key, chars->c, e);
#endif
                    return;
                }
                if (f[i].memb == 0) {
                    // Ok, chars->c should appear and it does
                    if (f[i].exactApp != -1 && chars->app != f[i].exactApp) {
                        // It should appear exactly f[i].exactApp times but it doesn't
                        t->used = 1;
                        e--;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should appear exactly %d times and not %d (e=%d)\n", t->key, chars->c, f[i].exactApp, chars->app, e);
#endif
                        return;
                    } else if (f[i].exactApp == -1 && chars->app < f[i].minApp) {
                        // It should appear at least f[i].minApp times but it doesn't
                        t->used = 1;
                        e--;
#ifdef PROMPTREMOVE
                        printf("Removing %s because %c should appear at least %d times but it appears %d (e=%d)\n", t->key, chars->c, f[i].minApp, chars->app, e);
#endif
                        return;
                    }

                    List appear = chars->pos;  // Positions where it appear
                    for (Positions list = f[i].pos; list != NULL; list = list->next) {
                        if (appear != NULL) {
                            while (appear != NULL && appear->n < list->n) appear = appear->next;
                            if (appear != NULL) {
                                if (appear->n == list->n && !list->f) {
                                    // It appear in a forbidden position
                                    t->used = 1;
                                    e--;
#ifdef PROMPTREMOVE
                                    printf("Removing %s because %c apprear in position %d but it shouldn't (e=%d)\n", t->key, chars->c, list->n, e);
#endif
                                    return;
                                } else if (appear->n != list->n && list->f) {
                                    // It doesn't appear in a mandatory position
                                    t->used = 1;
                                    e--;
#ifdef PROMPTREMOVE
                                    printf("Removing %s because %c does not appear in position %d but it should a (e=%d)\n", t->key, chars->c, list->n, e);
#endif
                                    return;
                                }
                            } else if (list->f) {
                                // It doesn't appear in a mandatory position
                                t->used = 1;
                                e--;
#ifdef PROMPTREMOVE
                                printf("Removing %s because %c does not appear in position %d but it should b (e=%d)\n", t->key, chars->c, list->n, e);
#endif
                                return;
                            }
                        }else if (list->f) {
                                // It doesn't appear in a mandatory position
                                t->used = 1;
                                e--;
#ifdef PROMPTREMOVE
                                printf("Removing %s because %c does not appear in position %d but it should c (e=%d)\n", t->key, chars->c, list->n, e);
#endif
                                return;
                            }
                    }
                    i--;
                }
            }
            while (i >= 0) {
                if (f[i].memb == 0) {
                    // There is a char < any char in t->key that should appear but doesn't
                    t->used = 1;
                    e--;
#ifdef PROMPTREMOVE
                    printf("Removing %s because %c does not appear but it should, < (e=%d)\n", t->key, hm1(i), e);
#endif
                    return;
                }
                i--;
            }
        }
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
#ifdef PROMPTTREEE
        FILE *f = fopen("../debug/tree.txt", "a");
        fprintf(f, "\tFreeing %c\n", head->c);
        fclose(f);
#endif
        free(head);
    }
}
void freeTree(tree root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
#ifdef PROMPTTREEE
        FILE *f = fopen("../debug/tree.txt", "a");
        fprintf(f, "\nFreeing %s\n", root->key);
        fclose(f);
#endif
        free(root->key);
        free(root->ordered);
        freeCharFilter(root->chars);
        free(root);
    }
}
void freeAlphabet(Positions *l) {
    if (*l != NULL) {
        freeAlphabet(&((*l)->next));
        free(*l);
    }
}

void play(int max, char *ref, tree *elig) {
    int attempts = 0;     // Number of attempts done until now
    int flag = 1;         // If the word must be compared or not
    Filter alphabet[64];  // Array of filters, one for each character

#ifdef PROMPTALPH
    FILE *fpalphabet = fopen("../debug/alphabet.txt", "w");
    fprintf(fpalphabet, "NUOVA PARTITA\n\n");
    // printf("\n\n==============\nNUOVA PARTITA\n==============\n\n");
#endif

    // Set every minappears to 0
    for (int i = 0; i < 64; i++) {
        alphabet[i].minApp = 0;
        alphabet[i].pos = NULL;
        alphabet[i].exactApp = -1;
        alphabet[i].memb = -1;
    }

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
                    fillEligibles(elig);
                    ignoreLine();
                    excludeOthers(*elig, alphabet);
                } else if (c == 's') {
                    // +stampa_filtrate
                    ignoreLine();
                    inorder(*elig);
                }
            } else {
                // It is a word
                char *orderRef = (char *)malloc(sizeof(char) * (k + 1));  // The reference word in the order of the alphabet
                char *output = (char *)malloc(sizeof(char) * (k + 1));    // String of comparison
                output[k] = '\0';
                char *unWord = (char *)malloc(sizeof(char) * (k + 1));  // Input word unordered
                Word ordWord;                                           // Input word ordered
                int hw = 0;                                             // Head word
                int hr = 0;                                             // Head reference
                int counter;                                            // Number of characters found in the right place

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
                    printf("not_exists\n");
                } else {
                    counter = 0;
                    if (!node->used) {
                        node->used = 1;
                        e--;
                    }

                    attempts++;

                    // Update the filter
                    char tempMinApps[64];  // Appearences of each char in this string
                    for (int i = 0; i < 64; i++) {
                        tempMinApps[i] = 0;
                    }

                    for (int i = 0; i < k; i++) {
                        int tempCharPos = h(ordWord.w[i]);
                        if (ordWord.w[i] == orderRef[i]) {
                            counter++;
                            alphabet[tempCharPos].memb = 0;
                            tempMinApps[tempCharPos]++;
                            if (tempMinApps[tempCharPos] > alphabet[tempCharPos].minApp)
                                alphabet[tempCharPos].minApp = tempMinApps[tempCharPos];
                            pos_insert(&alphabet[tempCharPos].pos, ordWord.pos[i], 1);

#ifdef MINA
                            printf("Stringa %s: il carattere %c è apparso 'correttamente' %d volte, contro le %d che vi sono nel filtro\n", unWord, ordWord.w[i], alphabet[tempCharPos].minApp, alphabet[tempCharPos].minApp);
#endif
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
#ifdef PROMPTSORT
                        for (int i = 0; i < k; i++) {
                            printf("%c ", ordWord.w[i]);
                            printf("%d\n", ordWord.pos[i]);
                        }

#endif

                        do {
                            // Filters for / , |
                            int tempCharPos = h(ordWord.w[hw]);
                            if (orderRef[hr] == ordWord.w[hw]) {
                                // Right caracter in the wrong position
                                output[ordWord.pos[hw]] = '|';
                                alphabet[tempCharPos].memb = 0;
                                tempMinApps[tempCharPos]++;

#ifdef MINA
                                printf("Stringa %s: il carattere %c è apparso 'correttamente' %d volte, contro le %d che vi sono nel filtro\n", unWord, ordWord.w[hw], tempMinApps[tempCharPos], alphabet[tempCharPos].minApp);
#endif
                                if (tempMinApps[tempCharPos] > alphabet[tempCharPos].minApp)
                                    alphabet[tempCharPos].minApp = tempMinApps[tempCharPos];
                                pos_insert(&alphabet[tempCharPos].pos, ordWord.pos[hw], 0);
                                hr++;
                                hw++;
                            } else if (orderRef[hr] > ordWord.w[hw]) {
                                // Wrong character
                                output[ordWord.pos[hw]] = '/';
                                pos_insert(&alphabet[tempCharPos].pos, ordWord.pos[hw], 0);
                                alphabet[tempCharPos].exactApp = alphabet[tempCharPos].minApp;
                                if (alphabet[tempCharPos].minApp == 0) alphabet[tempCharPos].memb = 1;
                                hw++;
                            } else {
                                hr++;
                            }
                            if (hr == k || orderRef[hr] == '~') {
                                // Remaining characters
                                for (; hw < k && ordWord.w[hw] != '~'; hw++) {
                                    output[ordWord.pos[hw]] = '/';
                                    pos_insert(&alphabet[h(ordWord.w[hw])].pos, ordWord.pos[hw], 0);
                                    alphabet[h(ordWord.w[hw])].exactApp = alphabet[h(ordWord.w[hw])].minApp;
                                    if (alphabet[h(ordWord.w[hw])].minApp == 0) alphabet[h(ordWord.w[hw])].memb = 1;
                                }
                            }
                        } while (hw < k && hr < k && orderRef[hr] != '~' && ordWord.w[hw] != '~');
                        printf("%s\n", output);
                        excludeOthers(*elig, alphabet);
                        printf("%d\n", e);
                    }

#ifdef PROMPTALPH
                    fprintf(fpalphabet, "Dopo la lettura della parola %s abbiamo appreso che:\n", unWord);
                    char a[64] = {'-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
                    for (int i = 0; i < 64; i++) {
                        if (alphabet[i].memb != -1) {
                            fprintf(fpalphabet, "Character %c:\n", a[i]);
                            if (alphabet[i].memb)
                                fprintf(fpalphabet, "    not member\n");
                            else {
                                fprintf(fpalphabet, "Member: %d\n", alphabet[i].memb);
                                fprintf(fpalphabet, "MinApp: %d\n", alphabet[i].minApp);
                                fprintf(fpalphabet, "ExactApp: %d\n", alphabet[i].exactApp);
                                fprintf(fpalphabet, "Cert: ");
                                for (Positions j = alphabet[i].pos; j != NULL; j = j->next)
                                    if (j->f) fprintf(fpalphabet, "%d, ", j->n);
                                fprintf(fpalphabet, "\nForb: ");
                                for (Positions j = alphabet[i].pos; j != NULL; j = j->next)
                                    if (!j->f) fprintf(fpalphabet, "%d, ", j->n);
                            }
                            fprintf(fpalphabet, "\n\n\n");
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
    // Free dynamically allocated memory
    for (int i = 0; i < 64; i++) {
        freeAlphabet(&alphabet[i].pos);
    }
#ifdef PROMPTALPH
    fclose(fpalphabet);
#endif
    return;
}

int main(int argc, char const *argv[]) {
#ifdef PROMPTTREE
    FILE *aaa = fopen("../debug/tree.txt", "w");
    fprintf(aaa, "Tree:\n");
    fclose(aaa);
#endif
    tree eligibiles = NULL;
    if (scanf("%d\n", &k) == EOF) return -1;
    char *ref = (char *)malloc(sizeof(char) * (k + 1));  // Reference word
    int max;                                             // Maximum number of attempts allowed

    fillEligibles(&eligibiles);

    char flag = getchar();
    while (flag != EOF && flag != '$') {
        char command = getchar();
        ignoreLine();

        switch (command) {
            case 'n':
                // +nuova_partita
                if (fgets(ref, k + 1, stdin) == NULL) break;
                if (scanf("%d\n", &max) == EOF) break;
                reset_used(eligibiles);
                printf("E = %d\n", e);
                play(max, ref, &eligibiles);
                break;
            case 'i':
                // +inserisci_inizio
                fillEligibles(&eligibiles);
                ignoreLine();
                break;
            default:
                break;
        }
        flag = getchar();
    }

    freeTree(eligibiles);
    free(ref);
    return 0;
}