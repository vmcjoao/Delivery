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
        std::string sql = "INSERT INTO produtos (nome, tipo, preco_base) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro no prepare Produto: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, p.nome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, p.tipo.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, p.precoBase);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao inserir Produto: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        std::cout << "Produto do catalogo salvo com sucesso!" << std::endl;
        return true;
    }

    std::vector<Produto> listarTodos() {
        std::vector<Produto> lista;
        std::string sql = "SELECT id, nome, tipo, preco_base FROM produtos;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Produto p;
            p.id = sqlite3_column_int(stmt, 0);
            p.nome = (const char*)sqlite3_column_text(stmt, 1);
            p.tipo = (const char*)sqlite3_column_text(stmt, 2);
            p.precoBase = sqlite3_column_double(stmt, 3);

            lista.push_back(p);
        }

        sqlite3_finalize(stmt);
        return lista;
    }
    
    // Método auxiliar para buscar um produto específico
    Produto buscarPorId(int id) {
        Produto p;
        p.id = 0; // 0 significa não encontrado
        std::string sql = "SELECT id, nome, tipo, preco_base FROM produtos WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, id);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                p.id = sqlite3_column_int(stmt, 0);
                p.nome = (const char*)sqlite3_column_text(stmt, 1);
                p.tipo = (const char*)sqlite3_column_text(stmt, 2);
                p.precoBase = sqlite3_column_double(stmt, 3);
            }
            sqlite3_finalize(stmt);
        }
        return p;
    }
};

#endif