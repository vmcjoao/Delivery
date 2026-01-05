#ifndef PEDIDODAO_H
#define PEDIDODAO_H

#include <vector>
#include <iostream>
#include "Database.hpp"
#include "Pedido.hpp"
#include "Cliente.hpp"

// Struct para listagem resumida
struct PedidoResumo {
    int id;
    std::string clienteNome;
    std::string dataEntrega;
    std::string status;
    double total;
};

class PedidoDAO {
private:
    sqlite3* db;

public:
    PedidoDAO(Database& database) {
        db = database.getConnection();
    }

    // 1. CRIAR PEDIDO
    bool criarPedido(Pedido& p) {
        char* zErrMsg = 0;
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, 0, &zErrMsg);

        std::string sql = 
            "INSERT INTO pedidos (cliente_id, data_entrega, data_recolhimento, nr_pessoas, "
            "observacao_entrega, status, total_previsto, desconto, forma_pagamento, status_pagamento) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
        
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro prepare Pedido: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, p.idCliente);
        sqlite3_bind_text(stmt, 2, p.dataEntrega.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, p.dataRecolhimento.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, p.nrPessoas);
        sqlite3_bind_text(stmt, 5, p.observacao.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, "ABERTO", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 7, p.totalPrevisto);
        sqlite3_bind_double(stmt, 8, p.desconto);
        sqlite3_bind_text(stmt, 9, p.formaPagamento.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 10, "PENDENTE", -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro step Pedido: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, 0, nullptr);
            return false;
        }
        sqlite3_finalize(stmt);

        long long idPedido = sqlite3_last_insert_rowid(db);

        std::string sqlItem = "INSERT INTO itens_pedido (pedido_id, produto_id, quantidade_solicitada, preco_unitario_momento) VALUES (?, ?, ?, ?);";
        
        for (auto& item : p.itens) {
            sqlite3_stmt* stmtItem;
            sqlite3_prepare_v2(db, sqlItem.c_str(), -1, &stmtItem, nullptr);
            sqlite3_bind_int64(stmtItem, 1, idPedido);
            sqlite3_bind_int(stmtItem, 2, item.produto.id);
            sqlite3_bind_int(stmtItem, 3, item.quantidade);
            sqlite3_bind_double(stmtItem, 4, item.precoNoMomento);
            
            if (sqlite3_step(stmtItem) != SQLITE_DONE) {
                sqlite3_finalize(stmtItem);
                sqlite3_exec(db, "ROLLBACK;", nullptr, 0, nullptr);
                return false;
            }
            sqlite3_finalize(stmtItem);
        }

        sqlite3_exec(db, "COMMIT;", nullptr, 0, nullptr);
        std::cout << "Pedido #" << idPedido << " criado com sucesso!\n";
        return true;
    }

    // 2. LISTAR PEDIDOS ABERTOS
    std::vector<PedidoResumo> listarAbertos() {
        std::vector<PedidoResumo> lista;
        std::string sql = 
            "SELECT p.id, c.nome, p.data_entrega, p.status, p.total_previsto "
            "FROM pedidos p JOIN clientes c ON p.cliente_id = c.id "
            "WHERE p.status != 'FINALIZADO' "
            "ORDER BY p.data_entrega ASC;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            PedidoResumo r;
            r.id = sqlite3_column_int(stmt, 0);
            r.clienteNome = (const char*)sqlite3_column_text(stmt, 1);
            r.dataEntrega = (const char*)sqlite3_column_text(stmt, 2);
            r.status = (const char*)sqlite3_column_text(stmt, 3);
            r.total = sqlite3_column_double(stmt, 4);
            lista.push_back(r);
        }
        sqlite3_finalize(stmt);
        return lista;
    }
};

#endif