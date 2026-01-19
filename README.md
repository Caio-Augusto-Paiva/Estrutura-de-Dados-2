# Estrutura-de-Dados-2
Um trabalho educacional onde visamos uma representação de dados balanceada e funcional

## Comandos Rápidos

### Compilar
```powershell
g++ -Wall -std=c++11 -g -o programa main.cpp GameReview.cpp HashTable.cpp ArvoreB.cpp Compressor.cpp
```

### Executar
```powershell
.\programa.exe dados
```

### Testes Automáticos
```powershell
# Arvore B (50 registros)
'1', '50', '3' | .\programa.exe dados

# Compressao Huffman (50 registros)
'2', '50', '1', '3' | .\programa.exe dados

# Compressao LZ77 (50 registros)
'2', '50', '2', '3' | .\programa.exe dados

# Relatorio Completo (saida.txt)
.\run_tests.ps1
```
