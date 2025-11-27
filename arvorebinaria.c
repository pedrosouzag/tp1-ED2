#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvorebinaria.h"
#include "registro.h"

void criarArvoreBinaria(const char *nomeArquivoArvore){
    FILE *arquivo = fopen(nomeArquivoArvore, "wb");
    if (arquivo == NULL){
        printf("Erro ao criar arquivo da arvore\n");
    }

    NoArquivo raizVazia;
    raizVazia.registro.chave = -1;
    raizVazia.esquerda = -1;
    raizVazia.direita = -1;

    fwrite(&raizVazia, sizeof(NoArquivo), 1, arquivo);
    fclose(arquivo);
}

NoArquivo lerNo(FILE *arquivo, long posicao){
    NoArquivo no;
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET);
    fread(&no, sizeof(NoArquivo), 1, arquivo);

    return no;
}

void escreveNo(FILE *arquivo, long posicao, NoArquivo no){
    fseek(arquivo, posicao * sizeof(NoArquivo), SEEK_SET);
    fwrite(&no, sizeof(NoArquivo), 1, arquivo);
}

//função auxiliar para a função inserir
long inserirRecursivo(FILE * arquivo, long posicao, Registro reg, long *comp){
    //se for -1 cria um novo no no final do arquivo
    if(posicao == -1){
        NoArquivo novoNo;
        novoNo.registro = reg;
        novoNo.esquerda = -1;
        novoNo.direita = -1;

        //vai pro final do arquivo
        fseek(arquivo, 0, SEEK_END);
        long novaPosicao = ftell(arquivo) / sizeof(NoArquivo);

        fwrite(&novoNo, sizeof(NoArquivo), 1, arquivo);

        return novaPosicao;
    }

    //le o no atual
    NoArquivo noAtual = lerNo(arquivo, posicao);
    (*comp)++;
    if(reg.chave > noAtual.registro.chave){
        //insere a direita
        noAtual.direita = inserirRecursivo(arquivo, noAtual.direita, reg, comp);
        escreverNo(arquivo, posicao, noAtual);
    }else if(reg.chave < noAtual.registro.chave){
        //insere a esquerda
        noAtual.esquerda = inserirRecursivo(arquivo, noAtual.esquerda, reg, comp);
        escreveNo(arquivo, posicao, noAtual);
    }
    //se for igual noa insere pra evitar duplicata

    return posicao;
}

void inserirEmArquivo(const char *nomeaArquivoArvore, Registro reg, long *comp){
    FILE *arquivo= fopen(nomeaArquivoArvore, "r+b");
    if(arquivo == NULL){
        printf("Erro ao abrir arquivo\n");
        return;
    }

    //le a raiz
    NoArquivo raiz = lerNo(arquivo, 0);

    //se tiver vazia faz a primeira inserção
    if(raiz.registro.chave == -1){
        raiz.registro = reg;
        raiz.esquerda = -1;
        raiz.direita = -1;
        escreverNo(arquivo, 0, reg, comp);
    }else{
        inserirRecursivo(arquivo, 0, reg, comp);
    }

    fclose(arquivo);
}

Registro* buscarEmArquivo(const char *nomeArquivoArvore, int chave, long *comp, long *transferencias){
    FILE *arq = fopen (nomeArquivoArvore, "rb");
    if (!arq){
        printf("Erro ao abrir o arquivo da arvore binaria");
        return NULL;
    }
    NoArquivo no;
    long posicao = 0;

    *comp = 0;
    *transferencias = 0;

    while (posicao != -1){
        // move ate o no desejaod na arvore 
        fseek(arq, posicao * sizeof (NoArquivo), SEEK_SET);
        
        // leitura do no na posicao movida
        if ( fread(&no, sizeof(NoArquivo), 1, arq) != 1){
            fclose(arq);
            return NULL;
        }
        // contagem de transferencias em arquivo, pois lemos
        (*transferencias)++;

        (*comp)++;

        if (chave = no.registro.chave){
            // se encontrar a chave, retorna a copia do registro
            Registro *regRetono = malloc (sizeof(Registro));
            *regRetono = no.registro;
            fclose(arq);
            return regRetono;
        }

        (*comp)++;
        if (chave < no.registro.chave){
            posicao = no.esquerda;
        } else {
            posicao = no.direita;
        }
            
    }
    // fecha e retorna NULL em caso de erro
    fclose (arq);
    return NULL;                   
     
}
    


void lerArquivoBinario(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros, long *transferencias, long *comp, double *tempo){
    FILE *arquivo;
    Registro reg;
    int registrosLidos = 0;
    clock_t inicio, fim;

    //cria o arquivo da arvore vazia
    criarArvoreBinaria(nomeArquivoArvore);

    arquivo = fopen(nomeArquivoArvore, "rb");
    if(arquivo == NULL){
        printf("Erro ao abrir arquivo");
        return;
    }

    *transferencias = 0;
    *comp = 0;
    printf("Lendo %d registro de %s construido arvore em %s \n", numRegistros, nomeArquivoDados, nomeArquivoArvore);

    inicio = clock();

    while(registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1){
        (*transferencias)++;
        long compInsercao = 0;
        inserirEmArquivo(nomeArquivoArvore, reg, &compInsercao);
        *comp += compInsercao;
        registrosLidos++;
    }

    fim = clock();
    *tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC; // converte os ticks de clock para segundo

    fclose(arquivo);
    
    printf("Arquivo: %s | Registro: %d | Transferencias: %ld | Comparacoes: %ld | tempo: %.6f s\n", *transferencias, *comp, *tempo);
    
}

void pesquisar20AleatoriasAB(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros){
    FILE *arquivo;
    Registro reg;
    long transTotal = 0;
    long compTotal = 0;
    clock_t inicio, fim;
    
    printf("Pesquisando 20 chaves aleatorias\n");

    srand(time(NULL));
    inicio = clock();

    for(int i = 0; i < 20; i++){
        int posicao = rand() % numRegistros;

        //abre a chave e pega na posição aleatoria
        arquivo = fopen(nomeArquivoArvore, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        fclose(arquivo);

        //pesquisa a chave
        long comp = 0;
        long transf = 0;
        Registro *resultado = buscarEmArquivo(nomeArquivoArvore, reg.chave, &comp, &transf);
        compTotal += comp;
        transTotal += transf;

        if(resultado == NULL){
            printf("Chave %d (pos %d) nao encontrada\n", reg.chave, posicao);
        }else{
            free(resultado);
        }
    }

    fim = clock();
    double tempo = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Pesquisas: 20 | comparacoes totais: %ld | media: %.2f | transferencias %ld | tempo: %.6f s\n", compTotal, compTotal/20.0, transTotal, tempo);
}