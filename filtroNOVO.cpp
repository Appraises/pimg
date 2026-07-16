#include <bits/stdc++.h> // 1. Includes sempre no topo
using namespace std;

// 2. Macros e Definições logo abaixo dos includes
#define _                                                                      \
  ios_base::sync_with_stdio(0);                                                \
  cin.tie(0);
#define endl '\n'
#define dbg(x) (cout << #x << " = " << x << endl);
typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3fll;

typedef struct {
  pair<int, int> cantoSE;
  pair<int, int> cantoSD;

  pair<int, int> cantoIE;
  pair<int, int> cantoID;
} Borda;

typedef struct {
  int extremoE;
  int extremoD;
  int extremoB;
  int extremoC;
} Extremos;

typedef struct {
  vector<pair<int, int>> pixels;
  Borda borda;
} Letra;

typedef struct {
  vector<Letra> letras;
  vector<int> centrosY;
  int centroYReferencia;
} LinhaTexto;
int centroY(const Letra &l) {
  return (l.borda.cantoSE.first + l.borda.cantoIE.first) / 2;
}

int topoYReal(const Letra &l) { return l.borda.cantoSE.first + 1; }

int baseYReal(const Letra &l) { return l.borda.cantoIE.first - 1; }

int esquerdaXReal(const Letra &l) { return l.borda.cantoIE.second + 1; }

int direitaXReal(const Letra &l) { return l.borda.cantoID.second - 1; }

int alturaReal(const Letra &l) { return baseYReal(l) - topoYReal(l) + 1; }

int larguraReal(const Letra &l) {
  return direitaXReal(l) - esquerdaXReal(l) + 1;
}

int medianaInteira(vector<int> valores) {
  if (valores.empty())
    return 0;

  size_t meio = valores.size() / 2;
  nth_element(valores.begin(), valores.begin() + meio, valores.end());
  return valores[meio];
}

int estimarAlturaReferencia(const vector<Letra> &letras) {
  vector<pair<int, double>> alturasComPeso;
  double pesoTotal = 0.0;

  for (const Letra &letra : letras) {
    double peso = sqrt((double)letra.pixels.size());
    alturasComPeso.push_back({alturaReal(letra), peso});
    pesoTotal += peso;
  }

  if (alturasComPeso.empty())
    return 0;

  sort(alturasComPeso.begin(), alturasComPeso.end());
  double pesoAcumulado = 0.0;
  for (const auto &[altura, peso] : alturasComPeso) {
    pesoAcumulado += peso;
    if (pesoAcumulado >= pesoTotal / 2.0)
      return altura;
  }

  return alturasComPeso.back().first;
}
int menorX(vector<pair<int, int>> &componente) {
  int resposta = INF;

  for (auto &ponto : componente) {
    resposta = min(resposta, ponto.second);
  }

  return resposta;
}

int maiorX(vector<pair<int, int>> &componente) {
  int resposta = INT_MIN;

  for (auto &ponto : componente) {
    resposta = max(resposta, ponto.second);
  }

  return resposta;
}

int menorXNaAltura(vector<pair<int, int>> &componente, int altura) {
  int resposta = INF;

  for (auto &ponto : componente) {
    int y = ponto.first;

    if (y == altura) {
      resposta = min(resposta, ponto.second);
    }
  }

  return resposta;
}

int maiorXNaAltura(vector<pair<int, int>> &componente, int altura) {
  int resposta = INT_MIN;

  for (auto &ponto : componente) {
    int y = ponto.first;

    if (y == altura) {
      resposta = max(resposta, ponto.second);
    }
  }

  return resposta;
}

int maiorY(vector<pair<int, int>> &componente) {
  int resposta = INT_MIN;

  for (auto &ponto : componente) {
    resposta = max(resposta, ponto.first);
  }

  return resposta;
}

int menorY(vector<pair<int, int>> &componente) {
  int resposta = INF;

  for (auto &ponto : componente) {
    resposta = min(resposta, ponto.first);
  }

  return resposta;
}

int ehValido(int ni, int nj, int n, int m) {
  return (ni >= 0 && ni < n && nj >= 0 && nj < m);
}

int contarColunasPorProjecaoVertical(
    const vector<vector<int>> &matrizFiltrada) {
  if (matrizFiltrada.empty() || matrizFiltrada[0].empty()) {
    return 0;
  }

  // Varre a imagem inteira e mede quantos pixels pretos caem em cada coluna
  int h = matrizFiltrada.size();
  int w = matrizFiltrada[0].size();

  vector<int> ocupacao(w, 0);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (matrizFiltrada[i][j] == 1) {
        ocupacao[j]++;
      }
    }
  }

  // Só serão consideradas colunas realmente ocupadas, evitando contar ruído
  // isolado
  int limiarAtivo = max(3, h / 300);
  int larguraMinimaColuna = max(20, w / 40);

  int contador = 0;
  int inicioBloco = -1;

  for (int j = 0; j < w; j++) {
    if (ocupacao[j] >= limiarAtivo) {
      if (inicioBloco == -1) {
        inicioBloco = j;
      }
    } else if (inicioBloco != -1) {
      if (j - inicioBloco >= larguraMinimaColuna) {
        contador++;
      }
      inicioBloco = -1;
    }
  }

  // Se a última faixa ativa chegou até o fim da imagem, fecha a contagem aqui
  if (inicioBloco != -1 && w - inicioBloco >= larguraMinimaColuna) {
    contador++;
  }

  return max(1, contador);
}

void dfs(pair<int, int> inicio, vector<vector<int>> &matriz,
         vector<vector<int>> &cor, int n, int m,
         vector<pair<int, int>> &componenteConexo, Extremos &extremos) {
  int dx[4] = {1, -1, 0, 0};
  int dy[4] = {0, 0, 1, -1};
  vector<pair<int, int>> pilha = {inicio};
  cor[inicio.first][inicio.second] = 1;

  while (!pilha.empty()) {
    pair<int, int> atual = pilha.back();
    pilha.pop_back();

    int i = atual.first;
    int j = atual.second;
    componenteConexo.push_back(atual);

    extremos.extremoE = min(extremos.extremoE, j);
    extremos.extremoD = max(extremos.extremoD, j);
    extremos.extremoB = max(extremos.extremoB, i);
    extremos.extremoC = min(extremos.extremoC, i);

    for (int d = 0; d < 4; d++) {
      int ni = i + dx[d];
      int nj = j + dy[d];

      if (ehValido(ni, nj, n, m) && cor[ni][nj] == 0 &&
          matriz[ni][nj] == 1) {
        cor[ni][nj] = 1;
        pilha.push_back({ni, nj});
      }
    }
  }
}

void pintarBorda(vector<vector<Letra>> &palavras,
                 vector<vector<int>> &matrizFiltrada, int h, int w) {
  vector<vector<array<int, 3>>> imagemRGB(h, vector<array<int, 3>>(w));

  for (int p = 0; p < (int)palavras.size(); p++) {
    auto &palavra = palavras[p];

    int palavraExtremoE = INF;
    int palavraExtremoD = -INF;
    int palavraExtremoC = INF;
    int palavraExtremoB = -INF;

    // Encontra os extremos da palavra com base nas letras que a compõem
    for (auto &letra : palavra) {
      palavraExtremoE = min(palavraExtremoE, letra.borda.cantoIE.second);
      palavraExtremoD = max(palavraExtremoD, letra.borda.cantoID.second);
      palavraExtremoC = min(palavraExtremoC, letra.borda.cantoSE.first);
      palavraExtremoB = max(palavraExtremoB, letra.borda.cantoIE.first);
    }

    // Borda de baixo
    int linhaSublin = palavraExtremoB + 2;
    for (int espessura = 0; espessura < 1; espessura++) {
      int linhaDraw = linhaSublin + espessura;
      if (linhaDraw >= 0 && linhaDraw < h) {
        for (int col = palavraExtremoE - 2; col <= palavraExtremoD; col++) {
          if (col >= 0 && col < w) {
            matrizFiltrada[linhaDraw][col] = 1;
          }
        }
      }
    }

    // Borda de cima
    linhaSublin = palavraExtremoC + 2;
    for (int espessura = 0; espessura < 1; espessura++) {
      int linhaDraw = linhaSublin + espessura - 8;
      if (linhaDraw >= 0 && linhaDraw < h) {
        for (int col = palavraExtremoE - 1; col <= palavraExtremoD; col++) {
          if (col >= 0 && col < w) {
            matrizFiltrada[linhaDraw][col] = 1;
          }
        }
      }
    }

    // Borda de esquerda
    linhaSublin = palavraExtremoE + 2;
    for (int espessura = 0; espessura < 1; espessura++) {
      int linhaDraw = linhaSublin + espessura - 4;
      if (linhaDraw >= 0 && linhaDraw < w) {
        for (int col = palavraExtremoC - 6; col <= palavraExtremoB; col++) {
          if (col >= 0 && col < h) {
            matrizFiltrada[col][linhaDraw] = 1;
          }
        }
      }
    }

    // Borda da direita
    linhaSublin = palavraExtremoD + 2;
    for (int espessura = 0; espessura < 1; espessura++) {
      int linhaDraw = linhaSublin + espessura;
      if (linhaDraw >= 0 && linhaDraw < w) {
        for (int col = palavraExtremoC - 6; col <= palavraExtremoB; col++) {
          if (col >= 0 && col < h) {
            matrizFiltrada[col][linhaDraw] = 1;
          }
        }
      }
    }
  }
}

// 3. APENAS UMA função main
int main(int argc, char *argv[]) {
  _ // Ativa o I/O rápido

      if (argc < 3) {
    cout << "Uso: " << argv[0] << " <entrada.pbm> <saida.pbm>" << '\n';
    return 1;
  }

  string nomeArquivo = argv[1];
  string nomeArquivoSaida = argv[2];
  ifstream arquivo(nomeArquivo);
  ofstream arquivoSaida(nomeArquivoSaida);

  if (!arquivo.is_open()) {
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

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      char ci;
      arquivo >> ci;
      matriz[i][j] = ci - '0';
    }
  }

  vector<int> dx = {1, -1, 1, -1, 1, -1, 0, 0};
  vector<int> dy = {0, 0, 1, -1, -1, 1, 1, -1};

  arquivoSaida << "P1" << endl;
  arquivoSaida << w << " " << h << endl;

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      int valorCentral = matriz[i][j];
      vector<int> mediana;
      mediana.push_back(valorCentral);

      for (int d = 0; d < 8; d++) {
        int ni = i + dx[d];
        int nj = j + dy[d];

        if (ehValido(ni, nj, h, w)) {
          int valorPos = matriz[ni][nj];
          mediana.push_back(valorPos);
        }
      }

      sort(mediana.begin(), mediana.end());
      matrizFiltrada[i][j] = mediana[mediana.size() / 2];
    }
  }

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (matrizFiltrada[i][j] == 1) {
        pontosPretos.push_back({i, j});
      }
    }
  }

  int extremoE = INF;
  int extremoD = -INF;
  int extremoB = -INF;
  int extremoC = INF;

  for (size_t i = 0; i < pontosPretos.size(); i++) {
    Extremos extremos = {extremoE, extremoD, extremoB, extremoC};
    pair<int, int> parPonto = pontosPretos[i];
    if (cor[parPonto.first][parPonto.second] == 0) {
      vector<pair<int, int>> componenteConexo;
      dfs({parPonto.first, parPonto.second}, matrizFiltrada, cor, h, w,
          componenteConexo, extremos);
      if (componenteConexo.size() > 4) {
        Letra letra;
        letra.borda.cantoID = {
            extremos.extremoB + 1,
            extremos.extremoD +
                1}; // Somamos ou diminuimos um pra realmente criar uma borda
        letra.borda.cantoIE = {extremos.extremoB + 1, extremos.extremoE - 1};
        letra.borda.cantoSD = {extremos.extremoC - 1, extremos.extremoD + 1};
        letra.borda.cantoSE = {extremos.extremoC - 1, extremos.extremoE - 1};

        letra.pixels = componenteConexo;

        conjuntoLetras.push_back(letra);
      }
    }
  }

  // Estima a escala antes de descartar pontos, acentos e pontuacao.
  int alturaReferencia = estimarAlturaReferencia(conjuntoLetras);
  int alturaMinima = max(2, (int)ceil(alturaReferencia * 0.60));

  vector<Letra> letrasComAlturaValida;
  for (Letra &letra : conjuntoLetras) {
    int altura = alturaReal(letra);
    if (altura < alturaMinima)
      continue;

    letrasComAlturaValida.push_back(move(letra));
  }

  conjuntoLetras.clear();
  int larguraMaximaSeparador =
      max(1, (int)ceil(alturaReferencia * 0.20));
  for (Letra &letra : letrasComAlturaValida) {
    bool separadorVertical = larguraReal(letra) <= larguraMaximaSeparador &&
                             alturaReal(letra) > alturaReferencia * 2.50;
    if (!separadorVertical)
      conjuntoLetras.push_back(move(letra));
  }

  // Ordenacao estrita por Y; a comparacao fuzzy antiga nao era transitiva.
  sort(conjuntoLetras.begin(), conjuntoLetras.end(),
       [](const Letra &a, const Letra &b) {
         if (centroY(a) != centroY(b))
           return centroY(a) < centroY(b);
         return esquerdaXReal(a) < esquerdaXReal(b);
       });

  int toleranciaLinha = max(2, (int)lround(alturaReferencia * 0.65));
  vector<LinhaTexto> linhas;
  for (const Letra &letra : conjuntoLetras) {
    int y = centroY(letra);
    int melhorLinha = -1;
    int menorDistancia = INF;

    for (int i = 0; i < (int)linhas.size(); i++) {
      int distancia = abs(y - linhas[i].centroYReferencia);
      if (distancia <= toleranciaLinha && distancia < menorDistancia) {
        menorDistancia = distancia;
        melhorLinha = i;
      }
    }

    if (melhorLinha == -1) {
      linhas.push_back({{letra}, {y}, y});
    } else {
      LinhaTexto &linha = linhas[melhorLinha];
      linha.letras.push_back(letra);
      linha.centrosY.push_back(y);
      linha.centroYReferencia = medianaInteira(linha.centrosY);
    }
  }

  sort(linhas.begin(), linhas.end(),
       [](const LinhaTexto &a, const LinhaTexto &b) {
         return a.centroYReferencia < b.centroYReferencia;
       });

  for (LinhaTexto &linha : linhas) {
    sort(linha.letras.begin(), linha.letras.end(),
         [](const Letra &a, const Letra &b) {
           if (esquerdaXReal(a) != esquerdaXReal(b))
             return esquerdaXReal(a) < esquerdaXReal(b);
           return centroY(a) < centroY(b);
         });
  }

  // Contador de linhas, colunas e palavras
  int contadorLinhas = linhas.size();
  int contadorColunas = 0;

  int contadorPalavras = 0;
  // Mede os espacos usando as caixas reais, sem o padding de desenho.
  vector<int> todosGaps;
  for (const LinhaTexto &linha : linhas) {
    for (int i = 1; i < (int)linha.letras.size(); i++) {
      int gap = esquerdaXReal(linha.letras[i]) -
                direitaXReal(linha.letras[i - 1]) - 1;
      if (gap > 0)
        todosGaps.push_back(gap);
    }
  }

  int threshold = max(1, (int)lround(alturaReferencia * 0.35));
  vector<int> gapsCalibracao;
  int limiteGapCalibracao =
      max(1, (int)lround(alturaReferencia * 1.50));
  for (int gap : todosGaps) {
    if (gap <= limiteGapCalibracao)
      gapsCalibracao.push_back(gap);
  }
  if (!gapsCalibracao.empty()) {
    threshold = max(threshold, medianaInteira(gapsCalibracao) * 2);
  }
  contadorColunas = contarColunasPorProjecaoVertical(matrizFiltrada);

  vector<vector<Letra>> palavras;
  for (const LinhaTexto &linha : linhas) {
    if (linha.letras.empty())
      continue;

    contadorPalavras++;
    vector<Letra> palavra = {linha.letras[0]};
    for (int i = 1; i < (int)linha.letras.size(); i++) {
      const Letra &atual = linha.letras[i];
      const Letra &anterior = linha.letras[i - 1];
      int gap = esquerdaXReal(atual) - direitaXReal(anterior) - 1;

      if (gap >= threshold) {
        palavras.push_back(palavra);
        palavra.clear();
        contadorPalavras++;
      }

      palavra.push_back(atual);
    }

    if (!palavra.empty()) {
      palavras.push_back(palavra);
    }
  }

  cout << "Palavras: " << contadorPalavras << endl;
  cout << "Linhas: " << contadorLinhas << endl;
  cout << "Colunas: " << contadorColunas << endl;

  // Pinta as bordas das palavras na matrizfiltrada
  if (!palavras.empty()) {
    pintarBorda(palavras, matrizFiltrada, h, w);
  }

  // Gera a imagem PPM com as palavras e suas respectivas bordas
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      arquivoSaida << matrizFiltrada[i][j];
    }
    arquivoSaida << endl;
  }

  arquivo.close();
  return 0;
}
