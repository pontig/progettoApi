#include <stdio.h>
#include <stdlib.h>

// Typedef to assign \, |
typedef struct {
    char *w;
    int *pos;
} Word;

int k;

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
    if (l < n && (a->w[l] > a->w[i] || (a->w[l] == a->w[i] && a->pos[l] < a->pos[i]))) {
        largest = l;
    } else {
        largest = i;
    }
    if (r < n && (a->w[r] > a->w[largest] || (a->w[r] == a->w[largest] && a->pos[r] < a->pos[largest]))) {
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

// Main: get a string of type Word and sort it
int main() {
    Word a;
    scanf("%d\n", &k);

    a.w = (char *)malloc((k + 1) * sizeof(char));
    a.pos = (int *)malloc((k + 1) * sizeof(int));

    for (int paosjd = 0; paosjd < 15; paosjd++) {
        /* code */

        for (int i = 0; i < k + 1; i++) {
            scanf("%c", &a.w[i]);
            a.pos[i] = i;
        }

        printf("%s\n", a.w);
        heap_sort(&a, k);

        for (int i = 0; i < k; i++) {
            printf("%c", a.w[i]);
        }
        printf("\n");
        for (int i = 0; i < k; i++) {
            printf("%d", a.pos[i]);
        }
        printf("\n===============\n");
    }

    return 0;
}
