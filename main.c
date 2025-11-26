
#include "arvoreb.h"
#include "acessoSequencial.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// base main
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: %s <metodo> <quantidade> <situacao> <chave> [-P]\n", argv[0]);
        printf("  ou: %s <metodo> <quantidade> <situacao> -T (teste 20 chaves)\n", argv[0]);
        return 1;
    }
    
    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    
    // Verifica se e modo teste (-T)
    int modoTeste = (argc == 5 && strcmp(argv[4], "-T") == 0);
    
    int chave = 0;
    int imprimirChaves = 0;
    
    if (!modoTeste) {
        if (argc < 5) {
            printf("Faltando parametro <chave>\n");
            return 1;
        }
        chave = atoi(argv[4]);
        imprimirChaves = (argc == 6 && strcmp(argv[5], "-P") == 0);
    }
    
    char nomeArquivo[50];
    if (situacao == 1) strcpy(nomeArquivo, "registros_asc.bin");
    else if (situacao == 2) strcpy(nomeArquivo, "registros_desc.bin");
    else if (situacao == 3) strcpy(nomeArquivo, "registros_mix.bin");
    else {
        printf("Situacao invalida\n");
        return 1;
    }
    
    if (metodo == 1) {
        if (modoTeste) {
            // Modo teste: pesquisa 20 chaves aleatorias
            pesquisar20AleatoriasSI(nomeArquivo, quantidade);
        } else {
            // Modo normal: pesquisa uma chave especifica
            if (imprimirChaves) {
                printf("\nChaves do arquivo:\n");
                FILE *arquivo = fopen(nomeArquivo, "rb");
                TipoItem item;
                int count = 0;
                while (count < quantidade && fread(&item, sizeof(TipoItem), 1, arquivo) == 1) {
                    printf("%d ", item.chave);
                    if ((count + 1) % 10 == 0) printf("\n");
                    count++;
                }
                printf("\n");
                fclose(arquivo);
            }
            
            printf("\nPesquisando chave %d...\n", chave);
            long transferencias, comp;
            double tempo;
            TipoItem resultado;
            int encontrado;
            
            lerArquivoSequencial(nomeArquivo, quantidade, chave, 
                                &transferencias, &comp, &tempo, 
                                &resultado, &encontrado);
            
            if (encontrado) {
                printf("CHAVE ENCONTRADA\n");
                printf("comp: %ld | Tempo: %.6f s\n", comp, tempo);
                if (imprimirChaves) {
                    printf("Chave: %d | Dado1: %ld | Dado2: %.50s...\n", 
                           resultado.chave, resultado.dado1, resultado.dado2);
                }
            } else {
                printf("CHAVE NAO ENCONTRADA\n");
                printf("comp: %ld | Tempo: %.6f s\n", comp, tempo);
            }
        }
        
    } else if (metodo == 2) {
        printf("Metodo 2: Arvore Binaria - NAO IMPLEMENTADO\n");
        
    } else if (metodo == 3) {
        Pagina *raiz;
        long transferencias, comp;
        double tempo;
        
        lerArquivoArvoreB(nomeArquivo, quantidade, &raiz, &transferencias, &comp, &tempo);
        
        if (modoTeste) {
            // Modo teste: pesquisa 20 chaves aleatorias
            pesquisar20Aleatorias(nomeArquivo, quantidade, raiz);
        } else {
            // Modo normal: pesquisa uma chave especifica
            if (imprimirChaves) {
                printf("\nChaves do arquivo:\n");
                FILE *arquivo = fopen(nomeArquivo, "rb");
                Registro reg;
                int count = 0;
                while (count < quantidade && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
                    printf("%d ", reg.chave);
                    if ((count + 1) % 10 == 0) printf("\n");
                    count++;
                }
                printf("\n");
                fclose(arquivo);
            }
            
            printf("\nPesquisando chave %d...\n", chave);
            long comp = 0;
            clock_t inicio = clock();
            Registro *resultado = pesquisa(raiz, chave, &comp);
            clock_t fim = clock();
            double tempoPesquisa = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
            
            if (resultado != NULL) {
                printf("CHAVE ENCONTRADA\n");
                printf("comp: %ld | Tempo: %.6f s\n", comp, tempoPesquisa);
                if (imprimirChaves) {
                    printf("Chave: %d | Dado1: %ld | Dado2: %.50s...\n", 
                           resultado->chave, resultado->dado1, resultado->dado2);
                }
            } else {
                printf("CHAVE NAO ENCONTRADA\n");
                printf("comp: %ld | Tempo: %.6f s\n", comp, tempoPesquisa);
            }
        }
        
    } else if (metodo == 4) {
        printf("Metodo 4: Arvore B* - NAO IMPLEMENTADO\n");
        
    } else {
        printf("Metodo invalido\n");
        return 1;
    }
    
    return 0;
}