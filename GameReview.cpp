#include "GameReview.h" 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

static string g_caminhoBinario = "reviews.bin";

void setBinarioPath(const string& path) {
    g_caminhoBinario = path;
}

string GameReview::getKey() const {
    return to_string(idJogo) + "_" + to_string(idAutor);
}

void GameReview::print() const {
    cout << "Offset: " << offset 
         << " | Key: " << getKey()
         << " | Nota: " << notaPonderada << endl;
}

// Retorna registro lido do disco no offset
GameReview lerRegistro(long long offset) {
    GameReview gr = {}; // Inicializa com zeros
    ifstream entrada(g_caminhoBinario, ios::binary);
    if (!entrada) {
        cerr << "Erro ao abrir arquivo para leitura no offset " << offset << endl;
        return gr;
    }
    entrada.seekg(offset, ios::beg);
    entrada.read(reinterpret_cast<char*>(&gr), sizeof(GameReview));
    gr.offset = offset; // Garante que o objeto tenha o offset correto
    return gr;
}

long long getTamanhoArquivo() {
    ifstream entrada(g_caminhoBinario, ios::binary | ios::ate);
    if (!entrada) return 0;
    return entrada.tellg();
}

void createBinary(const string& pathCsv) {
    // Mantive sua lógica original, apenas removi a verificação redundante de existência
    // para forçar a criação se necessário ou mude conforme preferir.
    string binPath = "reviews.bin"; 
    
    ifstream entrada(pathCsv);
    if (!entrada.is_open()) {
        cerr << "ERRO Nao abriu CSV: " << pathCsv << endl;
        return;
    }

    ofstream saida(binPath, ios::binary);
    string linha;
    getline(entrada, linha); // Header

    cout << "Gerando binario..." << endl;
    while (getline(entrada, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        string c1, c2, c3, c4;
        
        if (!getline(ss, c1, ',') || !getline(ss, c2, ',') || 
            !getline(ss, c3, ',') || !getline(ss, c4, ',')) continue;

        auto clean = [](string &s) { 
            if(s.size() && s.back()=='\r') s.pop_back(); 
            if(s.size() && s.front()=='"') s.erase(0,1);
            if(s.size() && s.back()=='"') s.pop_back();
        };
        clean(c1); clean(c2); clean(c3); clean(c4);

        try {
            GameReview gr;
            gr.idRecomendacao = stoll(c1);
            gr.idJogo = stoi(c2);
            gr.idAutor = stoll(c3);
            gr.notaPonderada = stof(c4);
            gr.offset = 0; // Placeholder, no disco não salvamos isso, é calculado na leitura

            saida.write(reinterpret_cast<char*>(&gr), sizeof(GameReview));
        } catch (...) { continue; }
    }
    cout << "Binario gerado!" << endl;
}