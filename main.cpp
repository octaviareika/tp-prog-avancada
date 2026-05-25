#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <algorithm>

#define EPSILON 0.1

typedef struct {
    double x, y;
} Point;

typedef struct {
    Point a, b;
} Segment;

typedef struct No {
    struct No *esq;
    struct No *dir;
    struct No *pai;
    Point p;
    int id;
} No, *ptrNo;


double dist(Point p1, Point p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

double orient(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool onSegment(Point p, Point q, Point r) {
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
           q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
}

bool doIntersect(Segment s1, Segment s2) {
    Point a = s1.a, b = s1.b, c = s2.a, d = s2.b;
    double o1 = orient(a, b, c);
    double o2 = orient(a, b, d);
    double o3 = orient(c, d, a);
    double o4 = orient(c, d, b);

    if (((o1 > 0 && o2 < 0) || (o1 < 0 && o2 > 0)) &&
        ((o3 > 0 && o4 < 0) || (o3 < 0 && o4 > 0))) return true;

    return false; 
}

bool isInside(Point p, double R) {
    return (p.x * p.x + p.y * p.y) <= (R * R);
}


void coletarSegmentos(ptrNo raiz, std::vector<Segment>& segmentos) {
    if (!raiz) return;
    if (raiz->esq) {
        segmentos.push_back({raiz->p, raiz->esq->p});
        coletarSegmentos(raiz->esq, segmentos);
    }
    if (raiz->dir) {
        segmentos.push_back({raiz->p, raiz->dir->p});
        coletarSegmentos(raiz->dir, segmentos);
    }
}

void contarFolhas(ptrNo raiz, int& folhas) {
    if (!raiz) return;
    if (!raiz->esq && !raiz->dir) folhas++;
    contarFolhas(raiz->esq, folhas);
    contarFolhas(raiz->dir, folhas);
}

ptrNo criarNo(Point p, int id, ptrNo pai) {
    ptrNo novo = new No;
    novo->p = p;
    novo->id = id;
    novo->pai = pai;
    novo->esq = novo->dir = nullptr;
    return novo;
}

// --- Algoritmo Principal ---

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <Nterm> <Raio>" << std::endl;
        return 1;
    }

    int n_term = std::stoi(argv[1]);
    double R = std::stod(argv[2]);
    int conexoes_rejeitadas = 0;
    int id_counter = 0;

    // Inicialização da raiz
    ptrNo raiz = criarNo({0.0, 0.0}, id_counter++, nullptr);
    std::vector<ptrNo> todos_nos = {raiz};

    // Gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist_unif(0.0, 1.0);

    for (int i = 0; i < n_term; ++i) {
        double r_rand = R * std::sqrt(dist_unif(gen));
        double theta = dist_unif(gen) * 2.0 * M_PI;
        Point p_novo = {r_rand * std::cos(theta), r_rand * std::sin(theta)};

        ptrNo melhor_pai = nullptr;
        double min_dist = 1e18;

        for (auto no_cand : todos_nos) {
            double d = dist(no_cand->p, p_novo);
            if (d < min_dist) {
                min_dist = d;
                melhor_pai = no_cand;
            }
        }

        // 3. Validar Restrições
        bool intercepta = false;
        Segment novo_seg = {melhor_pai->p, p_novo};
        
        std::vector<Segment> segs_existentes;
        coletarSegmentos(raiz, segs_existentes);

        for (const auto& s : segs_existentes) {
            if (doIntersect(novo_seg, s)) {
                intercepta = true;
                break;
            }
        }

        if (intercepta || (melhor_pai->esq && melhor_pai->dir) || min_dist < EPSILON) {
            conexoes_rejeitadas++;
            i--; // Tenta gerar outro ponto para cumprir Nterm
            continue;
        }

        ptrNo novo_no = criarNo(p_novo, id_counter++, melhor_pai);
        if (!melhor_pai->esq) melhor_pai->esq = novo_no;
        else melhor_pai->dir = novo_no;
        todos_nos.push_back(novo_no);
    }

    int folhas = 0;
    contarFolhas(raiz, folhas);
    
    std::vector<Segment> finais;
    coletarSegmentos(raiz, finais);
    double comprimento_total = 0;
    for (const auto& s : finais) comprimento_total += dist(s.a, s.b);

    std::cout << "--- Estatísticas da Árvore ---" << std::endl;
    std::cout << "Total de nós: " << id_counter << std::endl;
    std::cout << "Número de folhas: " << folhas << std::endl;
    std::cout << "Comprimento total: " << comprimento_total << std::endl;
    std::cout << "Conexões rejeitadas: " << conexoes_rejeitadas << std::endl;

    // Exportar CSV
    std::ofstream file("arvore.csv");
    file << "x1,y1,x2,y2\n";
    for (const auto& s : finais) {
        file << s.a.x << "," << s.a.y << "," << s.b.x << "," << s.b.y << "\n";
    }
    file.close();
    std::cout << "Arquivo 'arvore.csv' gerado com sucesso." << std::endl;

    return 0;
}