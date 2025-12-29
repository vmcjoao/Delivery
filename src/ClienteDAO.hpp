#ifndef CLIENTEDAO_H
#define CLIENTEDAO_H

#include <vector>
#include "Database.hpp"
#include "Cliente.hpp"

class ClienteDAO {
private:
    sqlite3* db;

public:
    ClienteDAO(Database& database) {
        db = database.getConnection();
    }

    // 1. INSERIR
    bool inserir(Cliente& c) {
        std::string sql = "INSERT INTO clientes (nome, telefone, endereco) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt; // Statement

        // A. Prepara
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Erro ao preparar INSERT: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        // B. Bind (Vincula)
        // Parâmetros: (statement, indice_do_interrogacao, valor, tamanho, destrutor)
        sqlite3_bind_text(stmt, 1, c.nome.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, c.telefone.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, c.endereco.c_str(), -1, SQLITE_STATIC);

        // C. Executa
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Erro ao inserir: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt); // Limpa memória
            return false;
        }

        // D. Limpa
        sqlite3_finalize(stmt);
        std::cout << "Cliente salvo com sucesso!" << std::endl;
        return true;
    }

    // 2. LISTAR TODOS (Read)
    std::vector<Cliente> listarTodos() {
        std::vector<Cliente> lista;
        std::string sql = "SELECT id, nome, telefone, endereco FROM clientes;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return lista; // Retorna vazio se der erro
        }

        //Enquanto houver ROWs para ler
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Cliente c;
            // Dados coluna por coluna (0, 1, 2, 3)
            c.id = sqlite3_column_int(stmt, 0);
            
            c.nome = (const char*)sqlite3_column_text(stmt, 1);
            c.telefone = (const char*)sqlite3_column_text(stmt, 2);
            c.endereco = (const char*)sqlite3_column_text(stmt, 3);

            lista.push_back(c);
        }

        sqlite3_finalize(stmt);
        return lista;
    }
};

#endif