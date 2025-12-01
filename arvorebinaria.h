#ifndef ARVOREBINARIA_H
#define ARVOREBINARIA_H
#include "registro.h"
#include "tempo.h"

/*typedef struct {
    int chave;
    long int dado1;
    char dado2[5001];
} Registro;*/

typedef struct {
    Registro registro;
    long esquerda;  //-1 se nao existe
    long direita;   //-1 se nao existe
} NoArquivo;

void criarArvoreBinaria(const char *nomeArquivoArvore);

void inserirEmArquivo(const char *nomeArquivoArvore, Registro reg, long *comp);

Registro* buscarEmArquivo(const char *nomeArquivoArvore, int chave, long *comp, long *transferencias);

void lerArquivoArvoreBinaria(const char *nomeArquivoDados, const char *nomeArquivoArvore,
                             int numRegistros, long *transferencias, long *comp, double *tempo);

void pesquisar20AleatoriasAB(const char *nomeArquivoDados, const char *nomeArquivoArvore, int numRegistros);
void executarArvoreBinaria(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif