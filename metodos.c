#include <stdio.h>
#include "tipos.h"
#include "frequencia.h"

void init_list(freqTable* list) {
    list->qtd_preenchida = 0; //na teoria isso é uma fila mas imagino que dê pra usar tabela
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

Node_arv* removeList(freqTable* list) {
    if (list->qtd_preenchida != 0)  { //qtd_preenchida = 10 -> return list->arr[9]
        return list->arr[--list->qtd_preenchida];
    } else {
        return NULL; 
    }
}

char *codigoHuffman[256]; 


ptr_node novoNo(U32 byte, U64 freq) {
    ptr_node no = (ptr_node)malloc(sizeof(Node_arv));
    no->info.byte = byte;
    no->info.frequencia = freq;
    no->esquerda = no->direita = NULL;
    return no;
    }

void trocar(ptr_node *a, ptr_node *b) {
    ptr_node temp = *a;
    *a = *b;
    *b = temp;
}
    

void transHeap(freqTable *heap, int i) {
    int menor = i, esq = 2 * i + 1, dir = 2 * i + 2;

    if (esq < heap->qtd_preenchida && heap->arr[esq]->info.frequencia < heap->arr[menor]->info.frequencia)
        menor = esq;
    if (dir < heap->qtd_preenchida && heap->arr[dir]->info.frequencia < heap->arr[menor]->info.frequencia)
        menor = dir;

    if (menor != i) {
        trocar(&heap->arr[i], &heap->arr[menor]);
        transHeap(heap, menor);
    }
}

ptr_node extrairMin(freqTable *heap) {
    ptr_node min = heap->arr[0];
    heap->arr[0] = heap->arr[--heap->qtd_preenchida];
    transHeap(heap, 0);
    return min;
}

void inserirHeap(freqTable *heap, ptr_node no) {
    int i = heap->qtd_preenchida++;
    heap->arr[i] = no;
    while (i && heap->arr[i]->info.frequencia < heap->arr[(i - 1) / 2]->info.frequencia) {
        trocar(&heap->arr[i], &heap->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void gerarCodigos(ptr_node raiz, char *caminho, int level) {
    if (!raiz) return;

    if (!raiz->esquerda && !raiz->direita) {
        caminho[level] = '\0';
        codigoHuffman[raiz->info.byte] = strdup(caminho);
        return;
    }

    caminho[level] = '0';
    gerarCodigos(raiz->esquerda, caminho, level + 1);
    caminho[level] = '1';
    gerarCodigos(raiz->direita, caminho, level + 1);
}

void salvarArv(ptr_node raiz, FILE *arquivo) {
    if (!raiz) return;

    if (!raiz->esquerda && !raiz->direita) {
        fputc('1', arquivo);
        fputc(raiz->info.byte, arquivo);
    } else {
        fputc('0', arquivo);
        salvarArv(raiz->esquerda, arquivo);
        salvarArv(raiz->direita, arquivo);
    }
}


Node_arv* reconstruct(FILE *arquivo) { //reconstrua a arvore a partir do arquivo
    //reconstruct WHAT? There's nothing left
    int bit = fgetc(arquivo);
    if (bit == EOF) return NULL;

    if (bit == '1') {
        unsigned char c = fgetc(arquivo); //caso o bit seja 1, lemos o byte
        return novoNo(c, 0); //cria um novo no com o byte lido
    } else {
        Node_arv *l = reconstruct(arquivo); //recursao para a esquerda
        Node_arv *r = reconstruct(arquivo); //recursao para a direita
        Node_arv *pai = novoNo(0, 0);
        pai->esquerda = l;
        pai->direita = r;
        return pai;
    }
}

