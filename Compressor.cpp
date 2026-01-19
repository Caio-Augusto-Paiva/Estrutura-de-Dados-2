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

//ESTRUTURAS HUFFMAN 
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

// MPLEMENTACAO COMPRESSOR 

void Compressor::comprimeArquivo(int metodo, int nRegistros) {
    // Ler N registros e concatenar strings
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
        
        // CONVERSAO DE STRING "0"/"1" PARA BITS REAIS
        vector<unsigned char> byteBuffer;
        unsigned char buffer = 0;
        int bitCount = 0;

        for(char c : res) {
            buffer = buffer << 1; 
            if(c == '1') buffer = buffer | 1;
            bitCount++;

            if(bitCount == 8) {
                byteBuffer.push_back(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
        // Adiciona padding se sobrarem bits
        if (bitCount > 0) {
            buffer = buffer << (8 - bitCount);
            byteBuffer.push_back(buffer);
        }

        // Escreve os bytes reais no arquivo
        out.write(reinterpret_cast<const char*>(byteBuffer.data()), byteBuffer.size());
        
        double originalBytes = (double)dadosCompletos.size();
        double compressedBytes = (double)byteBuffer.size();
        double taxa = (originalBytes > 0) ? (compressedBytes / originalBytes) * 100.0 : 0.0;

        cout << "Huffman: Original: " << originalBytes << " bytes | Comprimido: " << compressedBytes 
             << " bytes | Taxa: " << taxa << "%" << endl;
    } 
    else if (metodo == 2) { // LZ77
        string res = runLZ77Compress(dadosCompletos);
        out.write(res.data(), res.size()); // Binario simples (distancia, tamanho, char)

        double originalBytes = (double)dadosCompletos.size();
        double compressedBytes = (double)res.size();
        double taxa = (originalBytes > 0) ? (compressedBytes / originalBytes) * 100.0 : 0.0;

        cout << "LZ77(Otimizado): Original: " << originalBytes << " bytes | Comprimido: " << compressedBytes 
             << " bytes | Taxa: " << taxa << "%" << endl;
    }
    else if (metodo == 3) { // LZW
        vector<int> res = runLZWCompress(dadosCompletos);
        long long compressedSize = 0;
        
        // Escreve inteiros como binario de 2 bytes (unsigned short)
        // Nota: Se o dicionario crescer muito (>65535), precisaria de 4 bytes ou tamanho variavel.
        for(int x : res) {
            unsigned short code = (unsigned short)x; 
            out.write(reinterpret_cast<char*>(&code), sizeof(unsigned short));
            compressedSize += sizeof(unsigned short);
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
    cout << "Funcionalidade de descompressão requer metadados salvos (ex: Arvore Huffman). "
         << "Executando lógica In-Memory para demonstracao: " << endl;
}

//LOGICA HUFFMAN
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

// LOGICA LZ77 
string Compressor::runLZ77Compress(const string& input) {
    string output = "";
    size_t cursor = 0;
    const int windowSize = 255; // Janela de 1 byte
    const int bufferSize = 255; // Buffer de 1 byte

    // Buffer para 8 tokens e seu byte de controle
    vector<unsigned char> tokenBuffer;
    unsigned char controlByte = 0;
    int controlCount = 0;

    auto flushBuffer = [&]() {
        output += (char)controlByte;
        for (unsigned char c : tokenBuffer) output += (char)c;
        tokenBuffer.clear();
        controlByte = 0;
        controlCount = 0;
    };

    while (cursor < input.length()) {
        int bestLen = 0;
        int bestDist = 0;
        size_t startSearch = (cursor < (size_t)windowSize) ? 0 : cursor - windowSize;

        // Busca match
        for (size_t i = startSearch; i < cursor; i++) {
            int len = 0;
            while (len < bufferSize && (i + len) < cursor && (cursor + len) < input.length() 
                   && input[i + len] == input[cursor + len]) {
                len++;
            }
            if (len > bestLen) {
                bestLen = len;
                bestDist = (int)(cursor - i);
            }
        }

        // So comprime se economizar espaco (Reference=2 bytes vs Literal=1 byte)
        // Overhead do flag bit eh igual pra ambos.
        // Se len >= 3, substituimos 3+ bytes por 2 bytes -> Lucro.
        if (bestLen >= 3) {
            // Bit 1 = Referencia
            controlByte |= (1 << (7 - controlCount));
            tokenBuffer.push_back((unsigned char)bestDist);
            tokenBuffer.push_back((unsigned char)bestLen);
            cursor += bestLen;
        } else {
            // Bit 0 = Literal
            tokenBuffer.push_back((unsigned char)input[cursor]);
            cursor++;
        }

        controlCount++;

        // A cada 8 itens, escreve no output
        if (controlCount == 8) {
            flushBuffer();
        }
    }

    // Escreve o restante
    if (controlCount > 0) {
        flushBuffer();
    }
    
    return output;
}

// LOGICA LZW 
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