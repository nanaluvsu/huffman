#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "tipos.h"
#include "frequencia.h"
#include "metodos.c"



ptr_node constroiArv(U64 freq[]) {
    freqTable heap = {0}; // Inicializa a tabela de frequências
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            inserirHeap(&heap, novoNo(i, freq[i])); // Insere os nós na heap

    while (heap.qtd_preenchida > 1) {
        ptr_node esq = extrairMin(&heap);
        ptr_node dir = extrairMin(&heap);
        ptr_node novo = novoNo(0, esq->info.frequencia + dir->info.frequencia);
        novo->esquerda = esq;
        novo->direita = dir;
        inserirHeap(&heap, novo); // Insere o novo nó na heap
    }
    return extrairMin(&heap);
}


void compactarArquivo(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    char* extension = strrchr(saida, '.'); 
    if (!extension || strcmp(extension, ".huf") != 0) {
        strcat(saida, ".huf"); //Determina a extensão do arquivo de saída como .huf
    }
    FILE *arqSaida = fopen(saida, "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos.\n");
        return;
    }

    U64 freq[256] = {0};
    int tamanhoOriginal = 0;
    int c;

    while ((c = fgetc(arqEntrada)) != EOF) { // Lê o arquivo de entrada byte a byte
        freq[c]++; // Atualiza a frequência do byte lido
        tamanhoOriginal++; // Incrementa o tamanho original do arquivo
    }
    fseek(arqEntrada, 0, SEEK_SET); //Retorna o ponteiro ao início do arquivo

    ptr_node root = constroiArv(freq);
    char caminho[256];
    gerarCodigos(root, caminho, 0); // Gera os códigos de Huffman para cada byte

    salvarArv(root, arqSaida); // Salva a árvore no arquivo de saída
    fwrite(&tamanhoOriginal, sizeof(U32), 1, arqSaida); // Salva o tamanho original do arquivo

    unsigned char buffer = 0;
    int pos = 0;
    while ((c = fgetc(arqEntrada)) != EOF) { // Lê o arquivo de entrada novamente
        char *codigo = codigoHuffman[c];
        for (int i = 0; codigo[i]; i++) {
            buffer <<= 1; // Desloca o buffer para a esquerda
            if (codigo[i] == '1') buffer |= 1; // Adiciona o bit ao buffer
            pos++;
            if (pos == 8) {
                fputc(buffer, arqSaida); // Escreve o byte completo no arquivo de saída
                buffer = 0;
                pos = 0;
            }
        }
    }

    if (pos > 0) {
        buffer <<= (8 - pos); // Preenche os bits restantes com zeros
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
    while (lidos < tamanhoOriginal && (byte = fgetc(arqEntrada)) != EOF) { // Enquanto não leu o tamanho original e não chegou ao fim do arquivo
        for (int i = 7; i >= 0; i--) {
            int bit = (byte >> i) & 1;
            cur = (bit == 0) ? cur->esquerda : cur->direita; //condicao ternaria para decidir o caminho
            if (!cur->esquerda && !cur->direita) {
                fputc(cur->info.byte, arqSaida); // Se for uma folha, escreve o byte no arquivo de saída
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
