#### Programa para cálculo de caminho crítico

**Especificações**
- [Especificações](./especificacoes.md)

**Requisitos**
- CMake 2.8 ou posterior
- g\+\+ ou clang\+\+


##### Compilar e testar:
- **CMake:**
```bash
cd caminho-mistico
mkdir build
cd build
cmake ..
make
caminho_mistico ../exemplo.txt
```


- **clang\+\+:**
```bash
cd caminho-mistico
mkdir build
cd build
clang++ -std=c++11 ../src/main.cpp -o caminho_mistico
caminho_mistico ../exemplo.txt
```


- **g\+\+:**
```bash
cd caminho-mistico
mkdir build
cd build
g++ -std=c++11 -I../scr/ ../src/main.cpp -o caminho_mistico
caminho_mistico ../exemplo.txt
```


##### Uso:
```bash
./caminho_mistico caminho_do_arquivo.txt
```
