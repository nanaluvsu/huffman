#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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



char *codigoHuffman[256]; // Guarda os códigos de Huffman


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

void heapify(freqTable *heap, int i) {
    int menor = i, esq = 2 * i + 1, dir = 2 * i + 2;

    if (esq < heap->qtd_preenchida && heap->arr[esq]->info.frequencia < heap->arr[menor]->info.frequencia)
        menor = esq;
    if (dir < heap->qtd_preenchida && heap->arr[dir]->info.frequencia < heap->arr[menor]->info.frequencia)
        menor = dir;

    if (menor != i) {
        trocar(&heap->arr[i], &heap->arr[menor]);
        heapify(heap, menor);
    }
}

ptr_node extrairMin(freqTable *heap) {
    ptr_node min = heap->arr[0];
    heap->arr[0] = heap->arr[--heap->qtd_preenchida];
    heapify(heap, 0);
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

// =========================
// Codificação de Huffman
// =========================

ptr_node construirArvoreHuffman(U64 freq[]) {
    freqTable heap = {0};
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            inserirHeap(&heap, novoNo(i, freq[i]));

    while (heap.qtd_preenchida > 1) {
        ptr_node esq = extrairMin(&heap);
        ptr_node dir = extrairMin(&heap);
        ptr_node novo = novoNo(0, esq->info.frequencia + dir->info.frequencia);
        novo->esquerda = esq;
        novo->direita = dir;
        inserirHeap(&heap, novo);
    }
    return extrairMin(&heap);
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

void compactarArquivo(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    char* extension = strrchr(saida, '.');
    if (!extension || strcmp(extension, ".huf") != 0) {
        strcat(saida, ".huf");
    }
    FILE *arqSaida = fopen(saida, "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    U64 freq[256] = {0};
    int tamanhoOriginal = 0;
    int c;

    while ((c = fgetc(arqEntrada)) != EOF) {
        freq[c]++;
        tamanhoOriginal++;
    }
    rewind(arqEntrada);

    ptr_node raiz = construirArvoreHuffman(freq);
    char caminho[256];
    gerarCodigos(raiz, caminho, 0);

    salvarArv(raiz, arqSaida);
    fwrite(&tamanhoOriginal, sizeof(U32), 1, arqSaida);

    unsigned char buffer = 0;
    int pos = 0;
    while ((c = fgetc(arqEntrada)) != EOF) {
        char *codigo = codigoHuffman[c];
        for (int i = 0; codigo[i]; i++) {
            buffer <<= 1;
            if (codigo[i] == '1') buffer |= 1;
            pos++;
            if (pos == 8) {
                fputc(buffer, arqSaida);
                buffer = 0;
                pos = 0;
            }
        }
    }

    if (pos > 0) {
        buffer <<= (8 - pos);
        fputc(buffer, arqSaida);
    }

    fclose(arqEntrada);
    fclose(arqSaida);
    printf("Arquivo compactado com sucesso!\n");
}


Node_arv* desserializarArvore(FILE *arquivo) {
    int bit = fgetc(arquivo);
    if (bit == EOF) return NULL;

    if (bit == '1') {
        unsigned char c = fgetc(arquivo);
        return novoNo(c, 0);
    } else {
        Node_arv *l = desserializarArvore(arquivo);
        Node_arv *r = desserializarArvore(arquivo);
        Node_arv *pai = novoNo(0, 0);
        pai->esquerda = l;
        pai->direita = r;
        return pai;
    }
}

void descompactarArquivo(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    FILE *arqSaida = fopen(saida, "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    Node_arv *root = desserializarArvore(arqEntrada);
    unsigned int tamanhoOriginal = 0;
    fread(&tamanhoOriginal, sizeof(unsigned int), 1, arqEntrada);

    Node_arv *cur = root;
    int lidos = 0, byte;
    while (lidos < tamanhoOriginal && (byte = fgetc(arqEntrada)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1;
            cur = (bit == 0) ? cur->esquerda : cur->direita; //condicao ternaria para decidir o caminho
            if (!cur->esquerda && !cur->direita) {
                fputc(cur->info.byte, arqSaida);
                cur = root;
                lidos++;
                if (lidos == tamanhoOriginal) break;
            }
        }
    }

    fclose(arqEntrada);
    fclose(arqSaida);
    printf("Arquivo descompactado com sucesso!\n");
}


int main() {
    int opcao;
    char entrada[100], saida[100];

    do {
        printf("\nOmg huffman gaming hiiiii\n");
        printf("1 - Compactar\n");
        printf("2 - Descompactar\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 0:
                printf("Encerrando...\n");
                break;

            case 1:
                printf("Arquivo de entrada: ");
                scanf("%s", entrada);
                printf("Arquivo de saída (compactado): ");
                scanf("%s", saida);
                compactarArquivo(entrada, saida);
                break;

            case 2:
                printf("Arquivo compactado: ");
                scanf("%s", entrada);
                printf("Arquivo de saída (descompactado): ");
                scanf("%s", saida);
                descompactarArquivo(entrada, saida);
                break;

            default:
                printf("Opção inválida.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}
