#include <iostream>
#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "ProdutoDAO.hpp"
#include "AtivoDAO.hpp"
#include "Input.hpp"

// --- MENU 1: GESTÃO DE CATÁLOGO (Tipos de Produto) ---
void menuProdutos(ProdutoDAO& dao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- CATALOGO DE PRODUTOS (TIPOS) ---\n";
        std::cout << "1. Novo Tipo de Produto\n";
        std::cout << "2. Listar Catalogo\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::cout << "\nExemplos: 'Chopp Pilsen 50L', 'Chopeira Eletrica 110v'\n";
            std::string nome = Input::lerString("Nome do Produto: ");
            
            std::cout << "Tipos validos: [BARRIL] [CHOPEIRA] [CILINDRO] [OUTROS]\n";
            std::string tipo = Input::lerString("Categoria/Tipo: ");
            
            double preco = Input::lerDecimal("Preco Base de Venda/Aluguel (R$): ");
            
            Produto p(nome, tipo, preco);
            if (dao.inserir(p)) {
                Input::pausar();
            }
        }
        else if (opcao == 2) {
            std::vector<Produto> lista = dao.listarTodos();
            std::cout << "\n--- LISTA DE TIPOS ---\n";
            std::cout << "ID | TIPO         | NOME                          | PRECO BASE\n";
            std::cout << "------------------------------------------------------------\n";
            std::cout << std::fixed << std::setprecision(2); 
            
            for (auto& p : lista) {
                std::cout << std::left 
                          << std::setw(3) << p.id << "| "
                          << std::setw(13) << p.tipo << "| "
                          << std::setw(30) << p.nome.substr(0,29) << "| R$ " 
                          << p.precoBase << "\n";
            }
            Input::pausar();
        }
    }
}

// --- MENU 2: GESTÃO DE PATRIMÔNIO (Estoque Físico) ---
void menuAtivos(AtivoDAO& ativoDao, ProdutoDAO& prodDao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- ESTOQUE FISICO (ATIVOS) ---\n";
        std::cout << "1. Cadastrar Novo Ativo (Etiquetar)\n";
        std::cout << "2. Listar Todos os Ativos\n";
        std::cout << "3. Buscar Ativo por Codigo\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            // Passo 1: Escolher o TIPO
            Input::limparTela();
            std::cout << "--- PASSO 1: QUAL O TIPO DO ATIVO? ---\n";
            std::vector<Produto> produtos = prodDao.listarTodos();
            
            if (produtos.empty()) {
                std::cout << ">> Voce precisa cadastrar Produtos no Catalogo antes!\n";
                Input::pausar();
                continue;
            }

            for (auto& p : produtos) {
                std::cout << " [" << p.id << "] " << p.nome << " (" << p.tipo << ")\n";
            }
            int idProd = Input::lerInteiro("\nDigite o ID do Tipo de Produto: ");
            
            // Passo 2: Dados do Físico
            std::cout << "\n--- PASSO 2: DADOS DO ATIVO ---\n";
            std::string serial = Input::lerString("Codigo Serial (Ex: B-045, CH-01): ");
            std::string obs = Input::lerString("Observacao (Ex: Risco na lateral): ");

            Ativo novo(serial, idProd, obs);
            ativoDao.inserir(novo);
            Input::pausar();
        }
        else if (opcao == 2) {
            std::vector<AtivoExibicao> lista = ativoDao.listarTodos();
            std::cout << "\n--- LISTA DE PATRIMONIO ---\n";
            std::cout << "CODIGO      | STATUS       | TIPO (PRODUTO)            | OBS\n";
            std::cout << "-------------------------------------------------------------------\n";
            
            for (auto& item : lista) {
                std::cout << std::left 
                          << std::setw(12) << item.ativo.codigoSerial << "| "
                          << std::setw(13) << item.ativo.status << "| "
                          << std::setw(26) << item.nomeProduto.substr(0,25) << "| " 
                          << item.ativo.observacao << "\n";
            }
            std::cout << "-------------------------------------------------------------------\n";
            std::cout << "Total de ativos: " << lista.size() << "\n";
            Input::pausar();
        }
        else if (opcao == 3) {
            std::string busca = Input::lerString("Digite o codigo (ex: B-045): ");
            Ativo a = ativoDao.buscarPorCodigo(busca);
            
            if (a.id != 0) {
                std::cout << "\n>> ENCONTRADO:\n";
                std::cout << "ID Interno: " << a.id << "\n";
                std::cout << "Serial:     " << a.codigoSerial << "\n";
                std::cout << "Status:     " << a.status << "\n";
                std::cout << "ID Tipo:    " << a.produtoId << "\n";
            } else {
                std::cout << ">> Nao encontrado.\n";
            }
            Input::pausar();
        }
    }
}

// --- MAIN ---
int main() {
    Database db("delivery.db");
    
    // Instanciando os DAOs necessários para Estoque
    ProdutoDAO produtoDAO(db);
    AtivoDAO ativoDAO(db);

    int opcao = -1;

    while (opcao != 0) {
        Input::limparTela();
        std::cout << "=== SISTEMA DE GESTAO DE PATRIMONIO v2.0 ===\n";
        std::cout << "1. Catalogo de Produtos (O que vendemos)\n";
        std::cout << "2. Estoque Fisico / Ativos (O que temos)\n";
        std::cout << "3. Pedidos (EM BREVE - FASE 3)\n";
        std::cout << "0. Sair\n";
        opcao = Input::lerInteiro("Escolha: ");

        switch (opcao) {
            case 1:
                menuProdutos(produtoDAO);
                break;
            case 2:
                menuAtivos(ativoDAO, produtoDAO);
                break;
            case 3:
                std::cout << "Primeiro precisamos cadastrar o estoque!\n";
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