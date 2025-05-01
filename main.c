#include <stdio.h>
#include <stdlib.h>

//Main function for Huffman coding
#include "codigo.h"
#include "codigo.c"
#include "frequencia.h"
#include "frequencia.c"


void countChars() {
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
        printf("'%c': %d\n", arrayFreq[i].byte, arrayFreq[i].frequencia);
    }

    int sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += chars[i];
    }

    printf("Total: %d\n", sum);
}

int main() {
    countChars();
    printf("Huffman gaming\n");
    printf("Compactando: horuma.mp4\n");
    FILE *file = fopen("horuma.mp4", "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    while(fgetc(file) != EOF) {
        
    }
    return 0;
}