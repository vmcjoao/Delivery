#ifndef PEDIDODAO_H
#define PEDIDODAO_H

#include <vector>
#include "Database.hpp"
#include "Pedido.hpp"

class PedidoDAO {
private:
    sqlite3* db;

public:
    PedidoDAO(Database& database) {
        db = database.getConnection();
    }

    bool criarPedido(Pedido& p) {
        char* zErrMsg = 0;

        // 1. INICIAR TRANSAÇÃO (Trava o banco para escrita segura)
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, 0, &zErrMsg);

        // 2. INSERIR O CABEÇALHO (Tabela pedidos)
        std::string sqlPedido = "INSERT INTO pedidos (cliente_id, status, total_pedido, data_pedido) VALUES (?, ?, ?, datetime('now'));";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sqlPedido.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro ao preparar pedido: " << sqlite3_errmsg(db) << std::endl;
            return false; // Nota: Numa aplicação real, faríamos ROLLBACK aqui
        }

        sqlite3_bind_int(stmt, 1, p.idCliente);
        sqlite3_bind_text(stmt, 2, "ABERTO", -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, p.total);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao salvar cabeçalho: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
        sqlite3_finalize(stmt);

        // 3. PEGAR O ID GERADO AUTOMATICAMENTE
        long long idPedidoGerado = sqlite3_last_insert_rowid(db);

        // 4. INSERIR OS ITENS (Loop)
        std::string sqlItem = "INSERT INTO itens_pedido (pedido_id, produto_id, quantidade, preco_momento) VALUES (?, ?, ?, ?);";
        
        for (const auto& item : p.itens) {
            sqlite3_stmt* stmtItem;
            sqlite3_prepare_v2(db, sqlItem.c_str(), -1, &stmtItem, nullptr);

            sqlite3_bind_int64(stmtItem, 1, idPedidoGerado);
            sqlite3_bind_int(stmtItem, 2, item.produto.id);
            sqlite3_bind_int(stmtItem, 3, item.quantidade);
            sqlite3_bind_double(stmtItem, 4, item.precoCobrado);

            if (sqlite3_step(stmtItem) != SQLITE_DONE) {
                std::cerr << "Erro ao salvar item. Abortando..." << std::endl;
                sqlite3_finalize(stmtItem);
                // SE DEU ERRO, DESFAZ TUDO!
                sqlite3_exec(db, "ROLLBACK;", nullptr, 0, nullptr); 
                return false;
            }
            sqlite3_finalize(stmtItem);
        }

        // 5. CONFIRMAR TRANSAÇÃO (Grava tudo no disco permanentemente)
        sqlite3_exec(db, "COMMIT;", nullptr, 0, nullptr);
        
        std::cout << "Pedido #" << idPedidoGerado << " realizado com sucesso!" << std::endl;
        return true;
    }
};

#endif