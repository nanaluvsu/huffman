#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "frequencia.h"


void nova_tabela(freqTable* tbl) {
    for (U16 i =0; i < 256; i++) {
        tbl->arr[i] = NULL;

        
    }
    tbl->qtd_preenchida = 0;
    //obg augusto
}

static bool novo_no(ptr_node* novo, ptr_node l, Elemento inf, ptr_node r) {
    *novo = (ptr_node)malloc(sizeof(Node_arv));

    if(*novo == NULL) return false;
 
    (*novo)->esquerda = l;
    (*novo)->info = inf;
    (*novo)->direita = r;
    return true;

}

bool addByte(U8 byte, freqTable* tbl) {
    if (tbl->arr[byte]==NULL) {
        tbl->arr[byte]->info.frequencia++;
        return true;
    }

    Elemento el;
    el.byte = byte;
    el.frequencia = 1;
    
    bool confirm = novo_no(&(tbl->arr[byte]), NULL, el, NULL);

    if (!confirm) {
        return false;
    }

    tbl->qtd_preenchida++;
    return true;
}

void joinOnStart(freqTable* tbl)
{
    U16 fst_NULL, fst_non_NULL;

    for(;;)
    {
        fst_NULL=0;
        while (fst_NULL<256 && tbl->arr[fst_NULL]!=NULL)
            fst_NULL++;
        if (fst_NULL==256) break;

        fst_non_NULL=fst_NULL+1;
        while (fst_non_NULL<256 && tbl->arr[fst_non_NULL]==NULL)
            fst_non_NULL++;
        if (fst_non_NULL==256) break;

        tbl->arr[fst_NULL]=tbl->arr[fst_non_NULL];
        tbl->arr[fst_non_NULL]=NULL;
    }
}

