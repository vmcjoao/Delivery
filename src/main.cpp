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
        Input::limparTela();
        std::cout << "--- DETALHES DA ENTREGA ---\n";
        
        // Formato YYYY-MM-DD para ordenar certo na agenda
        pedidoAtual.dataEntrega = Input::lerString("Data da Entrega (YYYY-MM-DD): ");
        pedidoAtual.dataRecolhimento = Input::lerString("Data do Recolhimento (YYYY-MM-DD): ");
        
        // Taxa opcional
        std::cout << "Deseja cobrar taxa de entrega? (Digite 0 se nao houver)\n";
        pedidoAtual.taxaEntrega = Input::lerDecimal("Valor da Taxa: R$ ");
        
        pedidoAtual.observacao = Input::lerString("Observacoes (ex: Zona Rural, portao azul): ");

        std::cout << "\nTotal Itens: R$ " << pedidoAtual.total;
        std::cout << "\nTaxa:        R$ " << pedidoAtual.taxaEntrega;
        std::cout << "\nTOTAL FINAL: R$ " << pedidoAtual.getTotalComTaxa() << "\n";
        
        std::cout << "\n[1] Confirmar e Salvar  [0] Cancelar\n";
        int confirm = Input::lerInteiro("Opcao: ");
        
        if (confirm == 1) {
            pedDao.criarPedido(pedidoAtual);
        } else {
            std::cout << "Pedido cancelado.\n";
        }
    }
    Input::pausar();
}

void menuLogistica(PedidoDAO& pedDao) {
    int opcao = -1;
    while (opcao != 0) {
        Input::limparTela();
        std::cout << "--- LOGISTICA E FECHAMENTO ---\n";
        
        std::vector<ItemPendente> pendencias = pedDao.listarItensPendentes();

        if (pendencias.empty()) {
            std::cout << "\n>> Tudo recolhido!\n0. Voltar\n";
            Input::lerString(""); 
            return;
        }

        std::cout << "ID  | PEDIDO | PRODUTO               | PENDENTE | PRECO UN.\n";
        std::cout << "--------------------------------------------------------------\n";
        for (auto& p : pendencias) {
            std::cout << std::left << std::setw(4) << p.idItem << "| " 
                      << std::setw(7) << p.idPedido << "| "
                      << std::setw(22) << p.produto.substr(0, 21) << "| "
                      << p.quantidadePendente << "        | R$ " << p.precoUnitario << "\n";
        }
        std::cout << "--------------------------------------------------------------\n";
        
        std::cout << "\n[ID do Item] para dar baixa | [0] Voltar\n";
        opcao = Input::lerInteiro("Opcao: ");

        if (opcao != 0) {
            // Busca dados do item selecionado na lista local
            ItemPendente itemSel;
            bool achou = false;
            for(auto& p : pendencias) if(p.idItem == opcao) { itemSel = p; achou = true; }

            if(achou) {
                int qtd = Input::lerInteiro("Quantidade recolhida agora: ");
                int qtdNaoConsumida = 0;
                
                // Só pergunta se foi consumido se tiver preço
                if (itemSel.precoUnitario > 0) {
                    std::cout << "Dessa quantidade (" << qtd << "), quantos VOLTARAM CHEIOS (não consumidos)?\n";
                    std::cout << "Isso sera abatido da conta.\n";
                    qtdNaoConsumida = Input::lerInteiro("Qtd nao consumida: ");
                }

                std::string msg = pedDao.confirmarRecolhimento(opcao, qtd, qtdNaoConsumida);
                std::cout << "\n>> " << msg << "\n";
                Input::pausar();

                // Verificar se o pedido desse item acabou
                if (pedDao.pedidoEstaCompleto(itemSel.idPedido)) {
                    Input::limparTela();
                    std::cout << "!!! O PEDIDO #" << itemSel.idPedido << " FOI TOTALMENTE RECOLHIDO !!!\n";
                    std::cout << "Deseja realizar o fechamento financeiro agora? (1-Sim, 0-Nao)\n";
                    int fechar = Input::lerInteiro("Opcao: ");
                    
                    if (fechar == 1) {
                        std::cout << "\n--- FECHAMENTO FINANCEIRO ---\n";
                        double desconto = Input::lerDecimal("Desconto em dinheiro (R$): ");
                        std::cout << "Forma de Pagamento (PIX, DINHEIRO, CARTAO): ";
                        std::string forma = Input::lerString("");
                        
                        std::string res = pedDao.fecharPedido(itemSel.idPedido, desconto, forma);
                        std::cout << "\n***********************************\n";
                        std::cout << res << "\n";
                        std::cout << "***********************************\n";
                        Input::pausar();
                    }
                }

            } else {
                std::cout << ">> ID invalido.\n";
                Input::pausar();
            }
        }
    }
}

void menuAgenda(PedidoDAO& dao) {
    Input::limparTela();
    std::cout << "--- AGENDA / CALENDARIO ---\n";
    std::vector<AgendaItem> agenda = dao.listarAgenda();

    if (agenda.empty()) {
        std::cout << "Nenhum evento agendado.\n";
    } else {
        std::cout << "DATA       | TIPO          | CLIENTE          | OBS\n";
        std::cout << "---------------------------------------------------------------\n";
        for (const auto& item : agenda) {
            std::cout << std::left 
                      << std::setw(11) << item.data << "| "
                      << std::setw(14) << item.tipo << "| "
                      << std::setw(17) << item.nomeCliente.substr(0,16) << "| "
                      << item.observacao << "\n";
        }
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
        std::cout << "4. Logistica (Recolhimento)\n";
        std::cout << "5. Ver Agenda (Calendario)\n";
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
            case 4:
                menuLogistica(pedidoDAO);
                break;
            case 5:
                menuAgenda(pedidoDAO);
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