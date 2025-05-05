#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_ALFABETO 256

// =========================
// Estruturas de Dados
// =========================

typedef struct HuffmanNode {
    unsigned char caractere;
    int frequencia;
    struct HuffmanNode *esquerda, *direita;
} HuffmanNode;

typedef struct MinHeap {
    int tamanho;
    HuffmanNode* dados[TAM_ALFABETO];
} MinHeap;

char *codigoHuffman[TAM_ALFABETO]; // Guarda os códigos de Huffman

// =========================
// Utilitários de Heap
// =========================

HuffmanNode* novoNo(unsigned char c, int freq) {
    HuffmanNode *no = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    no->caractere = c;
    no->frequencia = freq;
    no->esquerda = no->direita = NULL;
    return no;
}

void trocar(HuffmanNode **a, HuffmanNode **b) {
    HuffmanNode *temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(MinHeap *heap, int i) {
    int menor = i, esq = 2*i + 1, dir = 2*i + 2;

    if (esq < heap->tamanho && heap->dados[esq]->frequencia < heap->dados[menor]->frequencia)
        menor = esq;
    if (dir < heap->tamanho && heap->dados[dir]->frequencia < heap->dados[menor]->frequencia)
        menor = dir;

    if (menor != i) {
        trocar(&heap->dados[i], &heap->dados[menor]);
        heapify(heap, menor);
    }
}

HuffmanNode* extrairMin(MinHeap *heap) {
    HuffmanNode *min = heap->dados[0];
    heap->dados[0] = heap->dados[--heap->tamanho];
    heapify(heap, 0);
    return min;
}

void inserirHeap(MinHeap *heap, HuffmanNode *no) {
    int i = heap->tamanho++;
    heap->dados[i] = no;
    while (i && heap->dados[i]->frequencia < heap->dados[(i-1)/2]->frequencia) {
        trocar(&heap->dados[i], &heap->dados[(i-1)/2]);
        i = (i-1)/2;
    }
}

// =========================
// Codificação de Huffman
// =========================

HuffmanNode* construirArvoreHuffman(int freq[]) {
    MinHeap heap = {0};
    for (int i = 0; i < TAM_ALFABETO; i++)
        if (freq[i] > 0)
            inserirHeap(&heap, novoNo(i, freq[i]));

    while (heap.tamanho > 1) {
        HuffmanNode *esq = extrairMin(&heap);
        HuffmanNode *dir = extrairMin(&heap);
        HuffmanNode *novo = novoNo(0, esq->frequencia + dir->frequencia);
        novo->esquerda = esq;
        novo->direita = dir;
        inserirHeap(&heap, novo);
    }
    return extrairMin(&heap);
}

void gerarCodigos(HuffmanNode *raiz, char *caminho, int profundidade) {
    if (!raiz) return;

    if (!raiz->esquerda && !raiz->direita) {
        caminho[profundidade] = '\0';
        codigoHuffman[raiz->caractere] = strdup(caminho);
        return;
    }

    caminho[profundidade] = '0';
    gerarCodigos(raiz->esquerda, caminho, profundidade + 1);
    caminho[profundidade] = '1';
    gerarCodigos(raiz->direita, caminho, profundidade + 1);
}

void serializarArvore(HuffmanNode *raiz, FILE *arquivo) {
    if (!raiz) return;

    if (!raiz->esquerda && !raiz->direita) {
        fputc('1', arquivo);
        fputc(raiz->caractere, arquivo);
    } else {
        fputc('0', arquivo);
        serializarArvore(raiz->esquerda, arquivo);
        serializarArvore(raiz->direita, arquivo);
    }
}

// =========================
// Compactação
// =========================

void compactarArquivo(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    FILE *arqSaida = fopen(saida, "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    int freq[TAM_ALFABETO] = {0}, tamanhoOriginal = 0, c;

    while ((c = fgetc(arqEntrada)) != EOF) {
        freq[c]++;
        tamanhoOriginal++;
    }
    rewind(arqEntrada);

    HuffmanNode *raiz = construirArvoreHuffman(freq);
    char caminho[256];
    gerarCodigos(raiz, caminho, 0);

    serializarArvore(raiz, arqSaida);
    fwrite(&tamanhoOriginal, sizeof(unsigned int), 1, arqSaida);

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

// =========================
// Descompactação
// =========================

HuffmanNode* desserializarArvore(FILE *arquivo) {
    int bit = fgetc(arquivo);
    if (bit == EOF) return NULL;

    if (bit == '1') {
        unsigned char c = fgetc(arquivo);
        return novoNo(c, 0);
    } else {
        HuffmanNode *esq = desserializarArvore(arquivo);
        HuffmanNode *dir = desserializarArvore(arquivo);
        HuffmanNode *pai = novoNo(0, 0);
        pai->esquerda = esq;
        pai->direita = dir;
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

    HuffmanNode *raiz = desserializarArvore(arqEntrada);
    unsigned int tamanhoOriginal = 0;
    fread(&tamanhoOriginal, sizeof(unsigned int), 1, arqEntrada);

    HuffmanNode *atual = raiz;
    int lidos = 0, byte;
    while (lidos < tamanhoOriginal && (byte = fgetc(arqEntrada)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1;
            atual = (bit == 0) ? atual->esquerda : atual->direita;
            if (!atual->esquerda && !atual->direita) {
                fputc(atual->caractere, arqSaida);
                atual = raiz;
                lidos++;
                if (lidos == tamanhoOriginal) break;
            }
        }
    }

    fclose(arqEntrada);
    fclose(arqSaida);
    printf("Arquivo descompactado com sucesso!\n");
}

// =========================
// Menu Principal
// =========================

int main() {
    int opcao;
    char entrada[100], saida[100];

    do {
        printf("\n===== COMPACTADOR HUFFMAN =====\n");
        printf("1 - Compactar arquivo\n");
        printf("2 - Descompactar arquivo\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
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

            case 0:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    return 0;
}
