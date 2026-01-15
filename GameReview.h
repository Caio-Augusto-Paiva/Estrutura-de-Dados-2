#ifndef GAME_REVIEW_H
#define GAME_REVIEW_H

#include <string>
#include <fstream>

struct GameReview {
    long long idRecomendacao;
    int idJogo;
    long long idAutor;
    float notaPonderada;
    
    // Novo campo para indexação
    long long offset; 

    void print() const;
    std::string getKey() const; // Retorna a chave concatenada
};

void createBinary(const std::string& path);
// Lê o registro numa posição específica
GameReview lerRegistro(long long offset);
// Retorna o tamanho do arquivo em qtd de registros
long long getTamanhoArquivo();
// Configura o caminho do binário globalmente
void setBinarioPath(const std::string& path);

#endif