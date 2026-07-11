#include <bits/stdc++.h> // 1. Includes sempre no topo
using namespace std;

// 2. Macros e Definições logo abaixo dos includes
#define _ ios_base::sync_with_stdio(0);cin.tie(0);
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

int menorX(vector<pair<int, int>> &componente) {
    int resposta = INF;

    for(auto& ponto : componente) {
        resposta = min(resposta, ponto.second);
    }

    return resposta;
}

int maiorX(vector<pair<int, int>> &componente) {
    int resposta = INT_MIN;

    for(auto& ponto : componente) {
        resposta = max(resposta, ponto.second);
    }

    return resposta;
}

int menorXNaAltura(vector<pair<int, int>> &componente, int altura) {
    int resposta = INF;

    for(auto& ponto : componente) {
        int x = ponto.second;
        int y = ponto.first;

        if(y == altura) {
            resposta = min(resposta, ponto.second);
        }
    }

    return resposta;
}

int maiorXNaAltura(vector<pair<int, int>> &componente, int altura) {
    int resposta = INT_MIN;

    for(auto& ponto : componente) {
        int x = ponto.second;
        int y = ponto.first;

        if(y == altura) {
            resposta = max(resposta, ponto.second);
        }
    }

    return resposta;
}

int maiorY(vector<pair<int, int>> &componente) {
    int resposta = INT_MIN;

    for(auto& ponto : componente) {
        resposta = max(resposta, ponto.first);
    }

    return resposta;
}

int menorY(vector<pair<int, int>> &componente) {
    int resposta = INF;

    for(auto& ponto : componente) {
        resposta = min(resposta, ponto.first);
    }

    return resposta;
}

int ehValido(int ni, int nj, int n, int m) {
    return (ni >= 0 && ni < n && nj >= 0 && nj < m);
}

void dfs(pair<int, int> inicio, vector<vector<int>> &matriz, vector<vector<int>> &cor, int n, int m, vector<pair<int, int>> &componenteConexo, Extremos &extremos) {
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

    for(int d = 0; d < 4; d++) {
        int ni = inicio.first + dx[d];
        int nj = inicio.second + dy[d];


        if(ehValido(ni, nj, n, m) && cor[ni][nj] == 0) {
            if(matriz[ni][nj] == 1) {
                dfs({ni, nj}, matriz, cor, n, m, componenteConexo, extremos);
            }
        }
    }

}

// 3. APENAS UMA função main
int main(int argc, char* argv[]) {
    _ // Ativa o I/O rápido

    if(argc < 3) {
        cout << "Não há argumentos o suficiente.\n" << endl;
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
            arquivoSaida << mediana[mediana.size() / 2];
        }

        arquivoSaida << endl;
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

    
    for(int i = 0; i < pontosPretos.size(); i++) {
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

    int contador = 0;
    for(auto &letra : conjuntoLetras) {
        cout << "CC " << contador++ << endl;

        sort(letra.pixels.begin(), letra.pixels.end(), [](pair<int,int> &a, pair<int,int> &b){
            if(a.first != b.first) return a.first > b.first;
            return a.second > b.second;
        });
    }

    sort(conjuntoLetras.begin(), conjuntoLetras.end(), [](const Letra &a, const Letra &b) {
    if(a.pixels[0].first != b.pixels[0].first) return a.pixels[0].first < b.pixels[0].first;

        // Preciso descobrir quem é o menor b[x], pra eu comparar com ele, porque assim, eu vou comparar o maior elemento do primeiro
        // com o menor elemento do segundo array (dado que eles estão na mesma altura)

        return a.pixels[0].second < b.pixels[0].second;
    });

    // Agora eu preciso tirar a diferença entre os componentes que foram ordenados, uma vez que agora eu tenho o ponto mais abaixo e mais 
    // a direita de cada componente (sempre vai ser o primeiro elemento de acordo com a primeira ordenação que eu fiz)

    int contadorLetras = conjuntoLetras.size();
    int contadorPalavras = 0;
    int threshold = 13;

    vector<vector<Letra>> palavras;
    
    if(!conjuntoLetras.empty()) {
        contadorPalavras = 1;
        vector<Letra> palavra;
        palavra.push_back(conjuntoLetras[0]);
        for(int i = 1; i < conjuntoLetras.size(); i++) {
            
            auto& atual = conjuntoLetras[i];
            auto& anterior = conjuntoLetras[i-1];

            int yAtual = atual.borda.cantoIE.first;
            int yAnterior = anterior.borda.cantoIE.first;

            if(yAtual > yAnterior) { // Isso aqui só acontece se a altura do canto aumentar de um pro outro, isso só rola se quebrar a linha
                // Achamos uma nova linha, somo uma nova palavra
                contadorPalavras++;
                palavras.push_back(palavra);
                palavra.clear();
                palavra.push_back(atual);
                continue;
            }

            int diffX = atual.borda.cantoIE.second - anterior.borda.cantoID.second - 1; // Diferença entre elas
            
            if(diffX >= threshold) { // Se essa diferença for maior do que o meu limiar, tenho mais uma palavra
                contadorPalavras++;
                palavras.push_back(palavra);
                palavra.clear();
            }
            
            palavra.push_back(atual);
        }
    }

    cout << "Letras: " << contadorLetras << endl;
    cout << "Palavras: " << contadorPalavras << endl;

    arquivo.close();
    return 0;
}