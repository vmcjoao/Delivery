#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include "sqlite3.h"

class Database {
private:
    sqlite3* db; // Ponteiro para a conexão com o banco
    char* zErrMsg = 0; // Para armazenar mensagens de erro do C
    int rc; // Código de retorno das funções

    // Cria as tabelas se não exixtirem
    void setup() {
        const char* sql = 
            // 1. Tabela Clientes
            "CREATE TABLE IF NOT EXISTS clientes ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "telefone TEXT NOT NULL,"
            "endereco TEXT NOT NULL,"
            "data_cadastro DATETIME DEFAULT CURRENT_TIMESTAMP);"
            
            // 2. Tabela Produtos
            "CREATE TABLE IF NOT EXISTS produtos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "preco_unitario REAL NOT NULL,"
            "tipo TEXT NOT NULL);" // BARRIL, EQUIPAMENTO, ETC
            
            // 3. Tabela Pedidos
            "CREATE TABLE IF NOT EXISTS pedidos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "cliente_id INTEGER,"
            "status TEXT NOT NULL,"
            "total_pedido REAL,"
            "FOREIGN KEY(cliente_id) REFERENCES clientes(id));";

        rc = sqlite3_exec(db, sql, nullptr, 0, &zErrMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "Erro ao criar tabelas: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        } else {
            std::cout << "Tabelas verificadas/criadas com sucesso." << std::endl;
        }
    }

public:
    // Construtor abre o banco
    Database(const std::string& db_name) {
        // Tenta abrir o arquivo. Se não existir o SQLite cria
        rc = sqlite3_open(db_name.c_str(), &db);

        if (rc) {
            std::cerr << "Erro ao abrir banco: " << sqlite3_errmsg(db) << std::endl;
            // Lançar uma exceção aqui depois
        } else {
            std::cout << "Banco de dados conectado com sucesso!" << std::endl;
        }
    }

    // Destrutor fecha o banco
    ~Database() {
        sqlite3_close(db);
        std::cout << "Conexão fechada." << std::endl;
    }

    // Método genérico para executar INSERT, UPDATE, DELETE
    bool executarQuery(const std::string& sql) {
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "Erro SQL: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg); // Libera a memória da mensagem de erro
            return false;
        }
        return true;
    }
    
    // Getter para o ponteiro
    sqlite3* getConnection() {
        return db;
    }
};

#endif