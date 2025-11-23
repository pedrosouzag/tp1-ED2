#include <stdio.h>
#include <stdlib.h>

#define ITENSSPAGINA 4
#define MAXTABELA 100

//definição de uma entrada da tabela de índice das páginas
typedef struct{
    int posicao;
    int chave;
} Tipoindice;

//definição de um item do arquivo de dados
typedef struct{
    char titulo[31];
    int chave;
    float preco;
} Tipoitem;

//rettorna 1 se encontrar 0 se não encontrar -1 se der erro
int pesquisa (Tipoindice tab[], int tam, Tipoitem* item, FILE *arq){
    Tipoitem pagina[ITENSSPAGINA];
    int i, quantitens;
    long desloc;

    if (arq == NULL || item == NULL || tab == NULL){
        return -1;
    }

    //procura a pagina onde pode ter o item
    i = 0;
    while(i < tam && tab[i].chave <= item->chave) i++;

    //caso a chave desejada seja menor que a primeira chave, o item não existe no arquivo

    if(i == 0){
        return 0;
    }

    //verifica se a ultima pagina esta completa
    if(i < tam){
        quantitens = ITENSSPAGINA;
    }else{
        fseek(arq, 0, SEEK_END);
        long tamanho = ftell(arq);
        quantitens = (tamanho/sizeof(tipoitem)) % ITENSSPAGINA;
        if(quantidade == 0) quantitens = ITENSSPAGINA; //verifica se a ultima pagina ta completa
    }

        //le a pagina desejada do arquivo
        desloc = (tab[i - 1].posicao-1) * ITENSSPAGINA * sizeof(tipoitem);
        fseek(arq, desloc, SEEK_SET);

        size_t lidos = fread(&pagina, sizeof(tipoitem), quantitens, arq);
        if(lidos != quantitens){
            fprintf(stderr, "Erro na leitura da pagina\n");
            return -1;
        }

        //pesquisa sequencial na página lida
        for(i = 0; i < quantitens; i++){
            if(pagina[i].chave == item->chave){
                *item = pagina[i];
                return 1;
            }
        return 0;
    }
} 

//função pra construir indice, sei se precisa disso não
int construirIndice(FILE *arq, TipoIndice tabela[], int maxTabela) {
    TipoItem x;
    int pos = 0;
    int cont = 0;

    if (arq == NULL || tabela == NULL) {
        return -1;
    }

    fseek(arq, 0, SEEK_SET);
    
    while (fread(&x, sizeof(TipoItem), 1, arq) == 1) {
        cont++;
        // Primeira chave de cada página
        if (cont % ITENSSPAGINA == 1) {
            if (pos >= maxTabela) {
                fprintf(stderr, "Tabela de índice cheia\n");
                return -1;
            }
            tabela[pos].chave = x.chave;
            tabela[pos].posicao = pos + 1;
            pos++;
        }
    }

    return pos;
}

//tem que tirar essa main
int main(){
    Tipoindice tabela[MAXTABELA];
    FILE *arq;
    Tipoitem x;
    int pos, cont;

    //abre o arquivo de dados
    if((Arq = fopen("livro.bin", "rb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return 0;
    }

    //gera a tabela de indice das paginas
    cont = 0;
    pos = 0;
    while(fread(&x, sizeof(x), 1, arq) == 1){
        cont++;
        if(cont%ITENSSPAGINA == 1){
            tabela[pos].chave = x.chave;
            tabela[pos].posicao = pos+1;
            pos++;
        }
    }

    fflush(stdout);
    printf("Codigo do livre desejado: ");
    scanf("%d", x.chave);

    //ativa a função de pesquisa
    if(pesquisa(tabela, pos, &x, arq))
        printf("Livro %s (codigo %d) foi localizado", x.livro, x.chave);
    else
        printf("Livro de codigo %d nao foi encontrado", x.chave);
    
    fclose(arq);
    return 0;
}
