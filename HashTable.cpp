#include "HashTable.h"
#include "GameReview.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;

int contadorColisoes = 0;

// Implementação da função import que estava faltando
GameReview* import(int n) {
    GameReview* reviews = new GameReview[n];
    ifstream bin("reviews.bin", ios::binary);
    if (!bin.is_open()) {
        cerr << "Erro ao abrir reviews.bin" << endl;
        return nullptr;
    }
    
    for (int i = 0; i < n; i++) {
        bin.read(reinterpret_cast<char*>(&reviews[i]), sizeof(GameReview));
        if (!bin) break;
    }
    bin.close();
    return reviews;
}

int hashFunction1(int key) {
    return key % TAM_TABELA;
}

int hashFunction2(int key) {
    double val = key * 0.618033988749895; 
    val = val - floor(val); 
    return (int)(TAM_TABELA * val);
}

RegistroHash* createTable(int n, int funcaoHash) {
    contadorColisoes = 0;

    RegistroHash* tabela = new RegistroHash[TAM_TABELA];
    for (int i = 0; i < TAM_TABELA; i++) {
        tabela[i].gameId = -1;
        tabela[i].qtdReviews = 0;
        tabela[i].mediaReviews = 0.0f;
        tabela[i].ocupado = false;
    }
    
    auto inicio = chrono::high_resolution_clock::now();

    // AQUI: Usa o import que mantivemos no GameReview.cpp
    GameReview* reviews = import(n);
    if (!reviews) {
        cout << "Falha ao carregar dados!" << endl;
        return tabela;
    }

    for (int i = 0; i < n; i++) {
        inserirNaTabela(tabela, reviews[i].idJogo, reviews[i].notaPonderada, funcaoHash);
    }

    auto fim = chrono::high_resolution_clock::now();
    auto dur = chrono::duration_cast<chrono::nanoseconds>(fim - inicio);

    cout << "\nTabela criada com sucesso!";
    cout << "\nTempo de execucao: " << dur.count() << " ns";
    cout << "\nTotal de colisoes: " << contadorColisoes << "\n\n";

    delete[] reviews;
    return tabela;
}

void inserirNaTabela(RegistroHash* tabela, int id, float nota, int f) {
    int idx = (f == 1) ? hashFunction1(id) : hashFunction2(id);
    int start = idx;
    int i = 0;
    bool primeiraColisao = false;

    while (tabela[idx].ocupado) {
        if (tabela[idx].gameId == id) {
            float soma = tabela[idx].mediaReviews * tabela[idx].qtdReviews;
            soma += nota;
            tabela[idx].qtdReviews++;
            tabela[idx].mediaReviews = soma / tabela[idx].qtdReviews;
            return;
        }
        if (!primeiraColisao) {
            contadorColisoes++;
            primeiraColisao = true;
        }
        if (i >= TAM_TABELA - 1) {
            cout << "Erro: tabela cheia!" << endl;
            return;
        }
        i++;
        idx = (start + i) % TAM_TABELA;
    }
    tabela[idx].gameId = id;
    tabela[idx].qtdReviews = 1;
    tabela[idx].mediaReviews = nota;
    tabela[idx].ocupado = true;
}

bool comp(const RegistroHash& a, const RegistroHash& b) {
    return a.qtdReviews > b.qtdReviews;
}

void exibirTopX(RegistroHash* tabela, int x) {
    vector<RegistroHash> v;
    for (int i = 0; i < TAM_TABELA; i++) {
        if (tabela[i].ocupado)
            v.push_back(tabela[i]);
    }
    sort(v.begin(), v.end(), comp);
    int limit = (x < (int)v.size()) ? x : (int)v.size();

    cout << "\nTOP " << limit << " JOGOS MAIS AVALIADOS\n";
    cout << left << setw(6) << "Rank" 
         << setw(10) << "ID" 
         << setw(10) << "Qntd" 
         << "Media" << endl;
    cout << "----------------------------------------\n";
    for (int i = 0; i < limit; i++) {
        cout << left << setw(6) << (i + 1)
             << setw(10) << v[i].gameId
             << setw(10) << v[i].qtdReviews
             << fixed << setprecision(2)
             << v[i].mediaReviews << endl;
    }
}