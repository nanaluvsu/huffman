//this file is supposed to contain the functions that will do the whole huffman coding
#include <stdio.h>
#include "tipos.h"
#include "codigo.h"
#include "frequencia.h"
#include "lista.c"

Node_arv* novo_nodo(char c, U8 freq) {
    Node_arv* node = (Node_arv*)malloc(sizeof(Node_arv));
    Elemento el;
    el.byte = c;
    el.frequencia = freq;
    node->info = el;
    node->esquerda = NULL;
    node->direita = NULL;
    return node;
}

Node_arv* arvoreDeHuffman(freqTable* list) {
    while (list->qtd_preenchida > 1) {
        Node_arv* l = removerMenor(list);
        Node_arv* r = removerMenor(list);

        I32 somaFrequencia = l->info.frequencia + r->info.frequencia;
        Node_arv* node = novo_nodo('c', somaFrequencia);

        node->esquerda = l;
        node->direita = r;

        insert(list, node);
    }
    return remove(list);
}

void buscaChar(Node_arv *raiz, char *path, U8 nivel, Codif **arr, U8 *size) {
     if (raiz == NULL) return;
 
     if (raiz->esquerda == NULL && raiz->direita == NULL) {
         path[nivel] = '\0';
 
         Codif c;
         c.info = raiz->info.byte;
         c.codigo = strdup(path);
 
         *arr = realloc(*arr, (*size + 1) * sizeof(Codif));
         if (*arr == NULL) {
             printf("Erro ao realocar memória!\n");
             return;
         }
     }
 
     if (raiz->esquerda != NULL) {
         path[nivel] = '0';
         buscaChar(raiz->esquerda, path, nivel + 1, arr, size);
     }
 
     if (raiz->direita != NULL) {
         path[nivel] = '1';
         buscaChar(raiz->direita, path, nivel + 1, arr, size);
     }
 }

bool compress(FILE *input, char *out, Codif* arr, U8 size) {
   FILE* output = fopen(out, "wb");

   U8 buf = 0;
   I32 bits = 0;


   char c = '\0';
   while ((c = fgetc(input)) != EOF) {
         for (U8 i = 0; i < size; i++) {
              if (arr[i].info == c) {
                for (U8 j = 0; arr[i].codigo[j] != '\0'; j++) {
                     buf <<= 1;
                     if (arr[i].codigo[j] == '1') {
                          buf |= 1;
                     }
                     bits++;
                     if (bits == 8) {
                          fwrite(&buf, sizeof(U8), 1, output);
                          buf = 0;
                          bits = 0;
                     }
                }
                break;
              }
         }
   }
   if (bits > 0) {
         buf <<= (8 - bits);
         fwrite(&buf, sizeof(U8), 1, output);
   }
}

void CodGenerator(Node_arv *raiz, Codif **vetor, U8 *tamanho) {
    char caminho[101];
    *tamanho = 0;
    *vetor = NULL;
    SearchCharacters(raiz, caminho, 0, vetor, tamanho);
}
