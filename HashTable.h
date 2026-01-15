#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>

struct RegistroHash {
    int gameId;
    int qtdReviews;
    float mediaReviews;
    bool ocupado;
};

const int TAM_TABELA = 100003;
extern int contadorColisoes;

RegistroHash* createTable(int n, int funcaoHash);
int hashFunction1(int key);
int hashFunction2(int key);
void inserirNaTabela(RegistroHash* tabela, int id, float nota, int funcaoHash);
void exibirTopX(RegistroHash* tabela, int x);

#endif