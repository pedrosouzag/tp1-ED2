#ifndef ACESSOSEQUENCIAL_H
#define ACESSOSEQUENCIAL_H
#include "tempo.h"

#define ITENSPAGINA 4
#define MAXTABELA 100

// Definicao de uma entrada da tabela de indice das paginas
typedef struct {
    int posicao;      // posicao da pagina no arquivo (numero da pagina)
    int chave;        // menor chave da pagina (chave do primeiro registro)
} TipoIndice;

// Definicao de um item do arquivo de dados
typedef struct {
    int chave;
    long int dado1;
    char dado2[5000];
} TipoItem;

// Definicao de uma pagina (bloco de registros)
typedef struct {
    TipoItem itens[ITENSPAGINA];
    int numItens;  // quantos itens estao realmente na pagina
} PaginaAS;

// Funções auxiliares
int criarIndicePaginas(const char *nomeArquivo, TipoIndice tabela[], int numRegistros, long *transferencias, double *tempoCriacao);
int buscarPaginaNoIndice(int chave, TipoIndice tabela[], int numPaginas, long *comp);
int buscarPaginaNoIndiceBinario(int chave, TipoIndice tabela[], int numPaginas, long *comp);
int carregarPagina(const char *nomeArquivo, int numPagina, PaginaAS *paginaAlvo, int numRegistros, long *transferencias);
int buscarNaPagina(int chave, PaginaAS *pag, long *comp, TipoItem *resultado);

// Função principal de busca sequencial indexada
void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave, long *transferencias, long *comp, double *tempo, TipoItem *resultado, int *encontrado, TipoIndice *tabelaIndice, int numPaginas);

// Funções para testes
void pesquisar20AleatoriasSI(const char *nomeArquivo, int quantidade);
void executarSequencial(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif