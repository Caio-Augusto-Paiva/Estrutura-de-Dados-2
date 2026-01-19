#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include "GameReview.h"
#include "ArvoreB.h"
#include "Compressor.h"

using namespace std; 

void runTesteArvoreB(int m, int n) {
    cout << "\n--- TESTE ARVORE B (Ordem " << m << ") ---" << endl;
    ArvoreB ab(m);
    
    // Leitura sequencial para popular a arvore
    ifstream bin("reviews.bin", ios::binary);
    GameReview gr;
    long long offset = 0;
    
    auto inicio = chrono::high_resolution_clock::now();
    for(int i=0; i<n; i++) {
        offset = bin.tellg();
        bin.read(reinterpret_cast<char*>(&gr), sizeof(GameReview));
        if(!bin) break;
        ab.insere(gr.getKey(), offset);
    }
    auto fim = chrono::high_resolution_clock::now();
    cout << "Tempo Insercao: " << chrono::duration_cast<chrono::milliseconds>(fim - inicio).count() << "ms" << endl;

    // Teste de Busca
    string chaveBusca = gr.getKey(); // Pega a ultima inserida
    cout << "Buscando chave: " << chaveBusca << endl;
    
    inicio = chrono::high_resolution_clock::now();
    GameReview* res = ab.busca(chaveBusca);
    fim = chrono::high_resolution_clock::now();
    
    if (res) {
        cout << "Encontrado! ";
        res->print();
        delete res;
    } else {
        cout << "Nao encontrado." << endl;
    }
    cout << "Comparacoes: " << ab.getComparacoes() << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <dir-dados>" << endl;
        return 1;
    }

    string pasta = argv[1];
    string caminhoCsv = pasta + (pasta.back() == '/' ? "" : "/") + "reviews.csv";
    
    // Cria binario se nao existir
    createBinary(caminhoCsv);

    int opcao = 0;
    while (true) {
        cout << "1 - Analise Arvore B (m=20 e m=200)" << endl;
        cout << "2 - Analise Compressao (Huffman, LZ77, LZW)" << endl;
        cout << "3 - Sair" << endl;
        cin >> opcao;

        if (opcao == 3) break;

        switch (opcao) {
            case 1: {
                int n;
                cout << "Quantos registros indexar? ";
                cin >> n;
                runTesteArvoreB(20, n);
                runTesteArvoreB(200, n);
                break;
            }
            case 2: {
                int n, metodo;
                cout << "Quantos registros comprimir? ";
                cin >> n;
                cout << "Metodo (1-Huffman, 2-LZ77, 3-LZW): ";
                cin >> metodo;
                
                Compressor comp;
                comp.comprimeArquivo(metodo, n);
                break;
            }
        }
    }
    return 0;
}