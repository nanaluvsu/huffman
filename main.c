#include <stdio.h>
#include <stdlib.h>

//Main function for Huffman coding
#include "codigo.h"
#include "codigo.c"


int main() {
    Codigo c;
    if (!novo_codigo(&c)) {
        printf("meudeus explodiu oq vc fez\n");
        return 1;
    } else {
        printf("Deu certo, criou :D\n");
    }

    if (!add_bit(&c, 1)) {
        printf("Deu erro nao adicionou:c\n");
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