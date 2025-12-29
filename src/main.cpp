#include <iostream>
#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "ClienteDAO.hpp"
#include "ProdutoDAO.hpp"
#include "PedidoDAO.hpp"
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

void menuNovoPedido(ClienteDAO& cDao, ProdutoDAO& pDao, PedidoDAO& pedDao) {
    Input::limparTela();
    std::cout << "--- NOVO PEDIDO ---\n";

    // 1. Identificar o Cliente
    // (Num sistema real, buscaríamos por nome, aqui vamos listar todos pra facilitar)
    std::cout << "Selecione o Cliente pelo ID:\n";
    std::vector<Cliente> clientes = cDao.listarTodos();
    for(auto& c : clientes) std::cout << " [" << c.id << "] " << c.nome << "\n";
    
    int idCli = Input::lerInteiro("\nDigite o ID do Cliente: ");
    
    // Validar se cliente existe (simplificado)
    bool clienteExiste = false;
    for(auto& c : clientes) if(c.id == idCli) clienteExiste = true;
    
    if (!clienteExiste) {
        std::cout << "Cliente nao encontrado!\n";
        Input::pausar();
        return;
    }

    // 2. Criar objeto Pedido
    Pedido pedidoAtual;
    pedidoAtual.idCliente = idCli;

    // 3. Loop de Produtos
    bool adicionando = true;
    std::vector<Produto> produtos = pDao.listarTodos();

    while (adicionando) {
        Input::limparTela();
        std::cout << "--- ADICIONANDO ITENS AO PEDIDO ---\n";
        std::cout << "Cliente ID: " << idCli << " | Total Atual: R$ " << pedidoAtual.total << "\n\n";
        
        std::cout << "Produtos Disponiveis:\n";
        for(auto& p : produtos) {
            std::cout << " [" << p.id << "] " << p.nome << " (R$ " << p.preco_unitario << ")\n";
        }
        std::cout << " [0] Encerrar e Salvar Pedido\n";

        int idProd = Input::lerInteiro("\nEscolha o produto (ID): ");

        if (idProd == 0) {
            adicionando = false;
        } else {
            // Busca o produto na lista local
            Produto prodSelecionado;
            bool achou = false;
            for(auto& p : produtos) {
                if(p.id == idProd) {
                    prodSelecionado = p;
                    achou = true;
                    break;
                }
            }

            if(achou) {
                int qtd = Input::lerInteiro("Quantidade: ");
                pedidoAtual.adicionarItem(prodSelecionado, qtd);
                std::cout << ">> Item adicionado!\nAperte ENTER para continuar\n";
                // Pequeno delay pra ver a mensagem
                std::cin.ignore(); 
            } else {
                std::cout << ">> Produto invalido.\nAperte ENTER para continuar\n";
                std::cin.ignore();
            }
        }
    }

    // 4. Salvar
    if (pedidoAtual.itens.empty()) {
        std::cout << "Pedido vazio cancelado.\n";
    } else {
        std::cout << "\nFinalizando pedido... Total: R$ " << pedidoAtual.total << "\n";
        pedDao.criarPedido(pedidoAtual);
    }
    Input::pausar();
}

// --- MAIN ---

int main() {
    Database db("delivery.db");
    ClienteDAO clienteDAO(db);
    ProdutoDAO produtoDAO(db);
    PedidoDAO pedidoDAO(db);

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
                menuNovoPedido(clienteDAO, produtoDAO, pedidoDAO);
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