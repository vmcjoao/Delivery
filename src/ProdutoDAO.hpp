#ifndef PRODUTODAO_H
#define PRODUTODAO_H

#include <vector>
#include <iostream>
#include "Database.hpp"
#include "Produto.hpp"

class ProdutoDAO {
private:
    sqlite3* db;

public:
    ProdutoDAO(Database& database) {
        db = database.getConnection();
    }

    bool inserir(Produto& p) {
        std::string sql = "INSERT INTO produtos (nome, tipo, preco_unitario) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro no prepare: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // Bind dos valores
        sqlite3_bind_text(stmt, 1, p.nome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p.tipo.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, p.preco_unitario); // Bind específico para double

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao inserir: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        std::cout << "Produto cadastrado com sucesso!" << std::endl;
        return true;
    }

    std::vector<Produto> listarTodos() {
        std::vector<Produto> lista;
        std::string sql = "SELECT id, nome, tipo, preco_unitario FROM produtos;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Produto p;
            p.id = sqlite3_column_int(stmt, 0);
            p.nome = (const char*)sqlite3_column_text(stmt, 1);
            p.tipo = (const char*)sqlite3_column_text(stmt, 2);
            p.preco_unitario = sqlite3_column_double(stmt, 3);

            lista.push_back(p);
        }

        sqlite3_finalize(stmt);
        return lista;
    }
};

#endif