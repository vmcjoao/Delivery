#ifndef PEDIDODAO_H
#define PEDIDODAO_H

#include <vector>
#include <iomanip>
#include "Database.hpp"
#include "Pedido.hpp"

// Struct auxiliar para a Agenda
struct AgendaItem {
    int idPedido;
    std::string data;
    std::string tipo; // "ENTREGA" ou "RECOLHIMENTO"
    std::string nomeCliente;
    std::string observacao;
};

// Struct auxiliar para itens pendentes
struct ItemPendente {
    int idItem;
    int idPedido;
    std::string cliente;
    std::string produto;
    double precoUnitario;
    int quantidadeOriginal;
    int quantidadePendente;
};

class PedidoDAO {
private:
    sqlite3* db;

public:
    PedidoDAO(Database& database) {
        db = database.getConnection();
    }

    // CRIAR PEDIDO
    bool criarPedido(Pedido& p) {
        char* zErrMsg = 0;
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, 0, &zErrMsg);

        std::string sqlPedido = "INSERT INTO pedidos (cliente_id, status, total_pedido, data_entrega, data_recolhimento, observacao, taxa_entrega) VALUES (?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sqlPedido.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro prepare pedido: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, p.idCliente);
        sqlite3_bind_text(stmt, 2, "ABERTO", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, p.getTotalComTaxa());
        sqlite3_bind_text(stmt, 4, p.dataEntrega.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, p.dataRecolhimento.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, p.observacao.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 7, p.taxaEntrega);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro step pedido: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
        sqlite3_finalize(stmt);

        long long idPedidoGerado = sqlite3_last_insert_rowid(db);

        std::string sqlItem = "INSERT INTO itens_pedido (pedido_id, produto_id, quantidade, preco_momento) VALUES (?, ?, ?, ?);";
        for (const auto& item : p.itens) {
            sqlite3_stmt* stmtItem;
            sqlite3_prepare_v2(db, sqlItem.c_str(), -1, &stmtItem, nullptr);
            sqlite3_bind_int64(stmtItem, 1, idPedidoGerado);
            sqlite3_bind_int(stmtItem, 2, item.produto.id);
            sqlite3_bind_int(stmtItem, 3, item.quantidade);
            sqlite3_bind_double(stmtItem, 4, item.precoCobrado);
            if (sqlite3_step(stmtItem) != SQLITE_DONE) {
                sqlite3_finalize(stmtItem);
                sqlite3_exec(db, "ROLLBACK;", nullptr, 0, nullptr); 
                return false;
            }
            sqlite3_finalize(stmtItem);
        }

        sqlite3_exec(db, "COMMIT;", nullptr, 0, nullptr);
        std::cout << "Pedido #" << idPedidoGerado << " salvo! Total c/ taxa: R$ " << p.getTotalComTaxa() << std::endl;
        return true;
    }

    // AGENDA
    std::vector<AgendaItem> listarAgenda() {
        std::vector<AgendaItem> agenda;
        
        // SQL Híbrido: Pega entregas E recolhimentos e ordena por data
        // UNION ALL para juntar duas consultas em uma lista só
        std::string sql = 
            "SELECT p.id, p.data_entrega as data, 'ENTREGA' as tipo, c.nome, p.observacao "
            "FROM pedidos p JOIN clientes c ON p.cliente_id = c.id WHERE p.status != 'FINALIZADO' "
            "UNION ALL "
            "SELECT p.id, p.data_recolhimento as data, 'RECOLHIMENTO' as tipo, c.nome, p.observacao "
            "FROM pedidos p JOIN clientes c ON p.cliente_id = c.id WHERE p.status != 'FINALIZADO' "
            "ORDER BY data ASC;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return agenda;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AgendaItem item;
            item.idPedido = sqlite3_column_int(stmt, 0);
            const unsigned char* dataText = sqlite3_column_text(stmt, 1);
            item.data = dataText ? (const char*)dataText : "Sem Data";
            item.tipo = (const char*)sqlite3_column_text(stmt, 2);
            item.nomeCliente = (const char*)sqlite3_column_text(stmt, 3);
            const unsigned char* obsText = sqlite3_column_text(stmt, 4);
            item.observacao = obsText ? (const char*)obsText : "";
            agenda.push_back(item);
        }
        sqlite3_finalize(stmt);
        return agenda;
    }

    // PENDÊNCIAS
    std::vector<ItemPendente> listarItensPendentes() {
        std::vector<ItemPendente> lista;
        std::string sql = 
            "SELECT i.id, i.pedido_id, c.nome, p.nome, i.preco_momento, i.quantidade, (i.quantidade - i.qtd_devolvida) "
            "FROM itens_pedido i "
            "JOIN pedidos ped ON i.pedido_id = ped.id "
            "JOIN clientes c ON ped.cliente_id = c.id "
            "JOIN produtos p ON i.produto_id = p.id "
            "WHERE i.qtd_devolvida < i.quantidade AND p.tipo != 'CONSUMIVEL';"; // Filtra consumíveis se necessário

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ItemPendente item;
            item.idItem = sqlite3_column_int(stmt, 0);
            item.idPedido = sqlite3_column_int(stmt, 1);
            item.cliente = (const char*)sqlite3_column_text(stmt, 2);
            item.produto = (const char*)sqlite3_column_text(stmt, 3);
            item.precoUnitario = sqlite3_column_double(stmt, 4);
            item.quantidadeOriginal = sqlite3_column_int(stmt, 5);
            item.quantidadePendente = sqlite3_column_int(stmt, 6);
            lista.push_back(item);
        }
        sqlite3_finalize(stmt);
        return lista;
    }

    // 4. BAIXA COM CONSUMO
    std::string confirmarRecolhimento(int idItem, int qtdRecolhida, int qtdNaoConsumida) {
        if (qtdNaoConsumida > qtdRecolhida) return "Erro: Nao consumidos > Recolhidos!";

        std::string sqlUp = "UPDATE itens_pedido SET qtd_devolvida = qtd_devolvida + ?, qtd_nao_consumida = qtd_nao_consumida + ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sqlUp.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, qtdRecolhida);
        sqlite3_bind_int(stmt, 2, qtdNaoConsumida);
        sqlite3_bind_int(stmt, 3, idItem);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return "Erro SQL ao atualizar item.";
        }
        sqlite3_finalize(stmt);
        return "Item atualizado com sucesso!";
    }

    // 5. FECHAMENTO FINANCEIRO
    std::string fecharPedido(int idPedido, double descontoDinheiro, std::string formaPagamento) {
        // A. Calcular Total Original + Taxa
        std::string sqlTotais = "SELECT total_pedido, taxa_entrega FROM pedidos WHERE id = ?;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sqlTotais.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, idPedido);
        
        if (sqlite3_step(stmt) != SQLITE_ROW) { sqlite3_finalize(stmt); return "Pedido nao encontrado."; }
        double totalOriginal = sqlite3_column_double(stmt, 0);
        double taxa = sqlite3_column_double(stmt, 1);
        sqlite3_finalize(stmt);

        // B. Calcular valor a abater (Itens não consumidos)
        std::string sqlAbatimento = "SELECT SUM(qtd_nao_consumida * preco_momento) FROM itens_pedido WHERE pedido_id = ?;";
        sqlite3_prepare_v2(db, sqlAbatimento.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, idPedido);
        double abatimentoProdutos = 0.0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            abatimentoProdutos = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);

        // C. Calcular Final
        double valorFinal = (totalOriginal + taxa) - abatimentoProdutos - descontoDinheiro;
        if (valorFinal < 0) valorFinal = 0; // Segurança

        // D. Atualizar Pedido
        std::string sqlFinal = "UPDATE pedidos SET status = 'FINALIZADO', status_pagamento = 'PAGO', forma_pagamento = ?, desconto = ?, valor_final = ? WHERE id = ?;";
        sqlite3_prepare_v2(db, sqlFinal.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, formaPagamento.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, descontoDinheiro);
        sqlite3_bind_double(stmt, 3, valorFinal);
        sqlite3_bind_int(stmt, 4, idPedido);
        
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return "Conta fechada! Valor final recebido: R$ " + std::to_string(valorFinal);
    }
    
    // Auxiliar: Verificar se o pedido já tem todos itens devolvidos
    bool pedidoEstaCompleto(int idPedido) {
        // Se existir algum item onde qtd_devolvida < quantidade, retorna falso
        std::string sql = "SELECT COUNT(*) FROM itens_pedido WHERE pedido_id = ? AND qtd_devolvida < quantidade;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, idPedido);
        int pendentes = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) pendentes = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (pendentes == 0);
    }
};

#endif