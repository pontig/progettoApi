#include <stdio.h>
#include <stdlib.h>

int k;      // lenght of the words
int e = 0;  // Number of eligibles words

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

//ignore the entire line in stdin, using getline and freeing the memory
void ignoreLine() {
    char *line = NULL;
    int len = 10;
    getline(&line, &len, stdin);
    free(line);
}

int main(int argc, char const *argv[]) {
    char **eligibiles = (char **)malloc(sizeof(char *));  // Dinamic array of elegibles words
    scanf("%d\n", &k);

    fillEligibles(eligibiles);

    char flag = getchar();
    while (flag != EOF) {
        char command = getchar();
        
        ignoreLine();

        switch (command) {
            case 'n': // nuova_partita
 
                 break;
            case 's': // stampa_filtrate
                
                break;
            case 'i': // inserisci_inizio
                fillEligibles(eligibiles);
                ignoreLine();
                break;
            default:
                break;
        }
    }

    return 0;
}