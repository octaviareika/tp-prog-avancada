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