/// Calculador de Caminho Critico
/// Autores: Wendeurick Silverio / Pedro Zanatelli
///
/// Universidade Federal do Parana
/// Setor de Engenharia Elétrica
/// Gerencia de Projetos - TE064
/// Prof. Edson Pacheco
///
/// Todo:
///     - Segunda parte (projeto final)
///     - Parser com localização das chaves - permitir comentários no arquivo
///

#include <sys/stat.h>  // S_ISREG, stat
#include <algorithm>   // find, count
#include <fstream>     // ifstream
#include <iostream>    // cout
#include <map>         // map
#include <string>      // strings - implícito
#include <vector>      // vector
#include <locale>

// Para modo de compilação
// Exibe msgs de debug e teste
#define DEBUG false

#if defined (_WIN32)
# ifndef S_ISDIR
# define S_ISDIR(mode)  (((mode)& S_IFMT) == S_IFDIR)
# endif

# ifndef S_ISREG
# define S_ISREG(mode)  (((mode)& S_IFMT) == S_IFREG)
# endif
#endif

struct Day {
    int dia;
    std::vector <std::string> iniciadas;
    std::vector <std::string> finalizadas;
};

/// Escreve uma mensagem no console (std::cout)
/// @param p mensagem
#define printMistico(p) std::cout << p << "\n";

/// Escreve uma mensagem no console e fecha o programa (erro)
/// @param msg mensagem de erro
#ifdef _WIN32
# define erroMistico(msg)			\
  printMistico("\nERRO: " << msg);	\
  std::cin.get();					\
  exit(EXIT_FAILURE);
#else
# define erroMistico(msg)			\
  printMistico("\nERRO: " << msg);	\
  exit(EXIT_FAILURE);
#endif

/// Fecha o arquivo, escreve uma mensagem de erro no console
/// e fecha o programa (erro)
/// @param f arquivo
/// @param msg mensagem de erro
#define erroArquivoMistico(f, msg) \
  f.close();                       \
  erroMistico(msg);

bool isInteger(const std::string &str) {
	std::locale loc;
    for (auto digi : str) {
        if (!std::isdigit(digi, loc)) {
            return false;
        }
    }
    return true;
}

/// Exibe 1 único caminho
/// @param caminho strings com os nomes dos nós
void printCaminho(const std::vector <std::string> &caminho) {
    for (const auto &nome : caminho) {
        std::cout << nome << " ";
    }
    std::cout << "\n";
}

/// Exibe os caminhos
/// @param caminhos caminhos a serem exibidos
void printCaminhos(const std::vector <std::vector<std::string>> &caminhos) {
    std::cout << "--------------\n";
    for (int i = 0; i < caminhos.size(); ++i) {
        std::cout << "[" << i << "] ";
        printCaminho(caminhos[i]);
    }
    std::cout << "--------------\n\n";
}

/// Remove indices vazios
/// @param vetor vetor a ser analisado
void removeVazios(std::vector <std::vector<std::string>> &vetor) {
    for (long j = vetor.size() - 1; j >= 0; --j) {
        if (vetor[j].empty()) {
            vetor.erase(vetor.begin() + j);
        }
    }
}

/// Remove vetores duplicados
/// @param vetor vetor a ser analisado
/// @param indices mapa com os indices a serem removidos
void removeDuplicados(std::vector <std::vector<std::string>> &vetor) {
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

/// Chama a próxima linha do arquivo
/// @param f arquivo
/// @param l conteúdo extraido
inline void nextLine(std::ifstream &f, std::string &str) {
    if (f.is_open()) {
        std::getline(f, str);

        // Remove espaços
        auto it = std::find(str.begin(), str.end(), ' ');

        while (it != str.end()) {
            str.erase(it);
            it = std::find(str.begin(), str.end(), ' ');
        }
    }
}

/// Testa se o arquivo é válido e verifica demarcadores "#"
/// @param  filename caminho do arquivo
void testFile(const std::string &filename) {

    std::ifstream file(filename);

    // Verifica se é um arquivo regular
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

            if ('#' == line.front()) {
                const auto qtde = std::count(line.begin(), line.end(), '#');
                // Verifica se ha mais de um '#' na linha de demarcação
                if (qtde > 1) {
                    erroArquivoMistico(file, "demarcadores \"##\" invalidos");
                } else {
                    ++cont;
                }
            }
        }

        if (cont != 3) {
            erroArquivoMistico(file, "demarcadores \"#\" invalidos");
        }

    } else {
        erroMistico("nao se pode abrir o arquivo");
    }
}

/// Extrai o cabeçalho das atividades
/// @param atv vetor com as atividades: {nome, peso}
/// @param filename caminho do arquivo
void parseAtv(std::vector <std::pair<std::string, int>> &atv,
              const std::string &filename) {

    std::ifstream file(filename);

    if (file.is_open()) {

        std::string line;

        while (!file.eof()) {
            // Ignora as linhas ate que se encontre a primeira demarcação
            file.ignore(std::numeric_limits<std::streamsize>::max(), '#');

            bool cabecalho = false;

            do {
                nextLine(file, line);

                // Verifica se o cabeçalho esta antes da segunda demarcação
                if ('#' == line.front()) {
                    erroArquivoMistico(file,
                                       "cabecalho invalido "
                                               "ou nao encontrado");
                }

                std::string abre("{{");
                std::string fecha("}}");

                // Verifica se o cabeçalho inicia com "{{" e termina com "}}"
                if (line.size() > (abre.size() + fecha.size())) {
                    if (line.substr(0, abre.size()) == abre &&
                        line.substr(line.size() - fecha.size(), fecha.size()) == fecha) {
                        cabecalho = true;
                    }
                }

            } while (!cabecalho);

            // Remove o primeiro '{' e o ultimo '}'
            line.erase(0, 1);
            line.pop_back();

            // Determina a quantidade de atividades a partir
            // da quantidade de virgulas no cabeçalho
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
            if (a.second == -1) qtde++;
        }

        if (qtde > 2) {
            erroArquivoMistico(file, "cabecalho com mais de 2 extremos");
        }

        file.close();

        return;
    } else {
        erroMistico("nao se pode abrir o arquivo");
    }
}

/// Extrai as ligações entre os nós
/// @param pairs conexões entre as atividades
/// @param atv vetor com as atividades
/// @param filename caminho do arquivo
void parsePares(std::vector <std::vector<std::string>> &pairs,
                std::vector <std::pair<std::string, int>> &atv,
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

            // Adiciona as conexões ao vetor
            if ('{' == line.front()) {

                if ('}' != line.back()) {
                    erroArquivoMistico(file, "codificacao invalida");
                }

                // Determina a quantidade de virgulas
                const auto qtde = std::count(line.begin(), line.end(), ',');

                if (1 != qtde) {
                    erroArquivoMistico(file, "codificacao invalida")
                }

                // Remove o primeiro '{'
                line.erase(0, line.find('{') + 1);
                // Extrai a origem
                auto from = line.substr(0, line.find(','));
                // Extrai o destino
                line.erase(0, line.find(',') + 1);
                auto to = line.substr(0, line.find('}'));

                // Verifica se a atividade esta no cabeçalho
                bool fromWasFound = false;
                bool toWasFound = false;

                for (const auto &a : atv) {
                    if (a.first == from)
                        fromWasFound = true;
                    else if (a.first == to)
                        toWasFound = true;

                    if (fromWasFound && toWasFound) break;
                }

                if (!(fromWasFound)) {
                    erroArquivoMistico(file,
                                       "\"" << from << "\" "
                                               "nao especificada no cabecalho");
                } else if (!(toWasFound)) {
                    erroArquivoMistico(file,
                                       "\"" << to << "\" "
                                               "nao especificada no cabecalho");
                }

                pairs.push_back({from, to});
            }

        } while (!file.eof() && line.front() != '#');

    } else {
        erroMistico("nao se pode abrir o arquivo");
    }

    removeDuplicados(pairs);
}

/// Extrai as ligações entre as atividades
/// @param caminhos conexões extraídos: cada "linha" do vetor é um caminho
/// @param pairs conexões entre as atividades
/// @param atv mapa com as atividades
void parseCaminho(std::vector <std::vector<std::string>> &caminhos,
                  std::vector <std::vector<std::string>> &pairs,
                  std::vector <std::pair<std::string, int>> &atv) {

    // Encontra os nomes das atividades "inicio" e "fim" (ou seus equivalentes)
    std::string inicio, fim;

    for (const auto &a : atv) {
        if (a.second == -1) {
            if (inicio.empty()) {
                inicio = a.first;
                continue;
            } else {
                fim = a.first;
                break;
            }
        }
    }

    for (auto i = 0; i < pairs.size(); ++i) {
        if (pairs[i].front() == inicio) {
            caminhos.push_back(std::move(pairs[i]));
        }
    }

    removeVazios(pairs);

    // Percorre caminho por caminho verificando
    // se ha a necessidade de adição de um novo destino
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
    // estão sem início ou fim
    for (auto &cam : caminhos) {
        if (cam.front() != inicio || cam.back() != fim) {
            cam.clear();
        }
    }

    removeVazios(caminhos);

    // Caso haja duplicação de
    // um no no arquivo
    removeDuplicados(caminhos);
}

/// Calcula o(s) caminho(s) critico(s)
/// @param critical vetor com os indices dos caminhos críticos
/// @param path caminhos
/// @param header cabeçalho
/// @var max peso critico
int findCriticals(std::vector<int> &critical,
                  const std::vector <std::vector<std::string>> &path,
                  std::map<std::string, int> &header) {

    int max = 0;

    for (int i = 0; i < path.size(); ++i) {
        int sum = 0;

        for (const auto &c : path[i]) {
            if (header[c] > 0) {
                sum += header[c];
            }
        }
        // Adiciona o índice ao vetor caso seja o maior
        if (sum > max) {
            max = sum;
            critical.clear();
            critical.push_back(i);
        } else if (sum == max) {
            critical.push_back(i);
        }
    }
    return max;
}

void parseExecucao(std::vector<struct Day> &days,
                   std::map<std::string, int> &header,
                   const std::string &filename) {

    std::ifstream file(filename);

    if (file.is_open()) {

        std::string line;

        // Ignora as linhas ate que se encontre a terceira demarcação
        file.ignore(std::numeric_limits<std::streamsize>::max(), '#');
        file.ignore(std::numeric_limits<std::streamsize>::max(), '#');
        file.ignore(std::numeric_limits<std::streamsize>::max(), '#');

        do {

            nextLine(file, line);

            if (line.empty()) {
                continue;
            }

            const std::string inicio_str = "i:";
            // Busca por atividades iniciadas no dia
            const auto temInicio_it = line.find(inicio_str);

            const std::string final_str = "f:";
            // Busca por atividades finalizadas no dia
            const auto temFinal_it = line.find(final_str);

            const bool temInicio = (std::string::npos != temInicio_it);
            const auto temFinal = (std::string::npos != temFinal_it);

            // Verifica linha válida
            if (!temInicio && !temFinal) {
                //erroArquivoMistico(file, "linha \'execucao\' invalida");
                continue; // pode haver linha em braco
            }

            // Índice do dia
            const auto it = line.find_first_of(":{");

            if (it == std::string::npos) {
                erroArquivoMistico(file, "linha \'execucao\' invalida");
            }

            // Extrai o índice
            std::string dia_str = line.substr(0, it);

            if (!isInteger(dia_str)) {
                erroArquivoMistico(file, "linha \'execucao\' invalida");
            }

            Day thisDay;

            thisDay.dia = std::stoi(dia_str);

            if (temFinal) {
                // Extrai as atividades finalizadas
                auto finalizadas_str = line.substr(temFinal_it + final_str.size());
                finalizadas_str.pop_back(); // remove '}'

                // Prepara para a extração das atv iniciadas (if: temInicio)
                line.erase(temFinal_it);

                const auto virgulas = std::count(finalizadas_str.begin(), finalizadas_str.end(), ',') + 1;

                // Adiciona atv por atv ao vetor
                for (auto i = 0; i < virgulas; ++i) {
                    thisDay.finalizadas.push_back(finalizadas_str.substr(0, finalizadas_str.find(',')));
                    finalizadas_str.erase(0, finalizadas_str.find(',') + 1);
                }
            }

            if (temInicio) {
                // Extrai as atividades iniciadas
                auto iniciadas_str = line.substr(temInicio_it + inicio_str.size());
                iniciadas_str.pop_back(); // remove '}' ou ';'

                const auto virgulas = std::count(iniciadas_str.begin(), iniciadas_str.end(), ',') + 1;

                // Adiciona atv por atv ao vetor
                for (auto i = 0; i < virgulas; ++i) {
                    thisDay.iniciadas.push_back(iniciadas_str.substr(0, iniciadas_str.find(',')));
                    iniciadas_str.erase(0, iniciadas_str.find(',') + 1);
                }

            }

            // Verifica se as atividades iniciadas constam no cabeçalho
            if (!thisDay.iniciadas.empty()) {

                for (auto const &ini : thisDay.iniciadas) {
                    bool atvWasFound = false;

                    for (const auto &atv : header) {
                        if (ini == atv.first) {
                            atvWasFound = true;
                            break;
                        }
                    }

                    if (!atvWasFound) {
                        erroArquivoMistico(file, "atividade \"" << ini << "\" nao especificada no cabecalho");
                    }
                }
            }


            // Verifica se as atividades finalizadas constam no cabeçalho
            if (!thisDay.finalizadas.empty()) {

                for (auto const &fin : thisDay.finalizadas) {
                    bool atvWasFound = false;

                    for (const auto &atv : header) {
                        if (fin == atv.first) {
                            atvWasFound = true;
                            break;
                        }
                    }

                    if (!atvWasFound) {
                        erroArquivoMistico(file, "atividade \"" << fin << "\" nao especificada no cabecalho");
                    }
                }
            }

            // Adiciona ao vetor
            days.push_back(thisDay);

        } while (!file.eof());

    } else {
        erroMistico("nao se pode abrir o arquivo");
    }

}

int main(int argc, const char *argv[]) {

    // Verifica os argumentos do programa
    if (argc != 2) {
        std::string helpMessage =
                "arquivo invalido. Tente:\n"
                        "$ " + std::string(argv[0]) +
				" caminho/do/arquivo.txt";

        erroMistico(helpMessage);
    }

    printMistico("\n");

    // Testa o arquivo
    testFile(argv[1]);

    /// Vetor das atividades (cabeçalho)
    /// Formato de armazenamento: {{"nome", peso}, ...}
    std::vector <std::pair<std::string, int>> atividades;

    /// Vetor com as conexões
    /// Formato de armazenamento: {{"from", "to"}, ...}
    std::vector <std::vector<std::string>> pares;

    /// Vetor dos caminhos
    /// Formato de armazenamento:
    ///     {{"inicio","atvA","atvB","atvC",...,"fim"}, ...}
    std::vector <std::vector<std::string>> caminhos;

    /// Vetor da execução dos dias
    /// Formato de armazenamento:
    /// dia: dia
    /// iniciadas {atvA, AtvB, ...}
    /// finalizadas {atvC, AtvD, ...}
    std::vector<struct Day> dias;

    // Extrai o cabeçalho a partir do arquivo
    parseAtv(atividades, std::string(argv[1]));
    // Extrai as conexões entre os nós
    parsePares(pares, atividades, std::string(argv[1]));
    // Extrai os caminhos a partir dos pares
    parseCaminho(caminhos, pares, atividades);

    // Converte std::vector<std::pair> para std::map
    std::map<std::string, int> atvMap;
    for (const auto &atv : atividades) {
        atvMap[atv.first] = atv.second;
    }

    // Nao mais necessário
    atividades.clear();

    // Extrai a execução dos dias
    parseExecucao(dias, atvMap, std::string(argv[1]));

#ifdef DEBUG
    // Imprime o cabeçalho
    printMistico("CABECALHO\n--------------");
    for (const auto &atv : atividades) {
        printMistico(atv.first << ": " << atv.second);
    }
    printMistico("--------------\n");

    // Imprime as ligações
    printMistico("PARES");
    printCaminhos(pares);

#endif

    // Imprime os caminhos
    printMistico("CAMINHOS");
    printCaminhos(caminhos);

    // Vetor dos caminhos críticos
    // Armazena os indices do maior peso
    std::vector<int> criticos;

    // Calcula caminho(s) critico(s)
    const auto max = findCriticals(criticos, caminhos, atvMap);

    printMistico("CRITICOS\n--------------");
    std::cout << "Caminho(s) critico(s): duracao(" << max << ")\n";

    // Exibe o(s) caminho(s) critico(s)
    for (const auto &index : criticos) {
        std::cout << "\t[" << index << "]: ";

        for (int j = 0; j < caminhos[index].size(); ++j) {
            if (0 != j) std::cout << " - ";
            const auto peso = atvMap[caminhos[index][j]];
            std::cout << caminhos[index][j] << '(' << peso << ')';
        }

        std::cout << '\n';
    }
    printMistico("--------------\n");

    for (const auto &d : dias) {
        printMistico("Dia [" << d.dia << "]:");
    }

    return 0;
}
