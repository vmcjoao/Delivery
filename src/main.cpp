#include <iostream>
#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "ClienteDAO.hpp"
#include "ProdutoDAO.hpp"
#include "Input.hpp"

// --- SUBMENUS ---

void menuClientes(ClienteDAO& dao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- GESTAO DE CLIENTES ---\n";
        std::cout << "1. Novo Cliente\n";
        std::cout << "2. Listar Todos\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::string nome = Input::lerString("Nome: ");
            std::string tel = Input::lerString("Telefone: ");
            std::string end = Input::lerString("Endereco: ");
            Cliente c(nome, tel, end);
            dao.inserir(c);
            Input::pausar();
        } 
        else if (opcao == 2) {
            std::vector<Cliente> lista = dao.listarTodos();
            std::cout << "\nLISTAGEM:\n";
            for (auto& c : lista) {
                std::cout << "#" << c.id << " " << c.nome << " (" << c.telefone << ")\n";
            }
            Input::pausar();
        }
    }
}

void menuProdutos(ProdutoDAO& dao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- GESTAO DE PRODUTOS/ESTOQUE ---\n";
        std::cout << "1. Novo Produto (Barril/Equipamento)\n";
        std::cout << "2. Catálogo de Produtos\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::string nome = Input::lerString("Nome do Produto (ex: Barril Pilsen 50L): "); 
            // TODO: Estruturar melhor, talvez uma classe por produto
            
            std::cout << "Tipos: [BARRIL] [EQUIPAMENTO] [OUTROS]\n";
            std::string tipo = Input::lerString("Tipo do Produto: ");
            
            double preco = Input::lerDecimal("Preco Unitario (0 para equipamentos): ");
            
            Produto p(nome, tipo, preco);
            dao.inserir(p);
            Input::pausar();
        }
        else if (opcao == 2) {
            std::vector<Produto> lista = dao.listarTodos();
            std::cout << "\nCATALOGO:\n";
            std::cout << std::fixed << std::setprecision(2); 
            
            for (auto& p : lista) {
                std::cout << "#" << p.id << " | " << p.nome 
                          << " [" << p.tipo << "] - R$ " << p.preco_unitario << "\n";
            }
            Input::pausar();
        }
    }
}

// --- MAIN ---

int main() {
    Database db("delivery.db");
    ClienteDAO clienteDAO(db);
    ProdutoDAO produtoDAO(db);

    int opcao = -1;

    while (opcao != 0) {
        Input::limparTela();
        std::cout << "=== DELIVERY CHOPP SYSTEM ===\n";
        std::cout << "1. Clientes\n";
        std::cout << "2. Produtos e Equipamentos\n";
        std::cout << "3. Novo Pedido\n";
        std::cout << "0. Sair\n";
        opcao = Input::lerInteiro("Escolha: ");

        switch (opcao) {
            case 1:
                menuClientes(clienteDAO);
                break;
            case 2:
                menuProdutos(produtoDAO);
                break;
            case 3:
                std::cout << "Em construção\n"; // TODO
                Input::pausar();
                break;
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                std::cout << "Invalido.\n";
                Input::pausar();
        }
    }
    return 0;
}