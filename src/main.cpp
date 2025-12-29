#include <iostream>
#include "Database.hpp"
#include "ClienteDAO.hpp"
#include "Input.hpp"

int main() {
    Database db("delivery.db");
    ClienteDAO clienteDAO(db);

    int opcao = -1;

    while (opcao != 0) {
        Input::limparTela();
        std::cout << "=== DELIVERY CHOPP PRO v1.0 ===\n";
        std::cout << "1. Cadastrar Novo Cliente\n";
        std::cout << "2. Listar Clientes\n";
        std::cout << "0. Sair\n";
        std::cout << "-------------------------------\n";
        
        opcao = Input::lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: {
                Input::limparTela();
                std::cout << "--- NOVO CADASTRO ---\n";
                
                std::string nome = Input::lerString("Nome Completo: ");
                std::string tel  = Input::lerString("Telefone: ");
                std::string end  = Input::lerString("Endereco: ");

                Cliente novo(nome, tel, end);
                clienteDAO.inserir(novo);
                
                Input::pausar();
                break;
            }
            case 2: {
                Input::limparTela();
                std::cout << "--- LISTA DE CLIENTES ---\n";
                
                std::vector<Cliente> lista = clienteDAO.listarTodos();
                
                if (lista.empty()) {
                    std::cout << "Nenhum cliente cadastrado.\n";
                } else {
                    for (const auto& c : lista) {
                        std::cout << "#" << c.id << " - " << c.nome 
                                  << " [" << c.telefone << "]\n";
                    }
                }
                
                Input::pausar();
                break;
            }
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                std::cout << "Opcao invalida!\n";
                Input::pausar();
        }
    }

    return 0;
}