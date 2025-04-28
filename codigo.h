#pragma once
#include "tipos.h"

typedef struct
{
    U8* byte;
    U8 capacidade;
    U8 tamanho;
} Codigo;

bool novo_codigo (Codigo* c);
void free_codigo (Codigo* c);

bool add_bit (Codigo* c, U8 valor);
bool disc_bit (Codigo* c );

bool clone (Codigo original, Codigo* cpy);
