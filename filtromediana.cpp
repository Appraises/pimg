#include <bits/stdc++.h> // 1. Includes sempre no topo
using namespace std;

// 2. Macros e Definições logo abaixo dos includes
#define _ ios_base::sync_with_stdio(0);cin.tie(0);
#define endl '\n'
#define dbg(x) (cout << #x << " = " << x << endl);
typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3fll;

// Borda guarda os cantos, que são calculados de acordo com os extremos na nossa DFS
typedef struct {
    pair<int, int> cantoSE;
    pair<int, int> cantoSD;

    pair<int, int> cantoIE;
    pair<int, int> cantoID;
} Borda;

// Estrutura de dados pra descobrir os extremos na nossa DFS
 typedef struct {
    int extremoE;
    int extremoD;
    int extremoB;
    int extremoC;
 } Extremos;

 // Cada letra tem seus pixels, e suas bordas
typedef struct {
    vector<pair<int, int>> pixels;
    Borda borda;
} Letra;

// Estrutura para guardar a linha de cada texto pra podermos calcular de
// uma melhor maneira as letras e as palavras
typedef struct {
    vector<Letra> letras;
    vector<int> centrosY;
    int centroYReferencia;
} LinhaTexto;

// Pegamos o centro de uma letra usando uma média simples
int centroY(const Letra &l) {
    return (l.borda.cantoSE.first + l.borda.cantoIE.first) / 2;
}

// Funções pra pegar o pixel mais extremo
int topoYReal(const Letra &l) {
    return l.borda.cantoSE.first + 1;
}

int baseYReal(const Letra &l) {
    return l.borda.cantoIE.first - 1;
}

int esquerdaXReal(const Letra &l) {
    return l.borda.cantoIE.second + 1;
}

int direitaXReal(const Letra &l) {
    return l.borda.cantoID.second - 1;
}

// Calculamos a altura de uma letra, dado que temos todos os pixels da caixa
int alturaReal(const Letra &l) {
    return baseYReal(l) - topoYReal(l) + 1;
}

// Calculamos a largura de uma letra, dado que temos todos os pixels da caixa
int larguraReal(const Letra &l) {
    return direitaXReal(l) - esquerdaXReal(l) + 1;
}

int medianaInteira(vector<int> valores) {
    if(valores.empty()) return 0;

    size_t meio = valores.size() / 2;
    nth_element(valores.begin(), valores.begin() + meio, valores.end());
    return valores[meio];
}

// Aqui, pra estimar a altura de uma letra, utilizamos pesos
int estimarAlturaReferencia(const vector<Letra> &letras) {
    vector<pair<int, double>> alturasComPeso; // Criamos um par aqui
    double pesoTotal = 0.0;

    for(const Letra &letra : letras) {
        double peso = sqrt((double)letra.pixels.size()); // O peso é feito pra diminuir o peso
        // dos pingos dos is por exemplo, que não devem ser usados pra calcular a altura
        // de referência do nosso texto, já que podia puxar nossa mediana pra baixo
        alturasComPeso.push_back({alturaReal(letra), peso}); // Usamos nossa função de altura real
        pesoTotal += peso; // Pegamos aqui também o peso total juntando todas as letras
    }

    // Se não tiver componentes, simplesmente paramos aqui
    if(alturasComPeso.empty()) return 0;

    // Ordenamos ele em relação a altura, e depois por peso
    sort(alturasComPeso.begin(), alturasComPeso.end());
    double pesoAcumulado = 0.0;

    // Fazmos aqui uma mediana ponderada por altura, porque assim damos mais valor pra componen
    // tes que realmente são corpos das letras
    for(const auto &[altura, peso] : alturasComPeso) {
        pesoAcumulado += peso;
        if(pesoAcumulado >= pesoTotal / 2.0) return altura; // Quando passarmos do peso total, achamos
        // exatamente a mediana entre as alturas
    }

    return alturasComPeso.back().first; // Fallback pra retornar a maior letra disponível se for o caso
}

int ehValido(int ni, int nj, int n, int m) {
    return (ni >= 0 && ni < n && nj >= 0 && nj < m);
}

int contarColunasPorProjecaoVertical(const vector<vector<int>> &matrizFiltrada) {
    if(matrizFiltrada.empty() || matrizFiltrada[0].empty()) {
        return 0;
    }

    // Varre a imagem inteira e mede quantos pixels pretos caem em cada coluna
    int h = matrizFiltrada.size();
    int w = matrizFiltrada[0].size();

    vector<int> ocupacao(w, 0);
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            if(matrizFiltrada[i][j] == 1) {
                ocupacao[j]++;
            }
        }
    }

    // Só serão consideradas colunas realmente ocupadas, evitando contar ruído isolado
    int limiarAtivo = max(3, h / 300);
    int larguraMinimaColuna = max(20, w / 40);

    int contador = 0;
    int inicioBloco = -1;

    for(int j = 0; j < w; j++) {
        if(ocupacao[j] >= limiarAtivo) {
            if(inicioBloco == -1) {
                inicioBloco = j;
            }
        } else if(inicioBloco != -1) {
            if(j - inicioBloco >= larguraMinimaColuna) {
                contador++;
            }
            inicioBloco = -1;
        }
    }

    // Se a última faixa ativa chegou até o fim da imagem, fecha a contagem aqui
    if(inicioBloco != -1 && w - inicioBloco >= larguraMinimaColuna) {
        contador++;
    }

    return max(1, contador);
}

// DFS iterativa, inicialmente tinha sido feita como recursiva, mas trocamos por podemos ter letras muito grandes
// é mais seguro termos uma pilha como estrutura de dados
void dfs(pair<int, int> inicio, vector<vector<int>> &matriz, vector<vector<int>> &cor, int n, int m, vector<pair<int, int>> &componenteConexo, Extremos &extremos) {
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};
    vector<pair<int, int>> pilha = {inicio};
    cor[inicio.first][inicio.second] = 1;

    while(!pilha.empty()) {
        pair<int, int> atual = pilha.back();
        pilha.pop_back();

        int i = atual.first;
        int j = atual.second;
        componenteConexo.push_back(atual);

        extremos.extremoE = min(extremos.extremoE, j);
        extremos.extremoD = max(extremos.extremoD, j);
        extremos.extremoB = max(extremos.extremoB, i);
        extremos.extremoC = min(extremos.extremoC, i);

        for(int d = 0; d < 4; d++) {
            int ni = i + dx[d];
            int nj = j + dy[d];

            if(ehValido(ni, nj, n, m) && cor[ni][nj] == 0 && matriz[ni][nj] == 1) {
                cor[ni][nj] = 1;
                pilha.push_back({ni, nj});
            }
        }
    }
}

void pintarBorda(vector<vector<Letra>>& palavras, vector<vector<int>>& matrizFiltrada, int h, int w) {
    vector<vector<array<int, 3>>> imagemRGB(h, vector<array<int, 3>>(w));

    for(int p = 0; p < (int)palavras.size(); p++) {
        auto& palavra = palavras[p];

        int palavraExtremoE = INF;
        int palavraExtremoD = -INF;
        int palavraExtremoC = INF;
        int palavraExtremoB = -INF;

        // Encontra os extremos da palavra com base nas letras que a compõem
        for(auto& letra : palavra) {
            palavraExtremoE = min(palavraExtremoE, letra.borda.cantoIE.second);
            palavraExtremoD = max(palavraExtremoD, letra.borda.cantoID.second);
            palavraExtremoC = min(palavraExtremoC, letra.borda.cantoSE.first);
            palavraExtremoB = max(palavraExtremoB, letra.borda.cantoIE.first);
        }

        // Borda de baixo
        int linhaSublin = palavraExtremoB + 2;
        for(int espessura = 0; espessura < 1; espessura++) {
            int linhaDraw = linhaSublin + espessura;
            if(linhaDraw >= 0 && linhaDraw < h) {
                for(int col = palavraExtremoE - 2; col <= palavraExtremoD; col++) {
                    if(col >= 0 && col < w) {
                        matrizFiltrada[linhaDraw][col] = 1;
                    }
                }
            }
        }

        // Borda de cima
        linhaSublin = palavraExtremoC + 2;
        for(int espessura = 0; espessura < 1; espessura++) {
            int linhaDraw = linhaSublin + espessura - 8;
            if(linhaDraw >= 0 && linhaDraw < h) {
                for(int col = palavraExtremoE - 1; col <= palavraExtremoD; col++) {
                    if(col >= 0 && col < w) {
                        matrizFiltrada[linhaDraw][col] = 1;
                    }
                }
            }
        }

        // Borda de esquerda
        linhaSublin = palavraExtremoE + 2;
        for(int espessura = 0; espessura < 1; espessura++) {
            int linhaDraw = linhaSublin + espessura - 4;
            if(linhaDraw >= 0 && linhaDraw < w) {
                for(int col = palavraExtremoC - 6; col <= palavraExtremoB; col++) {
                    if(col >= 0 && col < h) {
                        matrizFiltrada[col][linhaDraw] = 1;
                    }
                }
            }
        }

        // Borda da direita
        linhaSublin = palavraExtremoD + 2;
        for(int espessura = 0; espessura < 1; espessura++) {
            int linhaDraw = linhaSublin + espessura;
            if(linhaDraw >= 0 && linhaDraw < w) {
                for(int col = palavraExtremoC - 6; col <= palavraExtremoB; col++) {
                    if(col >= 0 && col < h) {
                        matrizFiltrada[col][linhaDraw] = 1;
                    }
                }
            }
        }
    }
}

// 3. APENAS UMA função main
int main(int argc, char* argv[]) {
    _ // Ativa o I/O rápido

    if(argc < 3) {
        cout << "Uso: " << argv[0] << " <entrada.pbm> <saida.pbm>" << '\n';
        return 1;
    }

    string nomeArquivo = argv[1];
    string nomeArquivoSaida = argv[2];
    ifstream arquivo(nomeArquivo);
    ofstream arquivoSaida(nomeArquivoSaida);

    if(!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo. " << endl;
        return 1;
    }

    string tipo;
    int h, w;
    string linhaComentario; 
    
    arquivo >> tipo;
    while (arquivo >> ws && arquivo.peek() == '#') {
        getline(arquivo, linhaComentario);
    }
    arquivo >> w >> h;
    cout << w << " " << h << endl;

    vector<vector<int>> matriz(h, vector<int>(w));
    vector<vector<int>> matrizFiltrada(h, vector<int>(w));
    vector<vector<int>> cor(h, vector<int>(w, 0));
    vector<pair<int, int>> pontosPretos;
    vector<Letra> conjuntoLetras;

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            char ci;
            arquivo >> ci;
            matriz[i][j] = ci - '0';
        }
    }

    vector<int> dx = {1, -1, 1, -1, 1, -1, 0, 0};
    vector<int> dy = {0, 0, 1, -1, -1, 1, 1, -1};

    arquivoSaida << "P1" << endl;
    arquivoSaida << w << " " << h << endl;

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            int valorCentral = matriz[i][j];
            vector<int> mediana;
            mediana.push_back(valorCentral);

            for(int d = 0; d < 8; d++) {
                int ni = i + dx[d];
                int nj = j + dy[d];

                if(ehValido(ni, nj, h, w)) {
                    int valorPos = matriz[ni][nj];
                    mediana.push_back(valorPos);
                }
            }

            sort(mediana.begin(), mediana.end());
            matrizFiltrada[i][j] = mediana[mediana.size() / 2];
        }
    }

    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            if(matrizFiltrada[i][j] == 1) {
                pontosPretos.push_back({i, j});
            }
        }
    }
    
    int extremoE = INF;
    int extremoD = -INF;
    int extremoB = -INF;
    int extremoC = INF;

    
    for(size_t i = 0; i < pontosPretos.size(); i++) {
        Extremos extremos = {extremoE, extremoD, extremoB, extremoC};
        pair<int, int> parPonto = pontosPretos[i];
        if(cor[parPonto.first][parPonto.second] == 0) {
            vector<pair<int, int>> componenteConexo;
            dfs({parPonto.first, parPonto.second}, matrizFiltrada, cor, h, w, componenteConexo, extremos);
            if(componenteConexo.size() > 4) {
                Letra letra;
                letra.borda.cantoID = {extremos.extremoB + 1, extremos.extremoD + 1}; // Somamos ou diminuimos um pra realmente criar uma borda
                letra.borda.cantoIE = {extremos.extremoB + 1, extremos.extremoE - 1};
                letra.borda.cantoSD = {extremos.extremoC - 1, extremos.extremoD + 1};
                letra.borda.cantoSE = {extremos.extremoC - 1, extremos.extremoE - 1};

                letra.pixels = componenteConexo;

                conjuntoLetras.push_back(letra);
            }
        }
    }

    // Estima a escala antes de descartar pontos, acentos e pontuacao.
    
    // A ideia aqui é usarmos a área pra descartar pontos, acentos, e outros
    // tipos de caractere como esse, o que eles possuem, em especial, é o fato 
    // de que possuem uma área ínfima comparado aos corpos das letras de fato
    int alturaReferencia = estimarAlturaReferencia(conjuntoLetras);
    int alturaMinima = max(2, (int)ceil(alturaReferencia * 0.60));

    vector<Letra> letrasComAlturaValida;
    for(Letra &letra : conjuntoLetras) {
        int altura = alturaReal(letra); // Calculamos a altura de cada letra
        if(altura < alturaMinima) continue; // Se a altura for menor do que a altura mínima, ignoramos

        letrasComAlturaValida.push_back(move(letra)); // Usamos move pra não ter que copiar tudo
    }

    // Limpamos nosso conjuntoLetras, que antes mais definia nossos componentes conexos do
    // que realmente letras
    conjuntoLetras.clear();

    // Função para remover possíveis separadores no nosso texto, tipo linhas verticas
    // ou horizontais que podem atrapalhar na leitura


    // Pra ser um separador, ou ele é muito estreito, tendo algo como 20% da nossa
    // altura de referência, ou ele é extremamente alto, sendo pelo menos 2.5x maior
    int larguraMaximaSeparador = max(1, (int)ceil(alturaReferencia * 0.20)); 

    // Rodamos isso pra cada letra com altura válida, e assim vamos ter nosso conjunto de letras real
    for(Letra &letra : letrasComAlturaValida) {
        bool separadorVertical = larguraReal(letra) <= larguraMaximaSeparador
            && alturaReal(letra) > alturaReferencia * 2.50;
        if(!separadorVertical) conjuntoLetras.push_back(move(letra));
    }

    // Aqui organizamos tudo de cima para baixo, e aí quando duas letras têm o mesmo centro vertical, 
    // organiza da esquerda para a direita.
    sort(conjuntoLetras.begin(), conjuntoLetras.end(), [](const Letra &a, const Letra &b) {
        if(centroY(a) != centroY(b)) return centroY(a) < centroY(b);
        return esquerdaXReal(a) < esquerdaXReal(b);
    });

    // Forma faixas com tolerancia proporcional a altura das letras. Somente
    // depois cada faixa e ordenada da esquerda para a direita.

    // Nossa tolerância entre linhas, é sempre algo como 65% da nossa altura de referência entre 
    // as letras
    int toleranciaLinha = max(2, (int)lround(alturaReferencia * 0.65));
    vector<LinhaTexto> linhas;


    for(const Letra &letra : conjuntoLetras) {
        int y = centroY(letra); // Centro do y da letra
        int melhorLinha = -1;
        int menorDistancia = INF;

        for(int i = 0; i < (int)linhas.size(); i++) {

            // Para cada linha, procuramos a linha cujo o centro vertical seja o mais próximo 
            int distancia = abs(y - linhas[i].centroYReferencia);
            if(distancia <= toleranciaLinha && distancia < menorDistancia) {
                menorDistancia = distancia;
                melhorLinha = i;
            }
        }

        // Se não encontrou, achamos uma nova linha, então adicionamos ela
        if(melhorLinha == -1) {
            linhas.push_back({{letra}, {y}, y}); // Lembrando que LinhasTexto tem o y de referência da linha
            // o centro vertical da letra, e a letra de fato (que por consequência guarda os pixels daquela letra)
        } else {
            LinhaTexto &linha = linhas[melhorLinha]; // Se eu achei a linha, então eu adiciono letras naquela linha, assim
                                                    // como os centros de cada Y, pra gente poder usar pra levar como referência
            linha.letras.push_back(letra);
            linha.centrosY.push_back(y);

            // Quando uma nova letra entra, recalculamos o Y de referência daquela linha, usando mediana
            linha.centroYReferencia = medianaInteira(linha.centrosY);
        }
    }

    // Ordenamos então as linhas de cima pra baixo
    sort(linhas.begin(), linhas.end(), [](const LinhaTexto &a, const LinhaTexto &b) {
        return a.centroYReferencia < b.centroYReferencia;
    });

    // E pra cada linha que conseguimos, ordenamos as letras em relação a x
    for(LinhaTexto &linha : linhas) {
        sort(linha.letras.begin(), linha.letras.end(), [](const Letra &a, const Letra &b) {
            if(esquerdaXReal(a) != esquerdaXReal(b)) return esquerdaXReal(a) < esquerdaXReal(b);
            return centroY(a) < centroY(b);
        });
    }

    // ------------------------------------------------------------------

    // Aqui agora começa a contagem de linhas, palavras e colunas

    int contadorLinhas = linhas.size();
    int contadorColunas = 0;

    int contadorPalavras = 0;

    // Mede os espacos com a caixa real do componente. A borda expandida em um
    // pixel continua existindo apenas para o desenho.
    vector<int> todosGaps;

    // Entre letras da mesma linha, verificamos o espaço entre elas (os gaps)
    for(const LinhaTexto &linha : linhas) {
        for(int i = 1; i < (int)linha.letras.size(); i++) {

            // Calculamos cada gap
            int gap = esquerdaXReal(linha.letras[i]) - direitaXReal(linha.letras[i - 1]) - 1;

            // Se existir o gap, adicionamos ele a todos os gaps
            if(gap > 0) todosGaps.push_back(gap);
        }
    }

    // O limiar acompanha o espacamento observado e a altura da fonte. Gaps
    // enormes entre colunas nao participam da calibracao.
    int threshold = max(1, (int)lround(alturaReferencia * 0.35));
    vector<int> gapsCalibracao;

    int limiteGapCalibracao = max(1, (int)lround(alturaReferencia * 1.50)); 

    // Somente os gaps razoáveis usados para descobrir qual é o espaçamento normal da fonte
    // Já que aqui podemos ter gaps entre colunas, que foi definido como o limiteGapCalibracao
    for(int gap : todosGaps) {
        if(gap <= limiteGapCalibracao) gapsCalibracao.push_back(gap);
    }

    // Se o gaps de calibração não estiverem vazios, pegamos o máximo entre 
    // o threshold que tinhamos definido antes, e a medianaInteira multiplicada por 2
    if(!gapsCalibracao.empty()) {
        threshold = max(threshold, medianaInteira(gapsCalibracao) * 2);
        // A ideia é multiplicar por 2 já que nossa mediana possivelmente vai dar algo
        // próximo entre os espaços entre letras, algo 2x maior que isso é um valor palpável
    }

    // Usamos nossa função de contar colunas por projetação vertical
    contadorColunas = contarColunasPorProjecaoVertical(matrizFiltrada);

    vector<vector<Letra>> palavras;
    for(const LinhaTexto &linha : linhas) {
        if(linha.letras.empty()) continue; // Se não tiver linhas, simplesmente pula

        contadorPalavras++; // Caso contrário somamos uma palavra
        vector<Letra> palavra = {linha.letras[0]}; 
        for(int i = 1; i < (int)linha.letras.size(); i++) {
            const Letra &atual = linha.letras[i];
            const Letra &anterior = linha.letras[i - 1];

            int gap = esquerdaXReal(atual) - direitaXReal(anterior) - 1;
            
            // Aqui agora, usando nosso threshold, se o gap for maior do que o threshold
            // nós temos uma palavra
            if(gap >= threshold) {
                palavras.push_back(palavra);
                palavra.clear();
                contadorPalavras++;
            }

            // Caso contrário temos uma letra
            palavra.push_back(atual);
        }

        // Se tiver letras na palavra, adicionamos ela como uma palavra válida
        if(!palavra.empty()) {
            palavras.push_back(palavra);
        }
    }

    // -----------------------------------------------



    cout << "Palavras: " << contadorPalavras << endl;
    cout << "Linhas: " << contadorLinhas << endl;
    cout << "Colunas: " << contadorColunas << endl;

    // Pinta as bordas das palavras na matrizfiltrada
    if(!palavras.empty()) {
        pintarBorda(palavras, matrizFiltrada, h, w);
    }

    // Gera a imagem PPM com as palavras e suas respectivas bordas
    for(int i = 0; i < h; i++) {
        for(int j = 0; j < w; j++) {
            arquivoSaida << matrizFiltrada[i][j];
        }
        arquivoSaida << endl;
    }

    arquivo.close();
    return 0;
}
