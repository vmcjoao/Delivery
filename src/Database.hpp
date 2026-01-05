#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include "sqlite3.h"

class Database {
private:
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    void criarTabela(const std::string& nomeTabela, const std::string& sql) {
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Erro ao criar " << nomeTabela << ": " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }
    }

    void setup() {
        // 1. Clientes
        criarTabela("CLIENTES", 
            "CREATE TABLE IF NOT EXISTS clientes ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "telefone TEXT NOT NULL,"
            "endereco TEXT NOT NULL,"
            "data_cadastro DATETIME DEFAULT CURRENT_TIMESTAMP);"
        );

        // 2. Tipos de Ativos
        criarTabela("TIPOS_ATIVOS", 
            "CREATE TABLE IF NOT EXISTS tipos_ativos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL UNIQUE);" 
        );

        // 3. Produtos
        criarTabela("PRODUTOS", 
            "CREATE TABLE IF NOT EXISTS produtos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "preco_base REAL NOT NULL,"
            "tipo_ativo_id INTEGER,"        
            "FOREIGN KEY(tipo_ativo_id) REFERENCES tipos_ativos(id));"
        );

        // 4. Ativos
        criarTabela("ATIVOS", 
            "CREATE TABLE IF NOT EXISTS ativos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "codigo_serial TEXT NOT NULL UNIQUE," 
            "tipo_ativo_id INTEGER,"              
            "status TEXT DEFAULT 'DISPONIVEL',"
            "observacao_fixa TEXT,"
            "FOREIGN KEY(tipo_ativo_id) REFERENCES tipos_ativos(id));"
        );

        // 5. Pedidos
        criarTabela("PEDIDOS", 
            "CREATE TABLE IF NOT EXISTS pedidos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "cliente_id INTEGER,"
            "data_pedido DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "data_entrega TEXT NOT NULL,"
            "data_recolhimento TEXT NOT NULL,"
            "nr_pessoas INTEGER,"
            "observacao_entrega TEXT,"
            "status TEXT NOT NULL,"
            "total_previsto REAL,"
            "total_final REAL,"
            "desconto REAL DEFAULT 0,"
            "forma_pagamento TEXT,"
            "status_pagamento TEXT DEFAULT 'PENDENTE',"
            "FOREIGN KEY(cliente_id) REFERENCES clientes(id));"
        );

        // 6. Itens do Pedido
        criarTabela("ITENS_PEDIDO", 
            "CREATE TABLE IF NOT EXISTS itens_pedido ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pedido_id INTEGER,"
            "produto_id INTEGER,"
            "quantidade_solicitada INTEGER NOT NULL,"
            "preco_unitario_momento REAL,"
            "FOREIGN KEY(pedido_id) REFERENCES pedidos(id),"
            "FOREIGN KEY(produto_id) REFERENCES produtos(id));"
        );

        // 7. Alocação
        criarTabela("ALOCACAO_ATIVOS", 
            "CREATE TABLE IF NOT EXISTS alocacao_ativos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pedido_id INTEGER,"
            "ativo_id INTEGER,"
            "data_hora_entrega DATETIME,"
            "data_hora_baixa DATETIME,"
            "status_retorno TEXT,"
            "obs_retorno TEXT,"
            "FOREIGN KEY(pedido_id) REFERENCES pedidos(id),"
            "FOREIGN KEY(ativo_id) REFERENCES ativos(id));"
        );
    }

public:
    Database(const std::string& db_name) {
        rc = sqlite3_open(db_name.c_str(), &db);
        if (rc) {
            std::cerr << "ERRO FATAL: Nao abriu banco: " << sqlite3_errmsg(db) << std::endl;
            exit(1);
        } else {
            setup();
        }
    }

    ~Database() {
        sqlite3_close(db);
    }

    sqlite3* getConnection() {
        return db;
    }
    
    bool executarQuery(const std::string& sql) {
        return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
    }
};

#endif