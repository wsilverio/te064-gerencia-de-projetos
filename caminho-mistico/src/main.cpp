/// Calculador de Caminho Critico
/// Autores: Wendeurick Silverio / Pedro Zanatelli
///
/// Universidade Federal do Parana
/// Setor de Engenharia Eletrica
/// Gerencia de Projetos - TE064
/// Prof. Edson Pacheco
///
/// Todo:
///     - Segunda parte (projeto final)
///     - Conversor arquivos Unix <-> Windows
///

#include <sys/stat.h>   // S_ISREG, stat
#include <algorithm>    // find, count
#include <fstream>      // ifstream
#include <iostream>     // cout
#include <vector>       // vector
#include <map>          // map
#include <string>       // strings

// Windows 32 compatibility 
#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#endif
// http://www.linuxquestions.org/questions/programming-9/porting-to-win32-429334/
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
  
// Para modo de compilacao
// Exibe msgs de debug e teste
#define DEBUG

/// Escreve uma mensagem no console (std::cout)
/// @param p mensagem
#define printMistico(p) std::cout << p << "\n";

/// Escreve uma mensagem no console e fecha o programa (erro)
/// @param msg mensagem de erro
#define erroMistico(msg)                    \
  printMistico("\nERRO: " << msg << ".\n"); \
  exit(EXIT_FAILURE);

/// Fecha o arquivo, escreve uma mensagem de erro no console
/// e fecha o programa (erro)
/// @param f arquivo
/// @param msg mensagem de erro
#define erroArquivoMistico(f, msg) \
  f.close();                       \
  erroMistico(msg);

/// Exibe 1 unico caminho
/// @param caminho strings com os nomes dos nos
void printCaminho(const std::vector<std::string> &caminho) {
    for (const auto &nome : caminho) {
        std::cout << nome << " ";
    }
    std::cout << "\n";
}

/// Exibe os caminhos
/// @param caminhos caminhos a serem exibidos
void printCaminhos(const std::vector<std::vector<std::string>> &caminhos) {
    std::cout << "--------------\n";

    for (int i = 0; i < caminhos.size(); ++i) {
        std::cout << "[" << i << "] ";
        printCaminho(caminhos[i]);
    }

    std::cout << "--------------\n\n";
}

/// Remove indices vazios
/// @param vetor vetor a ser analizado
void removeVazios(std::vector<std::vector<std::string>> &vetor) {
    for (long j = vetor.size() - 1; j >= 0; --j) {
        if (vetor.at(j).empty()) {
            vetor.erase(vetor.begin() + j);
        }
    }
}

/// Remove multiplos indices de um vetor
/// @param vetor vetor a ser analizado
/// @param indices mapa com os indices a serem removidos
void removeIndices(std::vector<std::vector<std::string>> &vetor,
    std::map<int, bool> &indices) {

    // Remove os caminhos que foram duplicados
    for (const auto &index : indices) {
        vetor.at(index.first).clear();
    }

    removeVazios(vetor);
}

/// Remove vetores duplicados
/// @param vetor vetor a ser analizado
/// @param indices mapa com os indices a serem removidos
void removeDuplicados(std::vector<std::vector<std::string>> &vetor) {

    for (auto i = 0; i < vetor.size(); ++i) {
        for (auto j = 0; j < vetor.size(); ++j) {
            if (i == j) continue;
            if (vetor[i].empty()) continue;
            if (vetor[j].empty()) continue;

            if (std::equal(vetor[i].begin(), vetor[i].end(), vetor[j].begin())) {
                vetor[j].clear();
            }
        }
    }

    removeVazios(vetor);
}

/// Chama a proxima linha do arquivo
/// @param f arquivo
/// @param l conteudo extraido
inline void nextLine(std::ifstream &f, std::string &l) {
    if (f.is_open()) {
        std::getline(f, l);
    }
}

/// Testa se o arquivo e valido e verifica demarcadores "#"
/// @param  filename caminho do arquivo
void testFile(const std::string &filename) {
    std::ifstream file(filename);

    // Verifica se e um arquivo regular
    struct stat st;
    stat(filename.c_str(), &st);
    if (!S_ISREG(st.st_mode)) {
        erroArquivoMistico(file, "este nao e um arquivo regular");
    }

    // Conta a quantidade de '#' no arquivo
    if (file.is_open()) {
        int cont = 0;
        std::string line;
        while (!file.eof()) {
            nextLine(file, line);
            if ('#' == line[0]) {
                const auto qtde = std::count(line.begin(), line.end(), '#');
                // Verifica se ha mais de um '#' na linha de demarcacao
                if (qtde > 1) {
                    erroArquivoMistico(file, "demarcadores \"##\" invalidos");
                }
                else {
                    ++cont;
                }
            }
        }
        if (cont != 2) {
            erroArquivoMistico(file, "demarcadores \"#\" invalidos");
        }
    }
    else {
        erroMistico("nao se pode abrir o arquivo");
    }
}

/// Extrai o cabecalho das atividades
/// @param atv vetor com as atividades: {nome, peso}
/// @param filename caminho do arquivo
void parseAtv(std::vector<std::pair<std::string, int>> &atv, const std::string &filename) {

    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;

        while (!file.eof()) {
            // Ignora as linhas ate que se encontre a primeira demarcacao
            file.ignore(std::numeric_limits<std::streamsize>::max(), '#');

            bool cabecalho = false;

            do {
                nextLine(file, line);

                // Verifica se o cabecalho esta antes da segunda demarcacao
                if ('#' == line[0]) {
                    erroArquivoMistico(file,
                        "cabecalho invalido "
                        "ou nao encontrado");
                }

                std::string abre("{{");
                std::string fecha("}}");

                // Verifica se o cabecalho inicia com "{{" e termina com "}}"
                if (line.size() > (abre.size() + fecha.size())) {
                    if (line.substr(0, abre.size()) == abre &&
                        line.substr(line.size() - fecha.size(), fecha.size()) == fecha) {
                        cabecalho = true;
                    }
                }

            } while (!cabecalho);

            // Remove o primeiro '{' e o ultimo '}'
            line.erase(0, 1);
            line.erase(line.size() - 1, 1);

            // Determina a quantidade de atividades a partir
            // da quantidade de virgulas no cabecalho
            auto qtde = (std::count(line.begin(), line.end(), ',') + 1);

            if (qtde % 2) {
                erroArquivoMistico(file, "cabecalho invalido");
            }
            qtde /= 2;

            // Armazena {atividade, peso} no mapa
            for (int i = 0; i < qtde; ++i) {
                line.erase(0, line.find('{') + 1);
                auto name = line.substr(0, line.find(','));
                line.erase(0, line.find(',') + 1);
                int number = std::stoi(line.substr(0, line.find('}')));

                // Verifica se o mapa ja contem a atividade
                for (const auto &a : atv) {
                    if (a.first == name) {
                        erroArquivoMistico(file, "atividade \"" << name << "\" duplicada");
                    }
                }

                atv.push_back(std::make_pair(name, number));
            }

            break;
        }

        int qtde = 0;
        for (const auto &a : atv) {
            if (a.second == -1)
                qtde++;
        }

        if (qtde > 2) {
            erroArquivoMistico(file, "cabecalho com mais de 2 extremos");
        }

        file.close();

        return;
    }
    else {
        erroMistico("nao se pode abrir o arquivo");
    }
}

/// Extrai as ligacoes entre os nos
/// @param pairs conexoes entre as atividades
/// @param atv vetor com as atividades
/// @param filename caminho do arquivo
void parsePares(std::vector<std::vector<std::string>> &pairs,
    std::vector<std::pair<std::string, int>> &atv,
    const std::string &filename) {

    std::ifstream file(filename);

    if (file.is_open()) {
        // Ignora todas as linhas ate que se
        // encontre o segundo demarcador
        file.ignore(std::numeric_limits<std::streamsize>::max(), '#');
        file.ignore(std::numeric_limits<std::streamsize>::max(), '#');

        std::string line;

        do {
            nextLine(file, line);

            // Adiciona as conexoes ao vetor
            if ('{' == line[0]) {
                nextLine(file, line);
                auto from = line;

                nextLine(file, line);
                auto to = line;

                nextLine(file, line);

                if ('}' != line[0]) {
                    erroArquivoMistico(file, "codificacao invalida");
                }

                // Verifica se a atividade esta no cabecalho
                auto fromWasFound = false;
                auto toWasFound = false;

                for (const auto &a : atv) {
                    if (a.first == from) fromWasFound = true;
                    else if (a.first == to) toWasFound = true;

                    if (fromWasFound && toWasFound) break;
                }

                if (!(fromWasFound)) {
                    erroArquivoMistico(file,
                        "\"" << from << "\" "
                        "nao especificada no cabecalho");
                }
                else if (!(toWasFound)) {
                    erroArquivoMistico(file,
                        "\"" << to << "\" "
                        "nao especificada no cabecalho");
                }

                pairs.push_back({ from, to });
            }

        } while (!file.eof());

    }
    else {
        erroMistico("nao se pode abrir o arquivo");
    }

    removeDuplicados(pairs);

}

/// Extrai as ligacoes entre as atividades
/// @param caminhos conexoes extraidos: cada "linha" do vetor e um caminho
/// @param pairs conexoes entre as atividades
/// @param atv mapa com as atividades
void parseCaminho(std::vector<std::vector<std::string>> &caminhos,
    std::vector<std::vector<std::string>> &pairs,
    std::vector<std::pair<std::string, int>> &atv) {


    // Encontra os nomes das atividades "inicio" e "fim" (ou seus equivalentes)
    std::string inicio, fim;
    for (const auto &a : atv) {
        if (a.second == -1) {
            if (inicio.empty()) {
                inicio = a.first;
                continue;
            }
            else {
                fim = a.first;
                break;
            }
        }
    }

    // Adiciona ao vetor as atividades que comecam com "inicio"
    std::map<int, bool> toRemove;
    for (int i = 0; i < pairs.size(); ++i) {
        if (pairs[i].front() == inicio) {
            caminhos.push_back(pairs[i]);
            toRemove.insert(std::make_pair(i, true));
        }
    }

    removeIndices(pairs, toRemove);
    toRemove.clear();

    // Percorre caminho por caminho verificando
    // se ha a necessidade de adicicao de um novo destino
    // Se houver, DUPLICA este caminho e adiciona o destino
    for (auto index = 0; index < caminhos.size(); ++index) {
        for (const auto &par : pairs) {
            if (caminhos[index].back() == par.front()) {
                auto aux = caminhos[index];
                aux.push_back(par.back());
                caminhos.push_back(aux);
            }
        }
    }

    // Remove os caminhos que foram duplicados e que
    // estao sem inicio ou fim
    for (auto &cam : caminhos) {
        if (cam.front() != inicio || cam.back() != fim) {
            cam.clear();
        }
    }

    removeVazios(caminhos);

    // Caso haja duplicacao de
    // um no no arquivo
    removeDuplicados(caminhos);

}

/// Calcula o(s) caminho(s) critico(s)
/// @param critical vetor com os indices dos caminhos criticos
/// @param path caminhos
/// @param header cabecalho
/// @var max peso critico
int findCriticals(std::vector<int> &critical,
    const std::vector<std::vector<std::string>> &path,
    std::map<std::string, int> &header) {
    int max = 0;

    for (int i = 0; i < path.size(); ++i) {
        int sum = 0;
        for (const auto &c : path[i]) {
            if (header[c] > 0) {
                sum += header[c];
            }
        }
        // Adiciona o indice ao vetor caso seja o maior
        if (sum > max) {
            max = sum;
            critical.clear();
            critical.push_back(i);
        }
        else if (sum == max) {
            critical.push_back(i);
        }
    }
    return max;
}

int main(int argc, const char *argv[]) {

    // Verifica os argumentos do programa
    if (argc != 2) {
        std::string helpMessage =
            "\nERRO: arquivo invalido. Tente:\n"
            "$ " +
            std::string(argv[0]) + " caminho/do/arquivo.txt\n";

        printMistico(helpMessage);
        return 1;
    }

    printMistico("\n");

    // Testa o arquivo
    testFile(argv[1]);

    /// Vetor das atividades (cabecalho)
    /// Formato de armazenamento: {{"nome", peso}, ...}
    std::vector<std::pair<std::string, int>> atividades;

    // Extrai o cabecalho a partir do arquivo
    parseAtv(atividades, std::string(argv[1]));

#ifdef DEBUG
    // Imprime o cabecalho
    printMistico("CABECALHO\n--------------");
    for (const auto &atv : atividades) {
        printMistico(atv.first << ": " << atv.second);
    }
    printMistico("--------------\n");
#endif

    // Vetor com as conexoes
    std::vector<std::vector<std::string>> pares;

    // Extrai as conexoes entre os nos
    parsePares(pares, atividades, std::string(argv[1]));

#ifdef DEBUG
    // Imprime as ligacoes
    printMistico("PARES");
    printCaminhos(pares);
#endif

    /// Vetor dos caminhos
    /// Formato de armazenamento:
    ///     {{"inicio","atvA","atvB","atvC",...,"fim"}, ...}
    std::vector<std::vector<std::string>> caminhos;

    // Extrai os caminhos a partir dos pares
    parseCaminho(caminhos, pares, atividades);

    // Imprime os caminhos
    printMistico("CAMINHOS");
    printCaminhos(caminhos);

    // Converte std::vector<std::pair> para std::map
    std::map<std::string, int> atvMap;
    for (const auto &atv : atividades) {
        atvMap[atv.first] = atv.second;
    }

    // Nao mais necessario
    atividades.clear();

    // Vetor dos caminhos criticos
    // Armazena os indices do maior peso
    std::vector<int> criticos;

    // Calcula caminho(s) critico(s)
    auto max = findCriticals(criticos, caminhos, atvMap);

    printMistico("CRITICOS\n--------------");
    std::cout << "Caminho(s) critico(s): duracao(" << max << ")\n";

    // Exibe o(s) caminho(s) critico(s)
    for (const auto &index : criticos) {
        std::cout << "\t[" << index << "]: ";

        for (int j = 0; j < caminhos[index].size(); ++j) {
            if (0 != j) std::cout << " - ";
            auto peso = atvMap[caminhos[index][j]];
            std::cout << caminhos[index][j] << '(' << peso << ')';
        }

        std::cout << '\n';

    }
    printMistico("--------------\n");

    return 0;
}
