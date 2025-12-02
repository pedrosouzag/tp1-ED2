#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "acessoSequencial.h"

//funcao auxiliar para criar o indice de paginas
int criarIndicePaginas(const char *nomeArquivo, TipoIndice tabela[],  int numRegistros, long *transferencias, double *tempoCriacao) {
    double inicio = now_seconds();
    
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
            (*transferencias)++;              
        }
    }
    
    fclose(arquivo);
    // calculo do tempo
    double fim = now_seconds();
    *tempoCriacao = (fim - inicio);
    
    return numPaginas;
}

//funcao auxiliar para buscar a pagina que pode conter a chave
int buscarPaginaNoIndice(int chave, TipoIndice tabela[], int numPaginas, long *comp) {
    *comp = 0;
    int paginaAlvo = -1;
    
    // busca sequencial no indice
    for (int i = 0; i < numPaginas; i++) {
        (*comp)++;
        
        // Se eh a ultima pagina ou se a chave eh menor que a proxima pagina
        if (i == numPaginas - 1 || chave < tabela[i + 1].chave) {
            if (chave >= tabela[i].chave) {
                paginaAlvo = tabela[i].posicao;
            }
            break;
        }
    }
    
    return paginaAlvo;
}

int buscarPaginaNoIndiceBinario(int chave, TipoIndice tabela[], int numPaginas, long *comp) {
    *comp = 0;

    int inicio = 0;
    int fim = numPaginas - 1;
    int paginaAlvo = -1;

    while (inicio <= fim) {

        int meio = (inicio + fim) / 2;
        (*comp)++;

        // verifica se a chave pertence ao intervalo desta pagina
        if ((meio == numPaginas - 1 || chave < tabela[meio + 1].chave) && chave >= tabela[meio].chave) {
            return tabela[meio].posicao; 
        }

        // decide para qual lado ir
        if (chave < tabela[meio].chave) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }

    return paginaAlvo;  // retorna -1 se não encontrou
}


// carregar pagina de um arquivo
int carregarPagina(const char *nomeArquivo, int numPagina, PaginaAS *paginaAlvo, int numRegistros, long *transferencias) {
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
// funcao para buscar dentro da uma pahina
int buscarNaPagina(int chave, PaginaAS *pag, long *comp, TipoItem *resultado) {
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
void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave, long *transferencias, long *comp, double *tempo, TipoItem *resultado, int *encontrado, TipoIndice *tabelaIndice, int numPaginas) {
    double inicio = now_seconds();
    
    *transferencias = 0;
    *comp = 0;
    *encontrado = 0;
    
    TipoIndice *indiceLocal = tabelaIndice;
    int paginasLocal = numPaginas;
    int criadoAgora = 0;
    
    // cria o vetor em casa de testo manual 
    if (indiceLocal == NULL) {
        paginasLocal = (quantidade + ITENSPAGINA - 1) / ITENSPAGINA; 
        indiceLocal = (TipoIndice *)malloc(paginasLocal * sizeof(TipoIndice));
        if (!indiceLocal) return;
        
        long transfIndice = 0;
        double tempoCriacaoIndice = 0;
        paginasLocal = criarIndicePaginas(nomeArquivo, indiceLocal, quantidade, &transfIndice, &tempoCriacaoIndice);
        if (paginasLocal == 0) {
            free(indiceLocal);
            return;
        }
        *transferencias += transfIndice;
        printf("Tempo para criacao do indice: %.3lf segundos\n", tempoCriacaoIndice);
        criadoAgora = 1;
    }
    
    //busca no indice
    long compIndice = 0;
    int numPaginaAlvo = buscarPaginaNoIndice(chave, indiceLocal, paginasLocal, &compIndice);
    *comp += compIndice;
    
    //carrega a pagina e busca
    if (numPaginaAlvo >= 0) {
        PaginaAS paginaAlvo;
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
    
    // libera apenas se criou agora
    if (criadoAgora) {
        free(indiceLocal);
    }
    
    double fim = now_seconds();
    *tempo = ((double)(fim - inicio));
}

//funcao para teste com 20 chaves aleatorias
void pesquisar20AleatoriasSI(const char *nomeArquivo, int quantidade) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        printf("erro ao abrir arquivo\n");
        return;
    }

    // pega todas as chaves do arquivo
    int *chaves = (int *)malloc(quantidade * sizeof(int));
    TipoItem item;
    for (int i = 0; i < quantidade; i++) {
        fread(&item, sizeof(TipoItem), 1, arquivo);
        chaves[i] = item.chave;
    }
    fclose(arquivo);

    // embaralha as chaves
    srand(time(NULL));
    for (int i = quantidade - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = chaves[i];
        chaves[i] = chaves[j];
        chaves[j] = temp;
    }

    // cria indice uma vez e marca o tempo
    int numPaginas = (quantidade + ITENSPAGINA - 1) / ITENSPAGINA;
    TipoIndice *tabelaIndice = (TipoIndice *)malloc(numPaginas * sizeof(TipoIndice));
    if (!tabelaIndice) {
        free(chaves);
        return;
    }
    
    long transfIndice = 0;
    double tempoCriacaoIndice = 0;
    numPaginas = criarIndicePaginas(nomeArquivo, tabelaIndice, quantidade, &transfIndice, &tempoCriacaoIndice);
    
    if (numPaginas == 0) {
        free(chaves);
        free(tabelaIndice);
        return;
    }
    
    printf("\nindice criado: %d paginas | tempo criacao indice: %.3f s | transf: %ld\n", numPaginas, tempoCriacaoIndice, transfIndice);

    long compTotal = 0;
    long transfTotal = 0;
    double tempoPesquisaTotal = 0.0;

    printf("\niniciando pesquisa de 20 chaves aleatorias\n");

    for (int i = 0; i < 20; i++) {
        int chaveAtual = chaves[i];
        long transf = 0;
        long comp = 0;
        double tempo = 0;
        TipoItem resultado;
        int encontrado = 0;

        // executa a busca passando o indice
        lerArquivoSequencial(nomeArquivo, quantidade, chaveAtual, &transf, &comp, &tempo, &resultado, &encontrado, tabelaIndice, numPaginas);

        // mostra o resultado da busca
        if (encontrado) {
            printf("chave buscada: %d | encontrada | transferencias: %ld | comp: %ld | tempo: %.3f s\n", chaveAtual, transf, comp, tempo);
        } else {
            printf("chave buscada: %d | nao encontrada | transferencias: %ld | comp: %ld | tempo: %.3f s\n", chaveAtual, transf, comp, tempo);
        }

        compTotal += comp;
        transfTotal += transf;
        tempoPesquisaTotal += tempo;
    }

    printf("\n=== RESULTADOS ===\n");
    printf("tempo criacao indice: %.3f s | transf indice: %ld\n", tempoCriacaoIndice, transfIndice);
    printf("pesquisas: 20 | comp totais: %ld | transf totais: %ld | tempo pesquisa: %.3f s\n", compTotal, transfTotal, tempoPesquisaTotal);
    printf("tempo TOTAL: %.3f s\n", tempoCriacaoIndice + tempoPesquisaTotal);

    free(chaves);
    free(tabelaIndice);
}


void executarSequencial(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {

    // modo teste (-T): faz 20 buscas aleatorias
    if (modoTeste) {
        pesquisar20AleatoriasSI(nomeArquivo, quantidade);
        return;
    }

    // print opcional das chaves
    if (imprimirChaves) {
        printf("\nchaves do arquivo:\n");
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

    // inicio da busca normal
    printf("\npesquisando chave %d...\n", chave);

    long transferencias = 0;
    long comp = 0;
    double tempo = 0;
    TipoItem resultado;
    int encontrado = 0;

    // processamento principal - passa NULL para criar indice internamente
    lerArquivoSequencial(nomeArquivo, quantidade, chave, &transferencias, &comp, &tempo, &resultado, &encontrado, NULL, 0);

    //  prints 
    if (encontrado) {
        printf("chave encontrada\n");
        printf("chave: %d | dado1: %ld | dado2: %.50s\n", resultado.chave, resultado.dado1, resultado.dado2);
    } else {
        printf("chave nao encontrada\n");
    }

    // conclusao 
    printf("transferencias: %ld | comparacoes: %ld | tempo: %.3f s\n", transferencias, comp, tempo);

}