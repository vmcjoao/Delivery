#ifndef PRODUTO_H
#define PRODUTO_H

#include <string>

class Produto {
public:
    int id;
    std::string nome;       // Ex: "Chopp Pilsen 50L"
    std::string tipo;       // Ex: "BARRIL", "EQUIPAMENTO"
    double precoBase;       // Preço de tabela

    Produto() : id(0), precoBase(0.0) {}

    Produto(std::string n, std::string t, double p) 
        : id(0), nome(n), tipo(t), precoBase(p) {}
};

#endif