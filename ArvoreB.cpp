// ArvoreB.cpp
#include "ArvoreB.h"
#include <iostream>

using namespace std;

NoB::NoB(bool _folha) {
    folha = _folha;
}

ArvoreB::ArvoreB(int m) {
    raiz = nullptr;
    t = m / 2; // Aproximação clássica: ordem m, grau min t = ceil(m/2)
    if (t < 2) t = 2;
    comparacoes = 0;
}

ArvoreB::~ArvoreB() {
    deleteTree(raiz);
}

void ArvoreB::deleteTree(NoB* x) {
    if (x == nullptr) return;
    for (auto filho : x->filhos) {
        deleteTree(filho);
    }
    delete x;
}

void ArvoreB::print() {
    if (raiz != nullptr) traverse(raiz);
}

void ArvoreB::traverse(NoB* x) {
    int i;
    for (i = 0; i < x->chaves.size(); i++) {
        if (!x->folha) traverse(x->filhos[i]);
        cout << " " << x->chaves[i];
    }
    if (!x->folha) traverse(x->filhos[i]);
}

GameReview* ArvoreB::busca(string k) {
    comparacoes = 0;
    return search(raiz, k);
}

GameReview* ArvoreB::search(NoB* x, string k) {
    if (x == nullptr) return nullptr;

    int i = 0;
    while (i < x->chaves.size() && k > x->chaves[i]) {
        comparacoes++;
        i++;
    }

    if (i < x->chaves.size() && k == x->chaves[i]) {
        comparacoes++;
        // Encontrou o offset, busca no disco
        long long offset = x->offsets[i];
        GameReview* gr = new GameReview();
        *gr = lerRegistro(offset);
        return gr;
    }

    if (x->folha) return nullptr;

    return search(x->filhos[i], k);
}

void ArvoreB::insere(string k, long long off) {
    if (raiz == nullptr) {
        raiz = new NoB(true);
        raiz->chaves.push_back(k);
        raiz->offsets.push_back(off);
    } else {
        if (raiz->chaves.size() == 2 * t - 1) {
            NoB* s = new NoB(false);
            s->filhos.push_back(raiz);
            splitChild(s, 0);
            int i = 0;
            if (s->chaves[0] < k) i++;
            insertNonFull(s->filhos[i], k, off);
            raiz = s;
        } else {
            insertNonFull(raiz, k, off);
        }
    }
}

void ArvoreB::splitChild(NoB* x, int i) {
    NoB* y = x->filhos[i];
    NoB* z = new NoB(y->folha);

    // z recebe as t-1 chaves finais de y
    for (int j = 0; j < t - 1; j++) {
        z->chaves.push_back(y->chaves[j + t]);
        z->offsets.push_back(y->offsets[j + t]);
    }

    if (!y->folha) {
        for (int j = 0; j < t; j++)
            z->filhos.push_back(y->filhos[j + t]);
    }

    // Reduz tamanho de y
    y->chaves.resize(t - 1);
    y->offsets.resize(t - 1);
    if (!y->folha) y->filhos.resize(t);

    x->filhos.insert(x->filhos.begin() + i + 1, z);
    x->chaves.insert(x->chaves.begin() + i, y->chaves[t-1]); // A mediana sobe
    x->offsets.insert(x->offsets.begin() + i, y->offsets[t-1]);
    
    // Como a mediana subiu, removemos ela de y (o resize ja cortou tecnicamente, mas garante limpeza se fosse struct manual)
}

void ArvoreB::insertNonFull(NoB* x, string k, long long off) {
    int i = x->chaves.size() - 1;

    if (x->folha) {
        // Encontra posicao e insere
        while (i >= 0 && x->chaves[i] > k) {
            i--;
        }
        x->chaves.insert(x->chaves.begin() + i + 1, k);
        x->offsets.insert(x->offsets.begin() + i + 1, off);
    } else {
        while (i >= 0 && x->chaves[i] > k) i--;
        
        if (x->filhos[i + 1]->chaves.size() == 2 * t - 1) {
            splitChild(x, i + 1);
            if (x->chaves[i + 1] < k) i++;
        }
        insertNonFull(x->filhos[i + 1], k, off);
    }
}