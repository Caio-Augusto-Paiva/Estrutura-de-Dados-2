// Compressor.h
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include <vector>

class Compressor {
public:
    // 1=Huffman, 2=LZ77, 3=LZW
    void comprimeArquivo(int metodo, int nRegistros);
    void descomprimeArquivo(int metodo);

private:
    std::string runHuffmanCompress(const std::string& input);
    std::string runHuffmanDecompress(const std::string& input); // Simplificado para string

    std::string runLZ77Compress(const std::string& input);
    
    std::vector<int> runLZWCompress(const std::string& input);
    std::string runLZWDecompress(const std::vector<int>& input);
};

#endif