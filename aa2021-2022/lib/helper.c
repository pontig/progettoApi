#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swapc(char **a, int pos1, int pos2) {
    char *aux = a[pos1];
    a[pos1] = a[pos2];
    a[pos2] = aux;
}
int partition(char **a, int p, int r) {
    char *x = a[r];
    int i = p - 1;
    int j = p;
    while (j < r) {
        if (strcmp(a[j], x) <= 0) {
            i++;
            swapc(a, i, j);
        }
        j++;
    }
    swapc(a, i + 1, r);
    return i + 1;
}
void quick_sort(char **a, int p, int r) {
    if (p < r) {
        int q = partition(a, p, r);
        quick_sort(a, p, q - 1);
        quick_sort(a, q + 1, r);
    }
}

int main(int argc, char const *argv[]) {
    FILE *correct = fopen("debug/coorectExtract.txt", "r");
    FILE *wrong = fopen("debug/wrongExtract.txt", "r");
    char *wordsW[9368];
    char *wordsC[9311];
    for (int i = 0; i < 9368; i++) {
        wordsW[i] = (char *)malloc(sizeof(char) * 7);
        fgets(wordsW[i], 7, wrong);
        wordsW[i][5] = '\0';
    }
    for (int i = 0; i < 9311; i++) {
        wordsC[i] = (char *)malloc(sizeof(char) * 7);
        fgets(wordsC[i], 7, correct);
        wordsC[i][5] = '\0';
    }
    
    // Print the elements not in common between the two arrays
    for (int i = 0; i < 9368; i++) {
        int found = 0;
        for (int j = 0; j < 9311; j++) {
            if (strcmp(wordsW[i], wordsC[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("%s in W and not in C\n", wordsW[i]);
        }
    }
    // Same thing, inverting the arrays
    for (int i = 0; i < 9311; i++) {
        int found = 0;
        for (int j = 0; j < 9368; j++) {
            if (strcmp(wordsC[i], wordsW[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("%s in C and not in W\n", wordsC[i]);
        }
    }
    


    return 0;
}
