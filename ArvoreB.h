#include "GameReview.h" 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct NoB {
    bool folha;
    vector<string> chaves;
    vector<long long> offsets;
    vector<NoB*> filhos;

    NoB(bool _folha);
};

class ArvoreB {
private:
    NoB *raiz;
    int t;
    int comparacoes;

    void deleteTree(NoB* x);
    void traverse(NoB* x);
    GameReview* search(NoB* x, string k);
    void splitChild(NoB* x, int i);
    void insertNonFull(NoB* x, string k, long long off);

public:
    ArvoreB(int m);
    ~ArvoreB();
    
    void print();
    GameReview* busca(string k);
    void insere(string k, long long off);
    
    int getComparacoes() { return comparacoes; }
};