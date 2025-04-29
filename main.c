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
    Codigo c;
    if (!novo_codigo(&c)) {
        printf("meudeus explodiu oq vc fez\n");
        return 1;
    } else {
        printf("Deu certo, criou :D\n");
    }

    if (!add_bit(&c, 1)) {
        printf("Deu erro nao adicionou :c\n");
        free_codigo(&c);
        return 1;
    } else {
        printf("Deu certo, adicionou :D\n");
    }

    if (!disc_bit(&c)) {
        printf("Deu erro nao jogou fora :c\n");
        free_codigo(&c);
        return 1;
    } else {
        printf("Deu certo, jogou fora :D\n");
    }

    free_codigo(&c);
    
    return 0;
}