
#ifndef ARVOREBESTRELA_H
#define ARVOREBESTRELA_H

#include "registro.h"
#include "tempo.h"

#define VERDADEIRO 1
#define FALSO 0

#define ORDEM_ESTRELA 50 
#define MM_ESTRELA (2 * ORDEM_ESTRELA)

// tipo que define se a pagina e interna (so indices) ou externa (dados reais)
typedef enum { Interna, Externa } TipoPaginaEstrela;

// estrutura principal da pagina da arvore b*
typedef struct PaginaEstrela {
    TipoPaginaEstrela tipo; // indica se e pagina interna ou externa
    union {
        // pagina interna: guarda chaves e ponteiros para outras paginas
        struct {
            int numChaves; // quantas chaves tem nesta pagina
            int chaves[MM_ESTRELA]; 
            struct PaginaEstrela *filhos[MM_ESTRELA + 1]; 
        } interna;
        
        // pagina externa: guarda os registros completos 
        struct {
            int numRegistros; 
            Registro registros[MM_ESTRELA]; 
        } externa;
    } conteudo;
} PaginaEstrela;

// funcao que busca uma chave na arvore
Registro* pesquisaBEstrela(PaginaEstrela *pagina, int chave, long *comparacoes);

// funcao que insere um registro na arvore
void insereBEstrela(Registro reg, PaginaEstrela **raiz, long *comparacoes);

// funcao que le registros de arquivo e constroi a arvore
void lerArquivoArvoreBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela **raiz, long *transferencias, long *comparacoes, double *tempo);

// funcao que testa a arvore pesquisando 20 chaves aleatorias
void pesquisar20AleatoriasBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela *raiz);

// funcao principal que executa toda a operacao da arvore
void executarArvoreBEstrela(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif