#ifndef ACESSOSEQUENCIAL_H
#define ACESSOSEQUENCIAL_H

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

// Funcao principal que carrega o indice e faz a busca
void lerArquivoSequencial(const char *nomeArquivo, int quantidade, int chave,
                          long *transferencias, long *comp, 
                          double *tempo, TipoItem *resultado, int *encontrado);

// Funcao para teste com 20 chaves aleatorias
void pesquisar20AleatoriasSI(const char *nomeArquivo, int quantidade);

void executarSequencial(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves);

#endif