#include <stdlib.h>
#include <stdio.h>
#include "frequencia.h"


void nova_tabela(freqTable* tbl) {
    for (U16 i =0; i < 256; i++) {
        tbl->arr[i] = NULL;

        tbl->qtd_preenchida = 0;
    }
}

static bool novo_no(ptr_node* novo, ptr_node l, Elemento inf, ptr_node r) {
    *novo = (ptr_node)malloc(sizeof(Node_arv));

    if(*novo == NULL) return false;
 
    (*novo)->esquerda = l;
    (*novo)->info = inf;
    (*novo)->direita = r;
    println("Nó criado.");
    return true;
    //vou chorar eu n sei usar variavel no commit tchau
}