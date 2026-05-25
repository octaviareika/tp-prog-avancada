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


double dist(Point a, Point b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double orientation(Point a, Point b, Point c){
    double val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
    if(fabs(val) < EPSILON) return 0; // considere que ele eh tao proximo de 0 que eh 0
    if (val > 0) return 1; // esquerda
    return -1; // direita - anti-horario
}

bool on_segment(Point p, Point q, Point r){
    return (q.x <= fmax(p.x, r.x) && q.x >= fmin(p.x, r.x) &&
            q.y <= fmax(p.y, r.y) && q.y >= fmin(p.y, r.y));
}


bool segments_intersect(Segment p1, Segment q1){
    double o1 = orientation(p1.a, p1.b, q1.a);
    double o2 = orientation(p1.a, p1.b, q1.b);
    double o3 = orientation(q1.a, q1.b, p1.a);
    double o4 = orientation(q1.a, q1.b, p1.b);
    
    if (o1 != o2 && o3 != o4) return true;
    
    return false;
    
}

bool isInside(Point p, double R) {
    return (p.x * p.x + p.y * p.y) <= (R * R);
}

void coletarSegmentos(ptrNo raiz, std::vector<Segment> &segmentos){

    if (!raiz) return;

    if(raiz->esq) {
        segmentos.push_back({raiz->p, raiz->esq->p});
        coletarSegmentos(raiz->esq, segmentos);
    }

    if(raiz->dir) {
        segmentos.push_back({raiz->p, raiz->dir->p});
        coletarSegmentos(raiz->dir, segmentos);
    }

}

void contarFolhas(ptrNo raiz, int &folhas){
    if (!raiz) return;

    if(!raiz->esq && !raiz->dir) { // se for folha, nao tiver filhos
        folhas++;
        return;
    }

    contarFolhas(raiz->esq, folhas);
    contarFolhas(raiz->dir, folhas);
}

ptrNo criarNo(Point p, int id){
    ptrNo novo = new No;
    novo->p = p;
    novo->id = id;
    novo->esq = nullptr;
    novo->dir = nullptr;
    novo->pai = nullptr;
    return novo;
}

int main (int argc, char* argv[]){
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <Nterm> <Raio>" << std::endl;
        return 1;
    }
    int n_term = std::stoi(argv[1]);
    double R = std::stod(argv[2]);
    int conexoes_rejeitadas = 0;
    int id_counter = 0;


    ptrNo raiz = criarNo({0, 0}, id_counter++);
    std::vector<ptrNo> todos_nos = {raiz};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist_unif(0.0, 1.0);

    for (int i = 0; i < n_term; i++){
        double r_rand = R * std::sqrt(dist_unif(gen));
        double theta_rand = dist_unif(gen) * 2 * M_PI;
        Point novo_ponto = {r_rand * cos(theta_rand), r_rand * sin(theta)};
        // achar melhor conexao
        ptrNo melhor_pai = nullptr;
        double min_dist = 1e18;
        

        for  (auto no_cand : todos_nos){
            double d = dist(no_cand->p, p_novo);
            if (d < min_dist) {
                min_dist = d;
                melhor_pai = no_cand;
            }
        }

        bool intercepta = false;
        Segment novo_seg = {melhor_pai->p, p_novo};
        
        std::vector<Segment> segs_existentes;
        coletarSegmentos(raiz, segs_existentes);

        for (const auto& s : segs_existentes) {
            if (segments_intersect(novo_seg, s)) {
                intercepta = true;
                break;
            }
        }

        if (intercepta || (melhor_pai->esq && melhor_pai->dir) || min_dist < EPSILON) {
            conexoes_rejeitadas++;
            i--; // Tenta gerar outro ponto para cumprir Nterm
            continue;
        }

        // 4. Inserir na árvore
        ptrNo novo_no = criarNo(p_novo, id_counter++, melhor_pai);
        if (!melhor_pai->esq) melhor_pai->esq = novo_no;
        else melhor_pai->dir = novo_no;
        todos_nos.push_back(novo_no);
    }


}