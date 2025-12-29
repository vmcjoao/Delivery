#ifndef PRODUTO_H
#define PRODUTO_H

#include <string>

class Produto {
public:
    int id;
    std::string nome;         // Ex: "Barril Heineken 50L" ou "Chopeira 1 Torneira 110v"
    std::string tipo;         // Ex: "BARRIL", "EQUIPAMENTO", "CONSUMIVEL"
    double preco_unitario;

    Produto() : id(0), preco_unitario(0.0) {}

    Produto(std::string n, std::string t, double p) 
        : id(0), nome(n), tipo(t), preco_unitario(p) {}
};

#endif