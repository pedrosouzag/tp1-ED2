#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "acessoSequencial.h"
#include "arvorebinaria.h"
#include "arvoreb.h"
#include "registro.h"

int main(int argc, char *argv[]) {

    // verifica se os argumentos minimos foram informados
    if (argc < 4) {
        printf("uso: %s <metodo> <quantidade> <situacao> <chave> [-p]\n", argv[0]);
        printf("  ou: %s <metodo> <quantidade> <situacao> -t\n", argv[0]);
        return 1;
    }

    // metodo de busca (1,2,3 ou 4)
    int metodo = atoi(argv[1]);

    // quantidade de registros a serem lidos
    int quantidade = atoi(argv[2]);

    // situacao do arquivo (ordenado, invertido ou misto)
    int situacao = atoi(argv[3]);

    // verifica se esta no modo teste, onde pesquisa 20 chaves
    int modoTeste = (argc == 5 && strcmp(argv[4], "-t") == 0);

    int chave = 0;
    int imprimirChaves = 0; // nao vai imprimir todas as chaves

    // se nao for modo teste, deve existir a chave alvo
    if (!modoTeste) {
        if (argc < 5) {
            printf("faltando parametro <chave>\n");
            return 1;
        }

        // chave a ser pesquisada
        chave = atoi(argv[4]);

        // se houver -p, imprime as chaves antes da pesquisa
        if (argc == 6) {
            if (strcmp(argv[5], "[-p]") == 0) {
                imprimirChaves = 1;
            }
        }
    }


    // define o nome do arquivo baseado na situacao escolhida
    char nomeArquivo[50];

    if (situacao == 1) strcpy(nomeArquivo, "registros_asc.bin");
    else if (situacao == 2) strcpy(nomeArquivo, "registros_desc.bin");
    else if (situacao == 3) strcpy(nomeArquivo, "registros_mix.bin");
    else {
        printf("situacao invalida\n");
        return 1;
    }
    printf("Teste com %d registros\n", quantidade);
    // escolhe o metodo usando switch
    switch (metodo) {

        case 1:
            // metodo sequencial com indice de paginas
            executarSequencial(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;

        case 2:
            // metodo arvore binaria
            executarArvoreBinaria(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;

        case 3:
            // metodo arvore b
            executarArvoreB(nomeArquivo, quantidade, chave, modoTeste, imprimirChaves);
            break;

        case 4:
            // metodo arvore b* (ainda nao implementado)
            printf("metodo 4 (arvore b*) ainda nao implementado\n");
            break;

        default:
            printf("metodo invalido\n");
            return 1;
    }

    return 0;
}
