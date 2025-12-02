#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvorebestrela.h" 

//insere uma pagina e um ponteiro em uma pagina de indices
void insereNaPaginaInterna(PaginaEstrela *pagina, int chave, PaginaEstrela *paginaDireita) {
    //inicia do final do vetor de chaves
    int k = pagina->conteudo.interna.numChaves;

    //movimenta chaves e ponteiros pra direita ate encontrar a posicao
    while (k > 0) {
        //encontra a posicao se a chave nova for maior ou igual
        if (chave >= pagina->conteudo.interna.chaves[k - 1]) {
            break;
        }
        //se nao encontrar ele move o pai e o filho pra direita 
        pagina->conteudo.interna.chaves[k] = pagina->conteudo.interna.chaves[k - 1];
        pagina->conteudo.interna.filhos[k + 1] = pagina->conteudo.interna.filhos[k];
        k--;
        
    }
    
    //insere a chave e o ponteiro na posicao encontrada
    pagina->conteudo.interna.chaves[k] = chave;
    pagina->conteudo.interna.filhos[k + 1] = paginaDireita;
    pagina->conteudo.interna.numChaves++; //incrementa contador
}

//insere um registro completo em uma pagina externa
void insereNaPaginaExterna(PaginaEstrela *pagina, Registro reg) {
    //inicia do final do vetor de registros
    int k = pagina->conteudo.externa.numRegistros;

    //movimenta registros pra direita ate encontrar a posicao
    while (k > 0) {
        //encontra a posicao se a chave nova for maior ou igual
        if (reg.chave >= pagina->conteudo.externa.registros[k - 1].chave) {
            break;
        }
        //se nao encontrar ele move o pai e o filho pra direita 
        pagina->conteudo.externa.registros[k] = pagina->conteudo.externa.registros[k - 1];
        k--;
    }
    
    //insere a chave e o ponteiro na posicao encontrada
    pagina->conteudo.externa.registros[k] = reg;
    pagina->conteudo.externa.numRegistros++; //incrementa contador
}

//retorno ponteiro para o registro se encontrar e NULL caso nao ache
Registro* pesquisaBEstrela(PaginaEstrela *pagina, int chave, long *comparacoes) {
    int i;
    
    if(pagina == NULL){
        return NULL;
    }

    //pagina interna
    if(pagina->tipo == Interna) {
        i = 1;
        
        //percorre as chaves da pagina interna para achar o filho que vai seguir
        while (i <= pagina->conteudo.interna.numChaves && chave > pagina->conteudo.interna.chaves[i - 1]) {
            i++;
            (*comparacoes)++; 
        }
        (*comparacoes)++; //conta as que saiu do loop

        //se chave <= chave[i-1] vai para o filho da esquerda, caso contrario vai para o filho da direita
        if (i <= pagina->conteudo.interna.numChaves && chave <= pagina->conteudo.interna.chaves[i-1]) {
            return pesquisaBEstrela(pagina->conteudo.interna.filhos[i-1], chave, comparacoes);
        } else {
            return pesquisaBEstrela(pagina->conteudo.interna.filhos[i], chave, comparacoes);
        }
    } 
    //pagina externa
    else {
        i = 1;
        
        //procura a chave no vetor de registros da folha
        while (i <= pagina->conteudo.externa.numRegistros && chave > pagina->conteudo.externa.registros[i - 1].chave) {
            i++;
            (*comparacoes)++; 
        }
        (*comparacoes)++; 
    
        //verifica se encontrou a chave
        if (i <= pagina->conteudo.externa.numRegistros && chave == pagina->conteudo.externa.registros[i - 1].chave) {
            return &pagina->conteudo.externa.registros[i - 1]; //retorna ponteiro para o registro
        }
        return NULL; 
    }
}

//cresceu: indica se a pagina atual ficou cheia e precisou dividir
//regRetorno: chave que sobe quando uma pagina divide
//paginaRetorno: nova pagina criada quando tem divisao
void insBEstrela(Registro reg, PaginaEstrela *pagina, short *cresceu, Registro *regRetorno, PaginaEstrela **paginaRetorno, long *comparacoes) {
    long i = 1;
    long j;
    PaginaEstrela *paginaTemp;

    //pagina interna    
    if (pagina->tipo == Interna) {
        //encontra o filho que deve descer pra inserir
        i = 0;
        while (i < pagina->conteudo.interna.numChaves && reg.chave > pagina->conteudo.interna.chaves[i]) {
            (*comparacoes)++;
            i++;
        }
        (*comparacoes)++;
        
        //faz a chamada ate achar o filho correto
        insBEstrela(reg, pagina->conteudo.interna.filhos[i], cresceu, regRetorno, paginaRetorno, comparacoes);

        if (!*cresceu){
            return;
        }

        //se o filho cresceu, subiu uma chave
        //tenta inserir a chave que subiu nesta pagina interna
        
        //se tiver espaco insere
        if (pagina->conteudo.interna.numChaves < MM_ESTRELA) {
            insereNaPaginaInterna(pagina, regRetorno->chave, *paginaRetorno);
            *cresceu = FALSO; //nao precisa subir mais
            return;
        }

        //se a pagina esta cheia precisa dividir e criar uma nova pagina
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Interna;
        paginaTemp->conteudo.interna.numChaves = 0;
        paginaTemp->conteudo.interna.filhos[0] = NULL;

        int chaveInserir = regRetorno->chave;
        PaginaEstrela *ponteiroInserir = *paginaRetorno;

        //distribui as chaves entre a pagina atual e a nova pagina
        //se a insercao eh na primeira metade, move uma chave para nova pagina
        if (i < ORDEM_ESTRELA + 1) {
            insereNaPaginaInterna(paginaTemp, pagina->conteudo.interna.chaves[MM_ESTRELA - 1], pagina->conteudo.interna.filhos[MM_ESTRELA]);
            pagina->conteudo.interna.numChaves--;
            insereNaPaginaInterna(pagina, chaveInserir, ponteiroInserir);
        } else {
            //insercao eh na segunda metade, vai direto para nova pagina
            insereNaPaginaInterna(paginaTemp, chaveInserir, ponteiroInserir);
        }

        //move a metade superior das chaves para a nova pagina
        for (j = ORDEM_ESTRELA + 1; j < MM_ESTRELA; j++) {
            insereNaPaginaInterna(paginaTemp, pagina->conteudo.interna.chaves[j], pagina->conteudo.interna.filhos[j + 1]);
        }

        //ajusta os tamanhos
        pagina->conteudo.interna.numChaves = ORDEM_ESTRELA;
        paginaTemp->conteudo.interna.filhos[0] = pagina->conteudo.interna.filhos[ORDEM_ESTRELA + 1];

        //promove a chave do meio para subir (elemento na posicao ORDEM_ESTRELA)
        regRetorno->chave = pagina->conteudo.interna.chaves[ORDEM_ESTRELA];
        *paginaRetorno = paginaTemp;
        
        return; // termina indicando que cresceu (chave subiu)
    }

    
    //pagina externa 
    else {
        //prepara o registro para retornar caso precise dividir
        *regRetorno = reg;

        //procura a posicao correta para inserir na folha
        i = 0;
        while (i < pagina->conteudo.externa.numRegistros && reg.chave > pagina->conteudo.externa.registros[i].chave) {
            (*comparacoes)++;
            i++;
        }

        //verifica se a chave ja existe 
        if (i < pagina->conteudo.externa.numRegistros && reg.chave == pagina->conteudo.externa.registros[i].chave) {
            (*comparacoes)++;
            *cresceu = FALSO; //chave ja existe, nao insere
            return;
        }

        //verifica se tem espaco na folha
        if (pagina->conteudo.externa.numRegistros < MM_ESTRELA) {
            insereNaPaginaExterna(pagina, reg);
            *cresceu = FALSO; //inseriu sem dividir
            return;
        }

        // pagina externa esta cheia, precisa dividir 
        // cria uma nova pagina externa
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Externa;
        paginaTemp->conteudo.externa.numRegistros = 0;

        // distribui os registros entre a pagina atual e a nova pagina
        // se a insercao e na primeira metade, move um registro para nova pagina
        if (i < ORDEM_ESTRELA + 1) {
            insereNaPaginaExterna(paginaTemp, pagina->conteudo.externa.registros[MM_ESTRELA - 1]);
            pagina->conteudo.externa.numRegistros--;
            insereNaPaginaExterna(pagina, reg);
        } else {
            // insercao e na segunda metade, vai direto para nova pagina
            insereNaPaginaExterna(paginaTemp, reg);
        }

        // move a metade superior dos registros para a nova pagina
        for (j = ORDEM_ESTRELA + 1; j < MM_ESTRELA; j++) {
            insereNaPaginaExterna(paginaTemp, pagina->conteudo.externa.registros[j]);
        }

        // ajusta a quantidade de registros na pagina atual
        pagina->conteudo.externa.numRegistros = ORDEM_ESTRELA + 1; 
        
        // a chave que sobe eh a primeira da nova pagina 
        *regRetorno = pagina->conteudo.externa.registros[ORDEM_ESTRELA]; 
        *paginaRetorno = paginaTemp;
        *cresceu = VERDADEIRO; // indica que precisa subir uma chave
        return;
    }
}



// insere um registro e gerencia a cricao da raiz quando necessario
void insereBEstrela(Registro reg, PaginaEstrela **raiz, long *comparacoes) {
    short cresceu = VERDADEIRO;
    Registro regRetorno;
    PaginaEstrela *paginaRetorno;
    PaginaEstrela *paginaTemp;

    // se a arvore esta vazia, cria a primeira pagina, que eh sempre externa
    if (*raiz == NULL) {
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Externa;
        paginaTemp->conteudo.externa.numRegistros = 0;
        insereNaPaginaExterna(paginaTemp, reg);
        *raiz = paginaTemp;
        return;
    }

    // chama a funcao recursiva de insercao
    insBEstrela(reg, *raiz, &cresceu, &regRetorno, &paginaRetorno, comparacoes);

    // se a raiz cresceu (dividiu), cria uma nova raiz interna
    if (cresceu) {
        paginaTemp = (PaginaEstrela *) malloc(sizeof(PaginaEstrela));
        paginaTemp->tipo = Interna;
        paginaTemp->conteudo.interna.numChaves = 1;
        paginaTemp->conteudo.interna.chaves[0] = regRetorno.chave;
        paginaTemp->conteudo.interna.filhos[0] = *raiz;         // antiga raiz (esquerda)
        paginaTemp->conteudo.interna.filhos[1] = paginaRetorno; // nova pagina (direita)
        *raiz = paginaTemp; // atualiza raiz
    }
}




// construcao da arvore via arquivo 
void lerArquivoArvoreBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela **raiz, long *transferencias, long *comparacoes, double *tempo) {
    FILE *arquivo;
    Registro reg;
    int registrosLidos = 0;
    double inicio, fim;
    
    // abre o arquivo binario para leitura
    arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return;
    }
    
    // inicializa contadores
    *transferencias = 0;
    *comparacoes = 0;
    *raiz = NULL;
    
    printf("Lendo %d registros de %s (B*)...\n", numRegistros, nomeArquivo);
    
    // marca o tempo de inicio
    inicio = now_seconds();
    
    // le registros do arquivo e insere na arvore
    while (registrosLidos < numRegistros && fread(&reg, sizeof(Registro), 1, arquivo) == 1) {
        (*transferencias)++; // conta cada leitura de disco
        insereBEstrela(reg, raiz, comparacoes); // insere na arvore
        registrosLidos++;
    }
    
    // marca o tempo de fim
    fim = now_seconds();
    *tempo = (fim - inicio); // calcula tempo decorrido
    
    fclose(arquivo);
    printf("Concluido B*! transf: %ld | comp: %ld | tempo: %.3f s\n", *transferencias, *comparacoes, *tempo);
}

// funcao de teste que pesquisa 20 chaves aleatorias na arvore
// util para testar o desempenho da busca
void pesquisar20AleatoriasBEstrela(const char *nomeArquivo, int numRegistros, PaginaEstrela *raiz) {
    FILE *arquivo;
    Registro reg;
    long comparacoesTotal = 0;
    long transferencias = 0;
    double inicio, fim;
    
    printf("\n=== PESQUISANDO 20 CHAVES ALEATORIAS (B*) ===\n");
    
    // inicializa gerador de numeros aleatorios
    srand(time(NULL));
    inicio = now_seconds();
    
    // faz 20 buscas aleatorias
    for (int i = 0; i < 20; i++) {
        // escolhe uma posicao aleatoria no arquivo
        int posicao = rand() % numRegistros;
        
        // le o registro naquela posicao do arquivo
        arquivo = fopen(nomeArquivo, "rb");
        fseek(arquivo, posicao * sizeof(Registro), SEEK_SET);
        fread(&reg, sizeof(Registro), 1, arquivo);
        transferencias++;
        fclose(arquivo);
        
        // busca a chave desse registro na arvore
        long comparacoes = 0;
        Registro *resultado = pesquisaBEstrela(raiz, reg.chave, &comparacoes);
        comparacoesTotal += comparacoes;
        
        // mostra resultado da busca
        if (resultado == NULL) printf("Chave %d nao encontrada\n", reg.chave);
        else printf("Chave %d encontrada | Comps: %ld\n", reg.chave, comparacoes);
    }
    
    fim = now_seconds();
    printf("B* Final -> Transf: %ld | Comp Total: %ld | Tempo: %.6f s\n", transferencias, comparacoesTotal, (fim - inicio));
}

void executarArvoreBEstrela(const char *nomeArquivo, int quantidade, int chave, int modoTeste, int imprimirChaves) {
    PaginaEstrela *raiz = NULL;
    long transferencias = 0, comparacoesConstrucao = 0;
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

    // construcao da arvore b*
    lerArquivoArvoreBEstrela(nomeArquivo, quantidade, &raiz, &transferencias, &comparacoesConstrucao, &tempoConstrucao);

    //teste 
    if (modoTeste) {
        pesquisar20AleatoriasBEstrela(nomeArquivo, quantidade, raiz);
        printf("Tempo de construcao da arvore : %.3f s \n", tempoConstrucao);
        printf("Transferencias para construcao: %ld\n", transferencias);
        return;
    }

    // busca normal
    printf("\npesquisando chave %d...\n", chave);

    long comparacoesBusca = 0;
    inicio = now_seconds();
    Registro *resultado = pesquisaBEstrela(raiz, chave, &comparacoesBusca);
    fim = now_seconds();
    tempoBusca = fim - inicio;

    long comparacoesTotal = comparacoesConstrucao + comparacoesBusca;

    if (resultado != NULL) {
        printf("chave encontrada\n");
        printf("chave: %d | dado1: %ld | dado2: %.50s\n", resultado->chave, resultado->dado1, resultado->dado2);
    } else {
        printf("chave nao encontrada\n");
    }

    // estatisticas finais
    printf("transferencias: %ld | comparacoes: %ld | tempo de construcao: %.3f s | tempo busca: %.3lf s\n", transferencias, comparacoesTotal, tempoConstrucao, tempoBusca);
}