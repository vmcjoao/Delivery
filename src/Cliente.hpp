#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>

class Cliente {
public:
    int id;
    std::string nome;
    std::string telefone;
    std::string endereco;

    // Construtor vazio
    Cliente() : id(0) {}

    // Construtor para criar novos (sem ID ainda)
    Cliente(std::string n, std::string t, std::string e) 
        : id(0), nome(n), telefone(t), endereco(e) {}
};

#endif