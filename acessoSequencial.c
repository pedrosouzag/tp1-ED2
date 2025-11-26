#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "acessoSequencial.h"

//funcao auxiliar para criar o indice de paginas
int criarIndicePaginas(const char *nomeArquivo, TipoIndice tabela[],  int numRegistros, long *transferencias) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        printf ("Erro ao abrir arquivo");
        return 0;
    }
    
    *transferencias = 0;
    int numPaginas = (numRegistros + ITENSPAGINA - 1) / ITENSPAGINA;
    TipoItem reg;
    
    //le o primeiro registro e cria uma entrada no indice para cada pagina
    for (int i = 0; i < numPaginas; i++) {
        long posArquivo = i * ITENSPAGINA * sizeof(TipoItem);
        fseek(arquivo, posArquivo, SEEK_SET);
        
        if (fread(&reg, sizeof(TipoItem), 1, arquivo) == 1) {
            tabela[i].posicao = i;           //numero da pagina
            tabela[i].chave = reg.chave;     //primeira chave da pagina
            (*transferencias)++;              //conta leitura do primeiro registro
        }
    }
    
    fclose(arquivo);
    return numPaginas;
}

//funcao auxiliar para buscar a pagina que pode conter a chave
int buscarPaginaNoIndice(int chave, TipoIndice tabela[], int numPaginas, long *comp) {
    *comp = 0;
    int paginaAlvo = -1;
    
    // Busca sequencial no indice
    for (int i = 0; i < numPaginas; i++) {
        (*comp)++;
        
        // Se e a ultima pagina ou se a chave e menor que a proxima pagina
        if (i == numPaginas - 1 || chave < tabela[i + 1].chave) {
            if (chave >= tabela[i].chave) {
                paginaAlvo = tabela[i].posicao;
            }
            break;
        }
    }
    
    return paginaAlvo;
}

// Funcao auxiliar para carregar uma pagina do arquivo
int carregarPagina(const char *nomeArquivo, int numPagina, Pagina *paginaAlvo, int numRegistros, long *transferencias) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        return 0;
    }
    
    long posArquivo = numPagina * ITENSPAGINA * sizeof(TipoItem);
    fseek(arquivo, posArquivo, SEEK_SET);
    
    paginaAlvo->numItens = 0;
    TipoItem item;
    
    //le ate ITENSPAGINA registros ou ate acabar o arquivo
    for (int i = 0; i < ITENSPAGINA; i++) {
        int posRegistro = numPagina * ITENSPAGINA + i;
        if (posRegistro >= numRegistros) break;
        
        if (fread(&item, sizeof(TipoItem), 1, arquivo) == 1) {
            paginaAlvo->itens[paginaAlvo->numItens++] = item;
            (*transferencias)++;  // conta cada registro transferido
        }
    }
    
    fclose(arquivo);
    return paginaAlvo->numItens > 0;
}

// Funcao auxiliar para buscar dentro de uma pagina
int buscarNaPagina(int chave, Pagina *pag, long *comp, TipoItem *resultado) {
    for (int i = 0; i < pag->numItens; i++) {
        (*comp)++;
        if (pag->itens[i].chave == chave) {
            *resultado = pag->itens[i];
            return 1;
        }
    }
    printf("Chave nao encontrada!\n");
    return 0;
}


//funcao principal
void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave, long *transferencias, long *comp, double *tempo, TipoItem *resultado, int *encontrado) {
    clock_t inicio = clock();
    
    *transferencias = 0;
    *comp = 0;
    *encontrado = 0;
    
    int numPaginas = (quantidade + ITENSPAGINA - 1) / ITENSPAGINA;
    TipoIndice *tabelaIndice = (TipoIndice *)malloc(numPaginas * sizeof(TipoIndice));
    if (!tabelaIndice) {
        return;
    }
    
    //cria o indice de paginas
    long transfIndice = 0;
    criarIndicePaginas(nomeArquivo, tabelaIndice, quantidade, &transfIndice);
    *transferencias += transfIndice;
    
    //busca no indice
    long compIndice = 0;
    int numPaginaAlvo = buscarPaginaNoIndice(chave, tabelaIndice, numPaginas, &compIndice);
    *comp += compIndice;
    
    //carrega a pagina e busca
    if (numPaginaAlvo >= 0) {
        Pagina paginaAlvo;
        long transfPagina = 0;
        
        if (carregarPagina(nomeArquivo, numPaginaAlvo, &paginaAlvo, quantidade, &transfPagina)) {
            *transferencias += transfPagina;
            
            long compPagina = 0;
            *encontrado = buscarNaPagina(chave, &paginaAlvo, &compPagina, resultado);
            *comp += compPagina;
        }
    } else {
        printf ("Pagina alvo nao encontrada");
    }
    
    free(tabelaIndice);
    
    clock_t fim = clock();
    *tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    //printa a conclusao
    printf("Concluido! Arquivo: %s | Registros: %d | Transferencias: %ld | comp: %ld | Tempo: %.6f s\n", nomeArquivo, quantidade, *transferencias, *comp, *tempo);
}

//funcao para teste com 20 chaves aleatorias
void pesquisar20AleatoriasSI(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo\n");
        return;
    }
    
    //passa  todas as chaves do arquivo para esse vetor
    int *chaves = (int *)malloc(quantidade * sizeof(int));
    TipoItem item;
    for (int i = 0; i < quantidade; i++) {
        fread(&item, sizeof(TipoItem), 1, arquivo);
        chaves[i] = item.chave;
    }
    fclose(arquivo);
    
    // Embaralha
    srand(time(NULL));
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = chaves[i];
        chaves[i] = chaves[j];
        chaves[j] = temp;
    }
    
    long compTotal = 0;
    clock_t inicio = clock();
    
    for (int i = 0; i < 20; i++) {
        int chaveAtual = chaves[i];
        long transf, comp;
        double tempo;
        TipoItem resultado;
        int encontrado;
        
        lerArquivoSequencial(nomeArquivo, quantidade, chaveAtual, &transf, &comp, &tempo, &resultado, &encontrado);
        
        compTotal += comp;
    }
    
    clock_t fim = clock();
    double tempoTotal = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    printf("Pesquisas: 20 | comp totais: %ld | Tempo: %.6f s\n", compTotal, tempoTotal);
    
    free(chaves);
}