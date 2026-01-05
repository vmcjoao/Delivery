#ifndef TIPOATIVODAO_H
#define TIPOATIVODAO_H

#include <vector>
#include <iostream>
#include "Database.hpp"
#include "TipoAtivo.hpp"

class TipoAtivoDAO {
private:
    sqlite3* db;

public:
    TipoAtivoDAO(Database& database) {
        db = database.getConnection();
    }

    bool inserir(TipoAtivo& t) {
        std::string sql = "INSERT INTO tipos_ativos (nome) VALUES (?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, t.nome.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao criar Tipo: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
        sqlite3_finalize(stmt);
        std::cout << "Tipo '" << t.nome << "' criado!\n";
        return true;
    }

    std::vector<TipoAtivo> listarTodos() {
        std::vector<TipoAtivo> lista;
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, "SELECT id, nome FROM tipos_ativos;", -1, &stmt, nullptr) != SQLITE_OK) return lista;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TipoAtivo t;
            t.id = sqlite3_column_int(stmt, 0);
            t.nome = (const char*)sqlite3_column_text(stmt, 1);
            lista.push_back(t);
        }
        sqlite3_finalize(stmt);
        return lista;
    }
};

#endif