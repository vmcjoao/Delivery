#ifndef ATIVODAO_H
#define ATIVODAO_H

#include <vector>
#include <iostream>
#include "Database.hpp"
#include "Ativo.hpp"

// Struct auxiliar para listagem
struct AtivoExibicao {
    Ativo ativo;
    std::string nomeTipo;
};

class AtivoDAO {
private:
    sqlite3* db;

public:
    AtivoDAO(Database& database) {
        db = database.getConnection();
    }

    // 1. CADASTRAR NOVO ATIVO
    bool inserir(Ativo& a) {
        std::string sql = "INSERT INTO ativos (codigo_serial, tipo_ativo_id, status, observacao_fixa) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro prepare Ativo: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, a.codigoSerial.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, a.tipoAtivoId);
        sqlite3_bind_text(stmt, 3, "DISPONIVEL", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, a.observacao.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao inserir (Codigo ja existe?): " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        std::cout << "Ativo " << a.codigoSerial << " cadastrado!\n";
        return true;
    }

    // 2. LISTAR TODOS
    std::vector<AtivoExibicao> listarTodos() {
        std::vector<AtivoExibicao> lista;
        std::string sql = 
            "SELECT a.id, a.codigo_serial, a.status, a.observacao_fixa, t.nome, t.id "
            "FROM ativos a JOIN tipos_ativos t ON a.tipo_ativo_id = t.id "
            "ORDER BY a.codigo_serial ASC;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return lista;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AtivoExibicao item;
            item.ativo.id = sqlite3_column_int(stmt, 0);
            item.ativo.codigoSerial = (const char*)sqlite3_column_text(stmt, 1);
            item.ativo.status = (const char*)sqlite3_column_text(stmt, 2);
            item.ativo.observacao = (const char*)sqlite3_column_text(stmt, 3) ? (const char*)sqlite3_column_text(stmt, 3) : "";
            
            item.nomeTipo = (const char*)sqlite3_column_text(stmt, 4);
            item.ativo.tipoAtivoId = sqlite3_column_int(stmt, 5);

            lista.push_back(item);
        }
        sqlite3_finalize(stmt);
        return lista;
    }

    // 3. BUSCAR POR CÓDIGO
    Ativo buscarPorCodigo(std::string codigo) {
        Ativo a;
        std::string sql = "SELECT id, tipo_ativo_id, status FROM ativos WHERE codigo_serial = ?;";
        sqlite3_stmt* stmt;
        
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, codigo.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            a.id = sqlite3_column_int(stmt, 0);
            a.tipoAtivoId = sqlite3_column_int(stmt, 1);
            a.status = (const char*)sqlite3_column_text(stmt, 2);
            a.codigoSerial = codigo;
        }
        sqlite3_finalize(stmt);
        return a;
    }
};

#endif