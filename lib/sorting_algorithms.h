// Swap two elements
void swap(int *a, int *b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

// Insertion sort array A
void insertion_sort(int A[], int n) {
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = A[i];
        j = i - 1;
        while (j >= 0 && A[j] > key) {
            A[j + 1] = A[j];
            j = j - 1;
        }
        A[j + 1] = key;
    }
}

// efine max integer representable by int type
#define INT_MAX 2147483647

void merge(int A[], int p, int q, int r) {
    int n1 = q - p + 1;
    int n2 = r - q;
    int L[n1 + 1];
    int R[n2 + 1];
    for (int i = 0; i < n1; i++) {
        L[i] = A[p + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = A[q + j + 1];
    }
    L[n1] = INT_MAX;
    R[n2] = INT_MAX;
    int i = 0;
    int j = 0;
    for (int k = p; k <= r; k++) {
        if (L[i] <= R[j]) {
            A[k] = L[i];
            i = i + 1;
        } else {
            A[k] = R[j];
            j = j + 1;
        }
    }
}

// Merge sort array A
void merge_sort(int A[], int p, int r) {
    if (p < r) {
        int q = (p + r) / 2;
        merge_sort(A, p, q);
        merge_sort(A, q + 1, r);
        merge(A, p, q, r);
    }
}

// Max heapify array A
void max_heapify(int A[], int n, int i) {
    int l, r, largest;
    l = 2 * i + 1;
    r = 2 * i + 2;
    if (l < n && A[l] > A[i])
        largest = l;
    else
        largest = i;
    if (r < n && A[r] > A[largest])
        largest = r;
    if (largest != i) {
        swap(&A[i], &A[largest]);
        max_heapify(A, n, largest);
    }
}

// Build max heap array A
void build_max_heap(int A[], int n) {
    int i;
    for (i = n / 2 - 1; i >= 0; i--)
        max_heapify(A, n, i);
}

// Heap sort array A
void heap_sort(int A[], int n) {
    build_max_heap(A, n);
    int i;
    for (i = n / 2 - 1; i >= 0; i--)
        max_heapify(A, n, i);
    for (i = n - 1; i >= 0; i--) {
        swap(&A[0], &A[i]);
        max_heapify(A, i, 0);
    }
}

// Partition array A
int partition(int A[], int n, int p, int r) {
    int x, i, j;
    x = A[r];
    i = p - 1;
    for (j = p; j < r; j++)
        if (A[j] <= x) {
            i++;
            swap(&A[i], &A[j]);
        }
    swap(&A[i + 1], &A[r]);
    return i + 1;
}

// Quick sort array A
void quick_sort(int A[], int n, int p, int r) {
    int q;
    if (p < r) {
        q = partition(A, n, p, r);
        quick_sort(A, n, p, q - 1);
        quick_sort(A, n, q + 1, r);
    }
}

// Counting sort array A
void counting_sort(int A[], int B[], int n) {
    int i, *C;
    B = (int *)malloc(n * sizeof(int));
    C = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        C[i] = 0;
    for (i = 0; i < n; i++)
        C[A[i]]++;
    for (i = 1; i < n; i++)
        C[i] += C[i - 1];
    for (i = n - 1; i >= 0; i--) {
        B[C[A[i]] - 1] = A[i];
        C[A[i]]--;
    }
    free(B);
    free(C);
}