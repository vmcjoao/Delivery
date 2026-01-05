#ifndef TIPOATIVO_H
#define TIPOATIVO_H

#include <string>

class TipoAtivo {
public:
    int id;
    std::string nome;

    TipoAtivo() : id(0) {}
    TipoAtivo(std::string n) : id(0), nome(n) {}
};

#endif