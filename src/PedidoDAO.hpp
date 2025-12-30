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
    std::string cliente;
    std::string endereco;
    std::string produto;
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
            "SELECT i.id, c.nome, c.endereco, p.nome, i.quantidade, (i.quantidade - i.qtd_devolvida) "
            "FROM itens_pedido i "
            "JOIN pedidos ped ON i.pedido_id = ped.id "
            "JOIN clientes c ON ped.cliente_id = c.id "
            "JOIN produtos p ON i.produto_id = p.id "
            "WHERE i.qtd_devolvida < i.quantidade AND p.tipo != 'CONSUMIVEL';";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ItemPendente item;
            item.idItem = sqlite3_column_int(stmt, 0);
            item.cliente = (const char*)sqlite3_column_text(stmt, 1);
            item.endereco = (const char*)sqlite3_column_text(stmt, 2);
            item.produto = (const char*)sqlite3_column_text(stmt, 3);
            item.quantidadeOriginal = sqlite3_column_int(stmt, 4);
            item.quantidadePendente = sqlite3_column_int(stmt, 5);
            lista.push_back(item);
        }
        sqlite3_finalize(stmt);
        return lista;
    }

    // RECOLHIMENTO PARCIAL
    std::string confirmarRecolhimento(int idItem, int qtdRecolhida) {
        // Primeiro: Verificar quantos faltam para não devolver mais do que deve
        std::string sqlCheck = "SELECT quantidade, qtd_devolvida FROM itens_pedido WHERE id = ?;";
        sqlite3_stmt* stmtCheck;
        sqlite3_prepare_v2(db, sqlCheck.c_str(), -1, &stmtCheck, nullptr);
        sqlite3_bind_int(stmtCheck, 1, idItem);
        
        if (sqlite3_step(stmtCheck) != SQLITE_ROW) {
            sqlite3_finalize(stmtCheck);
            return "Erro: Item nao encontrado.";
        }
        
        int total = sqlite3_column_int(stmtCheck, 0);
        int jaDevolvido = sqlite3_column_int(stmtCheck, 1);
        int pendente = total - jaDevolvido;
        sqlite3_finalize(stmtCheck);

        if (qtdRecolhida > pendente) {
            return "Erro: Voce tentou devolver mais do que o pendente!";
        }

        // Segundo: Atualizar somando
        std::string sqlUpdate = "UPDATE itens_pedido SET qtd_devolvida = qtd_devolvida + ? WHERE id = ?;";
        sqlite3_stmt* stmtUp;
        sqlite3_prepare_v2(db, sqlUpdate.c_str(), -1, &stmtUp, nullptr);
        
        sqlite3_bind_int(stmtUp, 1, qtdRecolhida);
        sqlite3_bind_int(stmtUp, 2, idItem);

        bool sucesso = (sqlite3_step(stmtUp) == SQLITE_DONE);
        sqlite3_finalize(stmtUp);

        if (sucesso) {
            // Verifica se finalizou tudo deste item
            if (qtdRecolhida == pendente) return "Item quitado completamente!";
            else return "Baixa parcial realizada. Ainda restam itens.";
        }
        return "Erro ao atualizar banco.";
    }
};

#endif