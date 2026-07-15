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

int centroY(const Letra &l) {
  return (l.borda.cantoSE.first + l.borda.cantoIE.first) / 2;
}

auto esquerdaX = [](const Letra &l) { return l.borda.cantoIE.second; };

auto direitaX = [](const Letra &l) { return l.borda.cantoID.second; };

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
    int x = ponto.second;
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
    int x = ponto.second;
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
  int i = inicio.first;
  int j = inicio.second;
  cor[i][j] = 1;
  componenteConexo.push_back(inicio);

  extremos.extremoE = min(extremos.extremoE, j);
  extremos.extremoD = max(extremos.extremoD, j);
  extremos.extremoB = max(extremos.extremoB, i);
  extremos.extremoC = min(extremos.extremoC, i);
  int dx[4] = {1, -1, 0, 0};
  int dy[4] = {0, 0, 1, -1};

  for (int d = 0; d < 4; d++) {
    int ni = inicio.first + dx[d];
    int nj = inicio.second + dy[d];

    if (ehValido(ni, nj, n, m) && cor[ni][nj] == 0) {
      if (matriz[ni][nj] == 1) {
        dfs({ni, nj}, matriz, cor, n, m, componenteConexo, extremos);
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

  for (int i = 0; i < pontosPretos.size(); i++) {
    Extremos extremos = {extremoE, extremoD, extremoB, extremoC};
    pair<int, int> parPonto = pontosPretos[i];
    if (cor[parPonto.first][parPonto.second] == 0) {
      vector<pair<int, int>> componenteConexo;
      dfs({parPonto.first, parPonto.second}, matrizFiltrada, cor, h, w,
          componenteConexo, extremos);
      if (componenteConexo.size() > 4) {
        int alturaComponente = extremos.extremoB - extremos.extremoC;
        int larguraComponente = extremos.extremoD - extremos.extremoE;

        // Filtros
        // Ignora partes pequenas demais para serem letra
        if (alturaComponente <= 10 && larguraComponente <= 7)
          continue;

        // Ignora pontuação (',', '.', ':', ';')
        if ((int)componenteConexo.size() < 22)
          continue;

        // Ignora linhas retas verticais entre colunas (muito estreitas e muito
        // altas)
        if (larguraComponente <= 2 && alturaComponente > 50)
          continue;

        // Ignora o "?"
        if (larguraComponente >= 5 && alturaComponente <= 15) {
          double area =
              (double)(alturaComponente + 1) * (larguraComponente + 1);
          double densidade = (double)componenteConexo.size() / area;
          if (densidade < 0.25)
            continue;
        }
        // -------------------------------------

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

  int toleranciaLinha = 10;

  // Ordeno as palavras de cima pra baixo e da esquerda pra direita
  sort(conjuntoLetras.begin(), conjuntoLetras.end(),
       [&](const Letra &a, const Letra &b) {
         int yA = centroY(a);
         int yB = centroY(b);

         // Usando o centro da palavra + tolerancia pra não dar falso positivo
         // por conta de letras tipo p
         if (abs(yA - yB) > toleranciaLinha) {
           return yA < yB;
         }

         return esquerdaX(a) < esquerdaX(b);
       });

  // Contador de linhas, colunas e palavras
  int contadorLinhas = 1;
  int contadorColunas = 0;

  // desenha a linha da primeira letra ate a prox (dps da ordenação em ordem de
  // leitura)
  if (!conjuntoLetras.empty()) {
    vector<tuple<int, int, int, int, int>> valores;

    for (int i = 1; i < conjuntoLetras.size(); i++) {
      auto &atual = conjuntoLetras[i];
      auto &anterior = conjuntoLetras[i - 1];

      int yAtual = centroY(atual);
      int yAnterior = centroY(anterior);

      int xAnterior = (esquerdaX(anterior) + direitaX(anterior)) / 2;
      int xAtual = (esquerdaX(atual) + direitaX(atual)) / 2;

      // so desenha se as letras estão próximas (nao cruza entre colunas)

      int diffX = atual.borda.cantoIE.second - anterior.borda.cantoID.second;
      if (diffX >= 5 && diffX < 350 && abs(xAtual - xAnterior) > 35) {
        valores.push_back({atual.borda.cantoIE.second, esquerdaX(anterior),
                           esquerdaX(atual), yAtual, xAtual});
      }
    }
  }

  int contadorLetras = conjuntoLetras.size();
  int contadorPalavras = 0;

  // coleta todos os gaps horizontais entre letras consecutivas na mesma linha
  vector<int> todosGaps;
  if (!conjuntoLetras.empty()) {
    for (int i = 1; i < (int)conjuntoLetras.size(); i++) {
      auto &atual = conjuntoLetras[i];
      auto &anterior = conjuntoLetras[i - 1];

      int yAtual = centroY(atual);
      int yAnterior = centroY(anterior);

      // so considera gaps na mesma linha
      if (abs(yAtual - yAnterior) <= 12) {
        int diffX =
            atual.borda.cantoIE.second - anterior.borda.cantoID.second - 1;
        if (diffX > 0) {
          todosGaps.push_back(diffX);
        }
      }
    }
  }

  // Encontra o threshold ótimo baseado na mediana dos gaps
  int threshold = 8; // fallback
  if (!todosGaps.empty()) {
    vector<int> gapsTexto;
    for (int g : todosGaps) {
      if (g > 0 && g <= 80)
        gapsTexto.push_back(g);
    }

    if (!gapsTexto.empty()) {
      sort(gapsTexto.begin(), gapsTexto.end());
      int mediana = gapsTexto[gapsTexto.size() / 2];
      threshold = max(4, (int)(mediana * 2.5));
    }
  }

  contadorColunas = contarColunasPorProjecaoVertical(matrizFiltrada);

  vector<vector<Letra>> palavras;

  if (!conjuntoLetras.empty()) {
    contadorPalavras = 1;
    vector<Letra> palavra;
    palavra.push_back(conjuntoLetras[0]);
    for (int i = 1; i < (int)conjuntoLetras.size(); i++) {

      auto &atual = conjuntoLetras[i];
      auto &anterior = conjuntoLetras[i - 1];

      int yAtual = centroY(atual);
      int yAnterior = centroY(anterior);
      int toleranciaLinha = 12;

      // Se a diferença do centro deles for de até 4 pixels, tá tranquilo, se
      // for maior do que isso, eh uma linha nova
      if (abs(yAtual - yAnterior) > toleranciaLinha) {
        // Achamos uma nova linha, somo uma nova palavra
        contadorPalavras++;
        palavras.push_back(palavra);
        palavra.clear();
        palavra.push_back(atual);
        contadorLinhas++;
        continue;
      }

      int diffX = atual.borda.cantoIE.second - anterior.borda.cantoID.second -
                  1; // Diferença entre elas

      if (diffX >= threshold) { // Se essa diferença for maior do que o meu
                                // limiar, tengo mais uma palavra
        contadorPalavras++;
        palavras.push_back(palavra);
        palavra.clear();
      }

      palavra.push_back(atual);
    }

    // Adiciona a última palavra que ficou pendente
    if (!palavra.empty()) {
      palavras.push_back(palavra);
    }
  }

  // cout << "Letras: " << contadorLetras << endl; :(
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