#include <iostream>
#include "Database.hpp"
#include "ClienteDAO.hpp"

int main() {
    // Inicialização
    Database db("delivery.db");
    ClienteDAO clienteDAO(db);
    Cliente novoCliente("Maria do Bar", "3198888-7777", "Av. Principal, 500");

    // Salva no banco
    clienteDAO.inserir(novoCliente);

    std::cout << "\n--- Lista de Clientes ---\n";

    // Recupera lista do banco
    std::vector<Cliente> todos = clienteDAO.listarTodos();

    // Mostra na tela
    for (const auto& c : todos) {
        std::cout << "ID: " << c.id 
                  << " | Nome: " << c.nome 
                  << " | Tel: " << c.telefone << std::endl;
    }

    return 0;
}