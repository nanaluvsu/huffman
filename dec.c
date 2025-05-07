#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "tipos.h"
#include "frequencia.h"
#include "metodos.c"



ptr_node constroiArv(U64 freq[]) {
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

    ptr_node raiz = constroiArv(freq);
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

void descompactarArquivo(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    FILE *arqSaida = fopen(saida, "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    Node_arv *root = reconstruct(arqEntrada);
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
    setlocale(LC_ALL, "Portuguese");
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
