#pragma once

#include "tipos.h"

typedef struct {
    U32 byte; 
    U64 frequencia;
} Elemento;


typedef struct Node_arv {
    struct Node_arv* esquerda;
    Elemento info; 
    struct Node_arv* direita; 
} Node_arv;

typedef Node_arv* ptr_node;

typedef struct  {
    ptr_node arr[256];
    U16 qtd_preenchida;
} freqTable;

void nova_tabela(freqTable* tbl);

bool addByte(U8 byte, freqTable* tbl);

void joinOnStart(freqTable* tbl);

