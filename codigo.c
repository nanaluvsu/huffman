#include "codigo.h"

bool novo_codigo (Codigo* c)
{
    c->byte=(U8*)malloc(1*sizeof(U8));
    if (c->byte== NULL) {
        return false;
    } 

    c->capacidade=8;
    c->tamanho=0;
    return true;
}

void free_codigo (Codigo* c)
{
    if (c->byte!=NULL) free(c->byte);
}

bool add_bit (Codigo* c, U8 valor)
{
    if (c->tamanho==c->capacidade)
    {
        U8* novo = (U8*)malloc((c->capacidade / 8 + 1 )* sizeof(U8));
        if (novo == NULL) return false;

        for(int i = 0; i < c->tamanho / 8 ; i++)
            novo[i] = c->byte[i];

        free(c->byte);
        c->byte = novo;

        c->capacidade+= 8;
    }

    c->byte[c->capacidade/8-1]<<=1;

    if (valor==1)
        c->byte[c->capacidade/8-1]|=1;

    c->tamanho++;
    return true;
}


bool disc_bit (Codigo* c)
{
    if (c->tamanho==0) {
        return false;
    } 

    c->byte[c->capacidade/8-1]>>=1;
    c->tamanho--;

    if (c->capacidade>8 && c->capacidade-c->tamanho==8)
    {
        U8* novo=(U8*)malloc((c->capacidade/8-1)*sizeof(U8));
        if ( novo==NULL ) {
            return false;
        }

        for(int i=0;i<c->tamanho/8;i++)
            novo[i]=c->byte[i];

        free(c->byte);
        c->byte=novo;

        c->capacidade-=8;
    }
}

bool clone (Codigo original, Codigo* cpy)
{
    cpy->byte=(U8*)malloc(original.capacidade*sizeof(U8));
    if (cpy->byte == NULL) return false;


    for(int i = 0;i < cpy->capacidade/8; i++)
        cpy->byte[i]=original.byte[i];

    cpy->capacidade=original.capacidade;
    cpy->tamanho=original.tamanho;
    return true;
}
