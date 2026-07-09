#include <bits/stdc++.h> // 1. Includes sempre no topo
using namespace std;

// 2. Macros e Definições logo abaixo dos includes
#define _ ios_base::sync_with_stdio(0);cin.tie(0);
#define endl '\n'
#define dbg(x) (cout << #x << " = " << x << endl);
typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3fll;

int ehValido(int ni, int nj, int n, int m) {
    return (ni >= 0 && ni < n && nj >= 0 && nj < m);
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
        cout << "Erro ao abrir o arquivo: " << endl;
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
            arquivoSaida << mediana[mediana.size() / 2];
        }

        arquivoSaida << endl;
    }

    cout << "tipo=" << tipo << " w=" << w << " h=" << h << endl;
    cout << "primeiro pixel=" << matriz[0][0] << endl;


    arquivo.close();
    return 0;
}