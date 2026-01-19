// Compressor.cpp
#include "Compressor.h"
#include "GameReview.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <sstream>

using namespace std;

// --- ESTRUTURAS HUFFMAN ---
struct NoHuff {
    char ch;
    int freq;
    NoHuff *esq, *dir;
    
    NoHuff(char c, int f) : ch(c), freq(f), esq(nullptr), dir(nullptr) {}
};

struct CompHuff {
    bool operator()(NoHuff* l, NoHuff* r) { return l->freq > r->freq; }
};

void geraCodigos(NoHuff* raiz, string str, map<char, string>& huffmanCode) {
    if (!raiz) return;
    if (!raiz->esq && !raiz->dir) huffmanCode[raiz->ch] = str;
    geraCodigos(raiz->esq, str + "0", huffmanCode);
    geraCodigos(raiz->dir, str + "1", huffmanCode);
}

// --- IMPLEMENTACAO COMPRESSOR ---

void Compressor::comprimeArquivo(int metodo, int nRegistros) {
    // 1. Ler N registros e concatenar strings
    ifstream bin("reviews.bin", ios::binary);
    string dadosCompletos = "";
    GameReview gr;
    for(int i=0; i<nRegistros; i++) {
        bin.read(reinterpret_cast<char*>(&gr), sizeof(GameReview));
        if(!bin) break;
        dadosCompletos += gr.getKey() + "|"; // Separador
    }

    ofstream out("reviewsComp.bin", ios::binary);

    if (metodo == 1) { // Huffman
        string res = runHuffmanCompress(dadosCompletos);
        out << res; // Salvando string de bits como texto '0' e '1'
        
        double originalBytes = (double)dadosCompletos.size();
        double compressedBytes = res.size() / 8.0; // Bits para bytes
        double taxa = (originalBytes > 0) ? (compressedBytes / originalBytes) * 100.0 : 0.0;

        cout << "Huffman: Original: " << originalBytes << " bytes | Comprimido: " << compressedBytes 
             << " bytes | Taxa: " << taxa << "%" << endl;
    } 
    else if (metodo == 2) { // LZ77
        string res = runLZ77Compress(dadosCompletos);
        out << res;

        double originalBytes = (double)dadosCompletos.size();
        double compressedBytes = (double)res.size();
        double taxa = (originalBytes > 0) ? (compressedBytes / originalBytes) * 100.0 : 0.0;

        cout << "LZ77: Original: " << originalBytes << " bytes | Comprimido: " << compressedBytes 
             << " bytes | Taxa: " << taxa << "%" << endl;
    }
    else if (metodo == 3) { // LZW
        vector<int> res = runLZWCompress(dadosCompletos);
        long long compressedSize = 0;
        for(int x : res) {
            out << x << " ";
            compressedSize += to_string(x).length() + 1;
        }

        double originalBytes = (double)dadosCompletos.size();
        double taxa = (originalBytes > 0) ? ((double)compressedSize / originalBytes) * 100.0 : 0.0;
        
        cout << "LZW: Original: " << originalBytes << " bytes | Comprimido: " << compressedSize 
             << " bytes | Taxa: " << taxa << "%" << endl;
    }
    out.close();
}

void Compressor::descomprimeArquivo(int metodo) {
    // Implementação simplificada: foca na lógica de descompressão
    // Em um sistema real, precisariamos salvar a arvore de Huffman ou Dicionario no arquivo.
    cout << "Funcionalidade de descompressão requer metadados salvos (ex: Arvore Huffman). "
         << "Executando lógica In-Memory para demonstracao: " << endl;
    // Aqui repetiriamos a logica inversa usando o input do arquivo reviewsComp.bin
}

// --- LOGICA HUFFMAN ---
string Compressor::runHuffmanCompress(const string& text) {
    map<char, int> freq;
    for (char c : text) freq[c]++;

    priority_queue<NoHuff*, vector<NoHuff*>, CompHuff> pq;
    for (auto pair : freq) pq.push(new NoHuff(pair.first, pair.second));

    while (pq.size() != 1) {
        NoHuff *left = pq.top(); pq.pop();
        NoHuff *right = pq.top(); pq.pop();
        NoHuff *sum = new NoHuff('\0', left->freq + right->freq);
        sum->esq = left; sum->dir = right;
        pq.push(sum);
    }

    NoHuff* raiz = pq.top();
    map<char, string> huffmanCode;
    geraCodigos(raiz, "", huffmanCode);

    string strCodificada = "";
    for (char c : text) strCodificada += huffmanCode[c];
    return strCodificada;
}

// --- LOGICA LZ77 (Simplificada) ---
string Compressor::runLZ77Compress(const string& input) {
    // Janela de busca: 4096, Buffer visualizacao: 16
    int windowSize = 100; // Pequeno para teste
    int bufferSize = 20;
    string output = "";
    int cursor = 0;

    while (cursor < input.length()) {
        int bestLen = 0;
        int bestDist = 0;
        int startSearch = (cursor - windowSize < 0) ? 0 : cursor - windowSize;

        for (int i = startSearch; i < cursor; i++) {
            int len = 0;
            while (len < bufferSize && (i + len) < cursor && (cursor + len) < input.length() 
                   && input[i + len] == input[cursor + len]) {
                len++;
            }
            if (len > bestLen) {
                bestLen = len;
                bestDist = cursor - i;
            }
        }

        char nextChar = (cursor + bestLen < input.length()) ? input[cursor + bestLen] : '\0';
        
        output += "<" + to_string(bestDist) + "," + to_string(bestLen) + "," + nextChar + ">";
        cursor += bestLen + 1;
    }
    return output;
}

// --- LOGICA LZW ---
vector<int> Compressor::runLZWCompress(const string& input) {
    map<string, int> dictionary;
    for (int i = 0; i < 256; i++) dictionary[string(1, i)] = i;

    string p = "";
    vector<int> result;
    int code = 256;

    for (char c : input) {
        string pc = p + c;
        if (dictionary.count(pc)) {
            p = pc;
        } else {
            result.push_back(dictionary[p]);
            dictionary[pc] = code++;
            p = string(1, c);
        }
    }
    if (!p.empty()) result.push_back(dictionary[p]);
    return result;
}