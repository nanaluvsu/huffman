#include <stdio.h>
#include "tipos.h"
#include "frequencia.h"

void init_list(freqTable* list) {
    list->qtd_preenchida = 0; //na teoria isso é uma fila mas imagino que dê pra usar tabela
    //pior dos casos eu mudo dps
}

void insert(freqTable* list, Node_arv* node) {
    U8 i = list->qtd_preenchida - 1;
    while (i >= 0 && list->arr[i]->info.frequencia > node->info.frequencia) {
        list->arr[i + 1] = list->arr[i];
        i--;
    }
    list->arr[i + 1] = node;
    list->qtd_preenchida++;
}

Node_arv* remove(freqTable* list) {
    if (list->qtd_preenchida != 0)  { //qtd_preenchida = 10 -> return list->arr[9]
        return list->arr[--list->qtd_preenchida];
    } else {
        return NULL; 
    }
}