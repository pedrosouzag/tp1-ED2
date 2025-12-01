#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvorebinaria.h"
#include "registro.h"

void criarArvoreBinaria(const char *nomeArquivoArvore){
    // cria o arquivo onde a arvore sera armazenada
    FILE *arquivo = fopen(nomeArquivoArvore, "wb");
    if (arquivo == NULL){
        printf("erro ao criar arquivo da arvore\n");
    }

    // cria um no raiz vazio
    NoArquivo raizVazia;
    raizVazia.registro.chave = -1;
    raizVazia.esquerda = -1;
    raizVazia.direita = -1;

    // grava a raiz no arquivo
    fwrite(&raizVazia, sizeof(NoArquivo), 1, arquivo);
    fclose(arquivo);
}

NoArquivo lerNo(FILE *arquivo, long posicao){
    // posiciona o ponteiro e le o no
    NoArquivo no;
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET);
    fread(&no, sizeof(NoArquivo), 1, arquivo);
    return no;
}

void escreveNo(FILE *arquivo, long posicao, NoArquivo no){
    // posiciona o ponteiro e escreve o no atualizado
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET);
    fwrite(&no, sizeof(NoArquivo), 1, arquivo);
}

long inserirRecursivo(FILE * arquivo, long posicao, Registro reg, long *comp){
    // cria um novo no quando a posicao e -1
    if(posicao == -1){
        NoArquivo novoNo;
        novoNo.registro = reg;
        novoNo.esquerda = -1;
        novoNo.direita = -1;

        // escreve no final do arquivo
        fseek(arquivo, 0, SEEK_END);
        long novaPosicao = ftell(arquivo) / sizeof(NoArquivo);

        fwrite(&novoNo, sizeof(NoArquivo), 1, arquivo);
        return novaPosicao;
    }

    // le o no atual da arvore
    NoArquivo noAtual = lerNo(arquivo, posicao);

    (*comp)++;

    // compara e decide se insere a direita
    if(reg.chave > noAtual.registro.chave){
        noAtual.direita = inserirRecursivo(arquivo, noAtual.direita, reg, comp);
        escreveNo(arquivo, posicao, noAtual);
    }
    // compara e decide se insere a esquerda
    else if(reg.chave < noAtual.registro.chave){
        noAtual.esquerda = inserirRecursivo(arquivo, noAtual.esquerda, reg, comp);
        escreveNo(arquivo, posicao, noAtual);
    }
    // se for igual nao insere para evitar duplicata

    return posicao;
}

void inserirEmArquivo(const char *nomeArquivoArvore, Registro reg, long *comp){
    // abre o arquivo da arvore para escrita
    FILE *arquivo = fopen(nomeArquivoArvore, "r+b");
    if(arquivo == NULL){
        printf("erro ao abrir arquivo\n");
        return;
    }

    // le a raiz da arvore
    NoArquivo raiz = lerNo(arquivo, 0);

    // insere na raiz se estiver vazia
    if(raiz.registro.chave == -1){
        NoArquivo novoNo;
        novoNo.registro = reg;
        novoNo.esquerda = -1;
        novoNo.direita = -1;

        escreveNo(arquivo, 0, novoNo);
    }
    else{
        // chama a recursao para inserir em outra posicao
        inserirRecursivo(arquivo, 0, reg, comp);
    }

    fclose(arquivo);
}

Registro* buscarEmArquivo(const char *nomeArquivoArvore, int chave, long *comp, long *transferencias){
    // abre o arquivo da arvore para leitura
    FILE *arq = fopen(nomeArquivoArvore, "rb");
    if (!arq){
        printf("erro ao abrir o arquivo da arvore binaria\n");
        return NULL;
    }

    NoArquivo no;
    long posicao = 0;

    *comp = 0;
    *transferencias = 0;

    // percorre a arvore ate achar o final ou encontrar a chave
    while (posicao != -1){
        fseek(arq, posicao * sizeof(NoArquivo), SEEK_SET);

        // le o no da posicao
        if (fread(&no, sizeof(NoArquivo), 1, arq) != 1){
            fclose(arq);
            return NULL;
        }

        (*transferencias)++;
        (*comp)++;

        // verifica se a chave procurada esta aqui
        if (chave == no.registro.chave){
            Registro *regRetorno = malloc(sizeof(Registro));
            *regRetorno = no.registro;
            fclose(arq);
            return regRetorno;
        }

        (*comp)++;

        // decide o lado para continuar a busca
        if (chave < no.registro.chave){
            posicao = no.esquerda;
        } 
        else {
            posicao = no.direita;
        }
    }

    fclose(arq);
    return NULL;                   
}

void lerArquivoBinario(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros, long *transferencias, long *comp, double *tempo){
    // abre arquivo com registros
    FILE *arquivo = fopen(nomeArquivoDados, "rb");
    if(arquivo == NULL){
        printf("erro ao abrir arquivo de dados\n");
        return;
    }

    Registro reg;
    int registrosLidos = 0;
    double inicio, fim;

    // cria a arvore vazia
    criarArvoreBinaria(nomeArquivoArvore);

    *transferencias = 0;
    *comp = 0;

    printf("lendo %d registros de %s construindo arvore em %s\n",
           numRegistros, nomeArquivoDados, nomeArquivoArvore);

    inicio = now_seconds();

    // le os registros e insere na arvore
    while(registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1){
        (*transferencias)++;

        long compInsercao = 0;
        inserirEmArquivo(nomeArquivoArvore, reg, &compInsercao);

        *comp += compInsercao;
        registrosLidos++;
    }

    fim = now_seconds();
    *tempo = ((double)(fim - inicio));

    fclose(arquivo);
}

void pesquisar20AleatoriasAB(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros){
    // pesquisa 20 registros aleatorios na arvore
    FILE *arquivo;
    Registro reg;

    long transTotal = 0;
    long compTotal = 0;

    double inicio, fim;

    printf("pesquisando 20 chaves aleatorias\n");

    srand(time(NULL));
    inicio = now_seconds();

    for(int i = 0; i < 20; i++){
        // escolhe uma posicao aleatoria do arquivo de dados
        int posicao = rand() % numRegistros;

        // abre o arquivo de dados e le um registro aleatorio
        arquivo = fopen(nomeArquivoDados, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        fclose(arquivo);

        long comp = 0;
        long transf = 0;

        // busca a chave na arvore
        Registro *resultado = buscarEmArquivo(nomeArquivoArvore, reg.chave, &comp, &transf);

        compTotal += comp;
        transTotal += transf;

        // libera memoria se achou
        if(resultado != NULL){
            printf("chave %d (pos %d) encontrada | transf: %ld | comp: %ld\n", reg.chave, posicao, transf, comp);
            free(resultado);
        }
        else{
            printf("chave %d (pos %d) nao encontrada | transf: %ld | comp: %ld\n", reg.chave, posicao, transf, comp);
        }
    }


    fim = now_seconds();
    double tempo = ((double)(fim - inicio));

    printf("pesquisas: 20 | comparacoes totais: %ld | transferencias: %ld | tempo de pesquisa: %.3f s\n", compTotal, transTotal, tempo);
}

void executarArvoreBinaria(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {

    const char *arquivoArvore = "arvore_binaria.bin";

    long transferencias = 0;
    long comp = 0;
    double tempoCriacao = 0, tempoPesquisa = 0;

    // modo teste -T : constroi arvore e faz 20 buscas aleatorias
    if (modoTeste) {
        lerArquivoBinario(nomeArquivo, arquivoArvore, quantidade, &transferencias, &comp, &tempoCriacao);
        printf ("Tempo para construir arvore : %.3lf\n" , tempoCriacao);
        pesquisar20AleatoriasAB(nomeArquivo, arquivoArvore, quantidade);
        return;
    }

    // imprimir chaves se -P for usado
    if (imprimirChaves) {
        printf("\nchaves do arquivo:\n");
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

    // construindo a arvore
    lerArquivoBinario(nomeArquivo, arquivoArvore, quantidade, &transferencias, &comp, &tempoCriacao);

    // busca normal
    printf("\npesquisando chave %d...\n", chave);

    // pasando variveis de comp, transferencias e marcando o tempo
    long compBusca = 0;
    long transfBusca = 0;
    double incio = now_seconds();
    Registro *resultado = buscarEmArquivo(arquivoArvore, chave, &compBusca, &transfBusca);
    double fim = now_seconds();

    tempoPesquisa = incio - fim;

    // somar os valores da busca aos totais
    transferencias += transfBusca;
    comp += compBusca;

    if (resultado != NULL) {
        printf("chave encontrada\n");
        printf("chave: %d | dado1: %ld | dado2: %.50s\n", resultado->chave, resultado->dado1, resultado->dado2);
        free(resultado);
    } else {
        printf("chave nao encontrada\n");
    }

    // estatisticas 
    printf("transferencias: %ld | comparacoes: %ld | tempo de criacao: %.3f s | tempo de pesquisa : %.3f s \n", transferencias, comp, tempoCriacao, tempoPesquisa);
}


