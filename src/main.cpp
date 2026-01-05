#include <iostream>
#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "Input.hpp"
#include "TipoAtivoDAO.hpp"
#include "ProdutoDAO.hpp"
#include "AtivoDAO.hpp"
#include "ClienteDAO.hpp"
#include "PedidoDAO.hpp"

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

// --- TIPOS DE ATIVO (Definições) ---
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

// --- PRODUTOS (Catálogo de Venda) ---
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

// --- ATIVOS (Patrimônio Físico) ---
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

// --- NOVO PEDIDO ---
void menuNovoPedido(ClienteDAO& cliDao, ProdutoDAO& prodDao, PedidoDAO& pedDao) {
    Input::limparTela();
    std::cout << "--- NOVO PEDIDO ---\n";

    // 1. Selecionar Cliente
    std::vector<Cliente> clientes = cliDao.listarTodos();
    if (clientes.empty()) {
        std::cout << ">> Nenhum cliente cadastrado! Use a opcao 5 no menu principal.\n";
        Input::pausar();
        return;
    }
    
    std::cout << "Selecione o Cliente:\n";
    for(auto& c : clientes) std::cout << " [" << c.id << "] " << c.nome << "\n";
    int idCli = Input::lerInteiro("ID do Cliente: ");

    // 2. Dados Logísticos
    Input::limparTela();
    std::cout << "--- DADOS LOGISTICOS ---\n";
    std::string entrega = Input::lerString("Data Entrega (YYYY-MM-DD HH:MM): ");
    std::string recolha = Input::lerString("Data Recolha (YYYY-MM-DD HH:MM): ");
    int pessoas = Input::lerInteiro("Numero de Pessoas: ");
    std::string obs = Input::lerString("Observacoes: ");

    Pedido pedido;
    pedido.idCliente = idCli;
    pedido.dataEntrega = entrega;
    pedido.dataRecolhimento = recolha;
    pedido.nrPessoas = pessoas;
    pedido.observacao = obs;

    // 3. Adicionar Produtos
    bool adicionando = true;
    std::vector<Produto> produtos = prodDao.listarTodos();

    while (adicionando) {
        Input::limparTela();
        std::cout << "--- CARRINHO DE COMPRAS ---\n";
        std::cout << "Total Previsto: R$ " << pedido.totalPrevisto << "\n\n";

        for(auto& p : produtos) {
            std::cout << " [" << p.id << "] " << p.nome << " (R$ " << p.precoBase << ")\n";
        }
        std::cout << " [0] Finalizar Selecao\n";

        int idProd = Input::lerInteiro("\nEscolha o Produto: ");
        if (idProd == 0) {
            adicionando = false;
        } else {
            bool achou = false;
            for(auto& p : produtos) {
                if(p.id == idProd) {
                    int qtd = Input::lerInteiro("Quantidade: ");
                    pedido.adicionarItem(p, qtd);
                    achou = true;
                    break;
                }
            }
            if(!achou) std::cout << "Produto invalido!\n";
        }
    }

    if (pedido.itens.empty()) {
        std::cout << "Pedido vazio cancelado.\n";
        Input::pausar();
        return;
    }

    // 4. Salvar
    Input::limparTela();
    std::cout << "Confirmar pedido? (1-Sim, 0-Nao): ";
    if (Input::lerInteiro("") == 1) {
        pedDao.criarPedido(pedido);
    }
    Input::pausar();
}

void menuListarPedidos(PedidoDAO& dao) {
    Input::limparTela();
    std::cout << "--- AGENDA DE PEDIDOS ---\n";
    auto lista = dao.listarAbertos();
    std::cout << "ID  | DATA             | CLIENTE          | TOTAL\n";
    for(auto& p : lista) {
        std::cout << std::left << std::setw(4) << p.id << "| "
                  << std::setw(17) << p.dataEntrega << "| "
                  << std::setw(17) << p.clienteNome.substr(0,16) << "| R$ " << p.total << "\n";
    }
    Input::pausar();
}

// --- MAIN ---
int main() {
    Database db("delivery.db");
    
    ClienteDAO clienteDAO(db);
    TipoAtivoDAO tipoDAO(db);
    ProdutoDAO produtoDAO(db);
    AtivoDAO ativoDAO(db);
    PedidoDAO pedidoDAO(db);

    int opcao = -1;

    while (opcao != 0) {
        Input::limparTela();
        std::cout << "=== DELIVERY CHOPP SYSTEM ===\n";
        std::cout << "1. Configuracoes (Tipos/Produtos/Ativos)\n";
        std::cout << "2. Novo Pedido\n";
        std::cout << "3. Ver Agenda de Pedidos\n";
        std::cout << "5. Cadastrar Cliente Rapido\n";
        std::cout << "0. Sair\n";
        opcao = Input::lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: 
                std::cout << "\n[1] Tipos [2] Produtos [3] Ativos: ";
                int sub; std::cin >> sub; std::cin.ignore();
                if(sub==1) menuTipos(tipoDAO);
                else if(sub==2) menuProdutos(produtoDAO, tipoDAO);
                else if(sub==3) menuAtivos(ativoDAO, tipoDAO);
                break;
            case 2: menuNovoPedido(clienteDAO, produtoDAO, pedidoDAO); break;
            case 3: menuListarPedidos(pedidoDAO); break;
            case 5: {
                Cliente c(Input::lerString("Nome: "), Input::lerString("Tel: "), Input::lerString("End: "));
                clienteDAO.inserir(c);
                break;
            }
            case 0: std::cout << "Saindo...\n"; break;
            default: Input::pausar();
        }
    }
    return 0;
}