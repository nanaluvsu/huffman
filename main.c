#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dissect.c"
#include "frequencia.h"
#include "frequencia.c"


void countChars() { //mainly for testing purposes
    char input[256];
    int chars[256] = {0};

    printf("Oi coloca uma frase pra testar :3 : ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input.\n");
    }

    for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
        chars[(U8)input[i]]++;
        Elemento el;
        el.byte = (U8)input[i];
        printf("%c", el.byte);
    }

    printf("\nFrequencias ordenadas:\n");

    Elemento arrayFreq[256];

    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (chars[i] > 0) {
            arrayFreq[count].byte = i;
            arrayFreq[count].frequencia = chars[i];
            count++;
            
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arrayFreq[i].frequencia < arrayFreq[j].frequencia) {
                Elemento temp = arrayFreq[i];
                arrayFreq[i] = arrayFreq[j];
                arrayFreq[j] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        printf("'%c': %u\n", arrayFreq[i].byte, arrayFreq[i].frequencia);
    }

    int sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += chars[i];
    }

    printf("Total: %d\n", sum);
}

void compact(FILE* file, char* toCompress) {
    int freq[256] = {0};
    char c;
    while ((c = fgetc(file)) != EOF) {
        freq[(U8)c]++;
    }
    
    
    char output[256];
    strcpy(output, toCompress);
    strcat(output, "_out.txt");
    
    FILE* outFile = fopen(output, "w");

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            fprintf(outFile, "%c %d\n", (char)i, freq[i]);
        }
    }
    fclose(outFile);

    freqTable list;
    init_list(&list);

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            insert(&list, novo_nodo((char)i, freq[i]));
        }
    }

    Node_arv* root = arvoreDeHuffman(&list);

    Codif *arr;
    U8 size;
    silly(root, &arr, &size);

    fseek(file, 0, SEEK_SET /* SEEK_SET significa 'começo do arquivo'*/);//volta pro começo do arquivo

    char arq[101];
    strcpy(arq, toCompress);
    strcat(arq, "_compressed");

    compress(file, arq, arr, size);

    for (int i = 0; i < size; i++) {
        free(arr[i].codigo);
    }
    free(arr);
}


int main() {
    //countChars();
    printf("Huffman gaming\n");
    printf("Compactando: horuma.mp4\n");
    FILE *file = fopen("horuma.mp4", "rb");

    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    } else {
        compact(file, "horuma.mp4");
        fclose(file);
    }

    return 0;
}