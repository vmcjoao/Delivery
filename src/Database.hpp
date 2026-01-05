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

    // Função auxiliar para rodar SQL e logar erro/sucesso
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

        // 2. Produtos (O Catálogo abstrato - Ex: Chopp 50L)
        criarTabela("PRODUTOS", 
            "CREATE TABLE IF NOT EXISTS produtos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "preco_base REAL NOT NULL," // Preço padrão de venda
            "tipo TEXT NOT NULL);"      // BARRIL, CHOPEIRA, CILINDRO
        );

        // 3. Ativos (Patrimônio Físico)
        // Ex: Barril nº B-045, Chopeira nº CH-02
        criarTabela("ATIVOS", 
            "CREATE TABLE IF NOT EXISTS ativos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "codigo_serial TEXT NOT NULL UNIQUE,"
            "produto_id INTEGER,"
            "status TEXT DEFAULT 'DISPONIVEL',"   // DISPONIVEL, EM_USO, MANUTENCAO
            "observacao_fixa TEXT,"               // Ex: "Amassado mas funciona"
            "FOREIGN KEY(produto_id) REFERENCES produtos(id));"
        );

        // 4. Pedidos
        criarTabela("PEDIDOS", 
            "CREATE TABLE IF NOT EXISTS pedidos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "cliente_id INTEGER,"
            "data_pedido DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "data_entrega TEXT NOT NULL,"
            "data_recolhimento TEXT NOT NULL,"
            "nr_pessoas INTEGER,"
            "observacao_entrega TEXT,"         // Obs gerais (Zona rural, etc)
            "status TEXT NOT NULL,"            // ABERTO, EM_ROTA, ENTREGUE, FINALIZADO
            
            // Controle Financeiro
            "total_previsto REAL,"             // Valor calculado no pedido
            "total_final REAL,"                // Valor real após fechamento
            "desconto REAL DEFAULT 0,"
            "forma_pagamento TEXT,"
            "status_pagamento TEXT DEFAULT 'PENDENTE',"
            
            "FOREIGN KEY(cliente_id) REFERENCES clientes(id));"
        );

        // 5. Itens do Pedido
        criarTabela("ITENS_PEDIDO", 
            "CREATE TABLE IF NOT EXISTS itens_pedido ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pedido_id INTEGER,"
            "produto_id INTEGER,"
            "quantidade_solicitada INTEGER NOT NULL,"
            "preco_unitario_momento REAL,"    // Preço no momento do pedido
            "FOREIGN KEY(pedido_id) REFERENCES pedidos(id),"
            "FOREIGN KEY(produto_id) REFERENCES produtos(id));"
        );

        // 6. Alocação de Ativos
        criarTabela("ALOCACAO_ATIVOS", 
            "CREATE TABLE IF NOT EXISTS alocacao_ativos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pedido_id INTEGER,"
            "ativo_id INTEGER,"
            "data_hora_entrega DATETIME,"
            "data_hora_baixa DATETIME,"
            "status_retorno TEXT,"            // VAZIO, CHEIO, COM_DEFEITO
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
    
    // Método utilitário para execuções rápidas
    bool executarQuery(const std::string& sql) {
        return sqlite3_exec(db, sql.c_str(), nullptr, 0, nullptr) == SQLITE_OK;
    }
};

#endif