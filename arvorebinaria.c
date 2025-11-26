#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int cod;
}Item;

typedef struct node {
    Item item;
    struct node *left;
    struct node *right;
}Node;

Node* inicializar(){
    return NULL;
}

Node* insert (Node *root, Item x){
    if (root == NULL){
        Node *aux = malloc (sizeof(Node));
        aux->item = x;
        aux->left = NULL;
        aux->right = NULL;
        return aux;
    }
    else{
        if (x.cod > root->item.cod){
            root->right = insert(root->right, x);
        }
        else if (x.cod < root->item.cod){
            root->left = insert(root->left, x);
        }
    }
    return root;
}

void treePrint (Node *root){
    if (root != NULL){
        printf ("%d" , root->item.cod);
        treePrint (root->left);
        treePrint (root->right);
    }
}

void treeFree (Node *root){
    if (root != NULL){
        treeFree (root->left);
        treeFree (root->right);
        free (root);
    }
}

Node* treeSearch(Node *root, int cod){
    if (root != NULL){
        if (root->item.cod == cod){
            return root;
        }
    }
    else{
        if(cod > root->item.cod ){
            return treeSearch(root->right, cod);
        }else{
            return treeSearch(root->left, cod);
        }
    }
    return  NULL;
}  
Node* treeRemove(Node *root, int cod) {
    if (root == NULL)
        return NULL;

    if (cod > root->item.cod) {
        root->right = treeRemove(root->right, cod);
    } else if (cod < root->item.cod) {
        root->left = treeRemove(root->left, cod);
    } else { // encontrou o nó
        if (root->left == NULL && root->right == NULL) { // folha
            free(root);
            return NULL;
        } else if (root->left == NULL) { // só tem filho à direita
            Node *aux = root->right;
            free(root);
            return aux;
        } else if (root->right == NULL) { // só tem filho à esquerda
            Node *aux = root->left;
            free(root);
            return aux;
        } else { // dois filhos
            Node *aux = root->right;
            while (aux->left != NULL) {
                aux = aux->left; // menor da subárvore direita
            }
            root->item = aux->item; // substitui valor
            root->right = treeRemove(root->right, aux->item.cod); // remove duplicado
        }
    }
    return root; // retorno correto
}


Node* inserir (Node *root, Item x){
    if(root == NULL){
        Node *aux = malloc (sizeof(Node));
        aux->item = x;
        aux->left = NULL;
        aux->right = NULL;
        return aux;
    }
    else{
        if (x.cod > root->item.cod){
            root->right = inserir (root->right, x);
        }
        else if (x.cod > root->item.cod){
            root->left = inserir (root->left, x);
        }
    }
    return root;
}


Node* remover (Node *root, int chave){
    if(root == NULL){
        return NULL;
    }

    if (chave > root->item.cod){
        root->right = remover (root->right, chave);
    }
    else if (chave < root->item.cod){
        root->left = remover(root->left, chave);
    }
    else{
        if (root->right == NULL && root->left == NULL){
            free(root);
            return NULL;
        }
        else if (root->right == NULL){
            Node *aux = root->left;
            free(root);
            return aux;
        }
        else if (root->left == NULL){
            Node *aux = root->right;
            free(root);
            return aux; 
        }
        else {
            Node *aux = root->right;
            while (aux != NULL){
                aux = aux->left;
            }
            root->item = aux->item;
            root->right = remover (root->right, aux->item.cod);
        }
    }
    return root;
}
