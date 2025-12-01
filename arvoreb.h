#ifndef ARVOREB_H
#define ARVOREB_H
#include "registro.h"
#include "tempo.h"

#define TRUE 1
#define FALSE 0

#define ORDEM 50 // fica a criterio para trocar
#define MM 2 * ORDEM 

typedef struct pagina {
    int n;
    Registro registro [MM];
    struct pagina *filhos [MM+ 1];
}Pagina;

Registro* pesquisa(Pagina *pagina, int chave, long *comp);

void InsereNaPagina(Pagina *Ap, Registro Reg, Pagina *ApDir);
void Ins (Registro reg, Pagina *ap, short *cresceu, Registro *regRetorno, Pagina **apRetorno, long *comp);
void Insere(Registro reg, Pagina **ap, long *comp);
void lerArquivoArvoreB(const char *nomeArquivo, int numRegistros, Pagina **raiz, long *transferencias, long *comp, double *tempo);
void pesquisar20Aleatorias(const char *nomeArquivo, int numRegistros, Pagina *raiz);
void executarArvoreB(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);


#endif