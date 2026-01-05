#include <iostream>
#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "Input.hpp"
#include "TipoAtivoDAO.hpp"
#include "ProdutoDAO.hpp"
#include "AtivoDAO.hpp"

// --- FUNÇÃO AUXILIAR: Selecionar um Tipo de Ativo ---
// Retorna o ID do tipo escolhido ou 0 se cancelar
int selecionarTipoAtivo(TipoAtivoDAO& tDao) {
    std::vector<TipoAtivo> tipos = tDao.listarTodos();
    
    if (tipos.empty()) {
        std::cout << ">> Nenhum Tipo de Equipamento cadastrado ainda!\n";
        return 0;
    }

    std::cout << "\n--- TIPOS DISPONIVEIS ---\n";
    for (const auto& t : tipos) {
        std::cout << " [" << t.id << "] " << t.nome << "\n";
    }
    return Input::lerInteiro("\nDigite o ID do Tipo desejado: ");
}

// --- MENU 1: TIPOS DE ATIVO (Definições) ---
void menuTipos(TipoAtivoDAO& tDao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- GESTAO DE TIPOS DE EQUIPAMENTO ---\n";
        std::cout << "1. Cadastrar Novo Tipo (Ex: 'Barril 50L', 'Chopeira')\n";
        std::cout << "2. Listar Tipos\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::string nome = Input::lerString("Nome do Tipo: ");
            TipoAtivo t(nome);
            tDao.inserir(t);
            Input::pausar();
        }
        else if (opcao == 2) {
            std::vector<TipoAtivo> lista = tDao.listarTodos();
            std::cout << "\nLISTA DE TIPOS:\n";
            for(auto& t : lista) {
                std::cout << "#" << t.id << " - " << t.nome << "\n";
            }
            Input::pausar();
        }
    }
}

// --- MENU 2: PRODUTOS (Catálogo de Venda) ---
void menuProdutos(ProdutoDAO& pDao, TipoAtivoDAO& tDao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- CATALOGO DE PRODUTOS (VENDA) ---\n";
        std::cout << "1. Novo Produto (Ex: 'Chopp Pilsen')\n";
        std::cout << "2. Listar Catalogo\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::cout << ">> Passo 1: Defina os dados comerciais\n";
            std::string nome = Input::lerString("Nome do Produto: ");
            double preco = Input::lerDecimal("Preco Base (R$): ");
            
            std::cout << "\n>> Passo 2: Esse produto usa que tipo de equipamento?\n";
            int idTipo = selecionarTipoAtivo(tDao);
            
            if (idTipo != 0) {
                Produto p(nome, preco, idTipo);
                pDao.inserir(p);
                Input::pausar();
            } else {
                Input::pausar();
            }
        }
        else if (opcao == 2) {
            std::vector<Produto> lista = pDao.listarTodos();
            std::cout << "\nCATALOGO:\n";
            std::cout << std::fixed << std::setprecision(2);
            for(auto& p : lista) {
                std::cout << "#" << p.id << " | " << p.nome 
                          << " | R$ " << p.precoBase 
                          << " | Usa Tipo ID: " << p.tipoAtivoId << "\n";
            }
            Input::pausar();
        }
    }
}

// --- MENU 3: ATIVOS (Patrimônio Físico) ---
void menuAtivos(AtivoDAO& aDao, TipoAtivoDAO& tDao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- CONTROLE DE PATRIMONIO (ESTOQUE FISICO) ---\n";
        std::cout << "1. Cadastrar Ativo (Etiquetar Barril/Chopeira)\n";
        std::cout << "2. Listar Patrimonio\n";
        std::cout << "3. Buscar por Codigo\n";
        std::cout << "0. Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao == 1) {
            std::cout << ">> Passo 1: Qual o tipo do equipamento fisico?\n";
            int idTipo = selecionarTipoAtivo(tDao);

            if (idTipo != 0) {
                std::cout << "\n>> Passo 2: Dados da Etiqueta\n";
                std::string serial = Input::lerString("Codigo Serial (Ex: B-045): ");
                std::string obs = Input::lerString("Observacao (Ex: Amassado): ");
                
                Ativo a(serial, idTipo, obs);
                aDao.inserir(a);
                Input::pausar();
            } else {
                Input::pausar();
            }
        }
        else if (opcao == 2) {
            std::vector<AtivoExibicao> lista = aDao.listarTodos();
            std::cout << "\nPATRIMONIO:\n";
            std::cout << "CODIGO      | STATUS       | TIPO DO ATIVO             | OBS\n";
            std::cout << "----------------------------------------------------------------\n";
            for(auto& item : lista) {
                std::cout << std::left 
                          << std::setw(12) << item.ativo.codigoSerial << "| "
                          << std::setw(13) << item.ativo.status << "| "
                          << std::setw(26) << item.nomeTipo.substr(0,25) << "| "
                          << item.ativo.observacao << "\n";
            }
            Input::pausar();
        }
        else if (opcao == 3) {
            std::string busca = Input::lerString("Digite o codigo: ");
            Ativo a = aDao.buscarPorCodigo(busca);
            if (a.id != 0) {
                std::cout << "Encontrado! ID: " << a.id << " Status: " << a.status << "\n";
            } else {
                std::cout << "Nao encontrado.\n";
            }
            Input::pausar();
        }
    }
}

// --- MAIN ---
int main() {
    Database db("delivery.db");
    
    // Instancia todos os DAOs
    TipoAtivoDAO tipoDAO(db);
    ProdutoDAO produtoDAO(db);
    AtivoDAO ativoDAO(db);

    int opcao = -1;

    while (opcao != 0) {
        Input::limparTela();
        std::cout << "=== DELIVERY CHOPP - GESTAO DE ESTOQUE v2.0 ===\n";
        std::cout << "1. Gerenciar TIPOS (Definicoes)\n";
        std::cout << "2. Gerenciar PRODUTOS (Catalogo)\n";
        std::cout << "3. Gerenciar ATIVOS (Estoque Fisico)\n";
        std::cout << "4. Pedidos\n";
        std::cout << "0. Sair\n";
        opcao = Input::lerInteiro("Escolha: ");

        switch (opcao) {
            case 1:
                menuTipos(tipoDAO);
                break;
            case 2:
                menuProdutos(produtoDAO, tipoDAO);
                break;
            case 3:
                menuAtivos(ativoDAO, tipoDAO);
                break;
            case 4:
                std::cout << "Vamos configurar o estoque primeiro!\n";
                Input::pausar();
                break;
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                std::cout << "Opcao invalida.\n";
                Input::pausar();
        }
    }
    return 0;
}