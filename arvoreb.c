#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "arvoreb.h"
#include "registro.h"

Registro* pesquisa(Pagina *pagina, int chave, long *comp) {
    int i;

    while (pagina != NULL) {
        i = 0;

          // percorre os itens da pagina ate achar posicao
        while (i < pagina->n && chave > pagina->registro[i].chave) {
            i++;
            (*comp)++;
        }

        // se achou, retorno o ponteiro para o registro
        (*comp)++;
        if (i < pagina->n && chave == pagina->registro[i].chave) {
            return &pagina->registro[i];
        }

        pagina = pagina->filhos[i];
    }

    return NULL;
}

// insere o registro e o ponteiro apDir dentro da pagina ap

void InsereNaPagina(Pagina *ap, Registro Reg, Pagina *apDir){
    int k = ap->n;

    while (k > 0) {

        if (Reg.chave >= ap->registro[k-1].chave){
            break;
        }

        // mover a chave e o ponteiro uma posicao para a direita para abrir espaco
        // a posicao k (nova posicao) pega o antigo ultimo traz pra ela

        ap->registro[k] = ap->registro[k - 1];
        ap-> filhos[k + 1] = ap->filhos[k];
        k--;

    }

    // depois do while, insere na posicao correta
    ap->registro[k] = Reg; 
    ap->filhos[k + 1] = apDir;
    ap->n++;  // atualiza o numero de chaves de chaves

}

void Ins (Registro reg, Pagina *ap, short *cresceu, Registro *regRetorno, Pagina **apRetorno, long *comp){
    int i = 0, j;  // COMEÇA EM 0
    Pagina *apTemp;

    // caso base recursao, chegou na posicao de insercao, devolve a chave para cima
    if (ap == NULL){
        *cresceu = TRUE; // marca que cresceu/ vai subir
        *regRetorno = reg; // chave que sobe 
        *apRetorno = NULL; // ponteiro direito da nova chave
        return;
    }

    // procura a posicao onde o registro deve ser inserido dentro da pagina 
    while (i < ap->n && reg.chave > ap->registro[i].chave){
        (*comp)++;
        i++;
    }

    // em caso de chave duplicada, nao insere
    if (i < ap->n && reg.chave == ap->registro[i].chave) {
        (*comp)++;
        printf("Erro: Registro ja esta presente\n");
        *cresceu = FALSE;
        return;
    }

    // chama a recursao
    Ins (reg, ap->filhos[i], cresceu, regRetorno, apRetorno, comp);

    if (!*cresceu) return; // nada subiu nao precisa mexer na pagina

    // se a pagina tem espaco, nao precisa dividir
    if(ap->n < MM){
        InsereNaPagina(ap, *regRetorno, *apRetorno);
        *cresceu = FALSE;
        return;
    }
    // se nao tem espaco, criar nova pagina e dividir
    apTemp = (Pagina *) malloc(sizeof(Pagina));
    apTemp->n = 0;
    apTemp->filhos[0] = NULL;

    // decide se a nova chave sera inserida na pagina original ou na nova
    if ( i < ORDEM + 1){
        // insere o ultimo elemento da pagina original na nova para abrir espaco
        InsereNaPagina (apTemp, ap->registro[MM - 1], ap->filhos[MM]);
        ap->n--;
        // insere a chave que veio da recursao na pagina original
        InsereNaPagina(ap, *regRetorno, *apRetorno);

    }else{
         // insere diretamente na nova pagina
        InsereNaPagina(apTemp, *regRetorno, *apRetorno);
    }

        // move as maiores chaves para a nova pagina
    for (j = ORDEM + 1; j < MM; j++) {  // j = ORDEM+1 ==  a primeira chave maior que o meio
        InsereNaPagina(apTemp, ap->registro[j], ap->filhos[j + 1]); // // insere a chave e o ponteiro filho correspondente na nova pagina
    }

    // ajusta quantidade de chaves na pagina original, ou seja, fica com a metade menor das chaves
    ap->n = ORDEM;  

    // o primeiro filho da nova pagina aponta para a subarvore correspondente
    apTemp->filhos[0] = ap->filhos[ORDEM + 1];  

    // chave do meio  promovida para o nivel superior
    *regRetorno = ap->registro[ORDEM];  // chave que sobe
    *apRetorno  = apTemp;                // ponteiro para a nova pagina a direita
}

void Insere(Registro reg, Pagina **ap, long *comp) {
    short cresceu;           // indica se a chave subiu ate a pagina mais alta
    Registro regRetorno;     // chave que subiu 
    Pagina *apRetorno;       // ponteiro para a nova pagina criada (metade direita)
    Pagina *apTemp;          // pag temp criada se a  chave subir ate o topo

    Ins(reg, *ap, &cresceu, &regRetorno, &apRetorno, comp);

    // se a chave subiu ate a pagina mais alta
    if (cresceu) {
        apTemp = (Pagina *) malloc(sizeof(Pagina));  // cria pagina temporaria para segurar a chave que subiu
        apTemp->n = 1;                                // pagina recebe 1 chave
        apTemp->registro[0] = regRetorno;            // chave promovida

        apTemp->filhos[0] = *ap;                     // filho esquerdo = pagina antiga
        apTemp->filhos[1] = apRetorno;               // filho direito = pagina criada na divisao

        *ap = apTemp;                                // atualiza ponteiro da pagina mais alta
    }
}

void lerArquivoArvoreB(const char *nomeArquivo, int numRegistros, Pagina **raiz, long *transferencias, long *comp, double *tempo) {
    FILE *arquivo;
    Registro reg;
    int registrosLidos = 0;
    double inicio, fim;
    
    arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return;
    }
    
    *transferencias = 0;
    *comp = 0;
    *raiz = NULL;
    
    printf("Lendo %d registros de %s...\n", numRegistros, nomeArquivo);
    
    inicio = now_seconds();
    
    // le registro, insere, le registro, insere...
    while (registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
        (*transferencias)++;
        Insere(reg, raiz, comp);
        //printf("Debug\n");
        registrosLidos++;
    }
    
    fim = now_seconds();
    *tempo = ((double)(fim - inicio));
    
    fclose(arquivo);
    
    printf("Concluido! Arquivo: %s | Registros: %d | Transferências: %ld | Comparacões: %ld | Tempo: %.6f s\n",
           nomeArquivo, registrosLidos, *transferencias, *comp, *tempo);
}


// Funcao para pesquisar 20 chaves aleatorias
void pesquisar20Aleatorias(const char *nomeArquivo, int numRegistros, Pagina *raiz) {
    FILE *arquivo;
    Registro reg;
    long compTotal = 0;
    long transferencias = 0;
    double inicio, fim;
    
    printf("\n=== PESQUISANDO 20 CHAVES ALEAToRIAS ===\n");
    
    srand(time(NULL));
    inicio = now_seconds();
    
    for (int i = 0; i < 20; i++) {
        int posicao = rand() % numRegistros;
        
        // Abre arquivo para pegar chave na posicao aleatoria
        arquivo = fopen(nomeArquivo, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        transferencias++;
        fclose(arquivo);
        
       //printf("Pegando chave do arquivo na posicao %d...\n", posicao);
        //printf("Chave lida = %d\n", reg.chave);

        // Pesquisa a chave
        long comp = 0;
        Registro *resultado = pesquisa(raiz, reg.chave, &comp);
        compTotal += comp;
        
        if (resultado == NULL) {
            printf("Chave %d  nao encontrada | comps: %ld\n", reg.chave, comp);
        } else {
            printf("Chave %d encontrada | comps: %ld\n", reg.chave, comp);
        }
    }
    
    fim = now_seconds();
    double tempo = ((double)(fim - inicio));
    
    printf("Pesquisas: 20 | Transferências: %ld | Comparações totais: %ld | Tempo: %.6f s\n",  transferencias, compTotal, tempo);
}

void executarArvoreB(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {

    Pagina *raiz = NULL;
    long transferencias = 0;
    long compConstrucao = 0;
    double tempoConstrucao = 0;
    double tempoBusca = 0, inicio = 0, fim = 0;

    // imprime todas as chaves do arquivo se solicitado
    if (imprimirChaves) {
        printf("\nchaves do arquivo:\n");

        FILE *arquivo = fopen(nomeArquivo, "rb");
        Registro reg;
        int contador = 0;

        while (contador < quantidade && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
            printf("%d ", reg.chave);
            if ((contador + 1) % 10 == 0) printf("\n");
            contador++;
        }

        printf("\n");
        fclose(arquivo);
    }


    // construcao da arvore B
    lerArquivoArvoreB(nomeArquivo, quantidade, &raiz, &transferencias, &compConstrucao, &tempoConstrucao);

    // modo teste -T
    if (modoTeste) {
        pesquisar20Aleatorias(nomeArquivo, quantidade, raiz);
        printf ("Tempo de construcao da arvore : %.3f s \n", tempoConstrucao);
        printf ("Transferencias para construcao: %ld\n", transferencias);
        return;
    }

    // busca normal
    printf("\npesquisando chave %d...\n", chave);

    long compBusca = 0;
    inicio = now_seconds();
    Registro *resultado = pesquisa(raiz, chave, &compBusca);
    fim = now_seconds();
    tempoBusca = fim - inicio;

    long compTotal = compConstrucao + compBusca;

    if (resultado != NULL) {
        printf("chave encontrada\n");
        printf("chave: %d | dado1: %ld | dado2: %.50s\n", resultado->chave, resultado->dado1, resultado->dado2);
    } else {
        printf("chave nao encontrada\n");
    }

    // estatisticas finais
    printf("transferencias: %ld | comparacoes: %ld | tempo de construcao: %.3f s | tempo busca: %.3lf s n", transferencias, compTotal, tempoConstrucao, tempoBusca);
}


    

    










    










    



