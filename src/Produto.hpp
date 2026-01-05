#ifndef PRODUTO_H
#define PRODUTO_H

#include <string>

class Produto {
public:
    int id;
    std::string nome;
    double precoBase;
    int tipoAtivoId;

    Produto() : id(0), precoBase(0.0), tipoAtivoId(0) {}

    Produto(std::string n, double p, int tId) 
        : id(0), nome(n), precoBase(p), tipoAtivoId(tId) {}
};
#endif