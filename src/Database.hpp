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

    // Função auxiliar para rodar um SQL e avisar se deu erro
    void criarTabela(const std::string& nomeTabela, const std::string& sql) {
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "Erro fatal ao criar tabela " << nomeTabela << ": " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        } else {
            // Comente esta linha se quiser limpar o terminal depois
            std::cout << "Tabela " << nomeTabela << " verificada." << std::endl;
        }
    }

    void setup() {
        // 1. Clientes
        std::string sqlClientes = 
            "CREATE TABLE IF NOT EXISTS clientes ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "telefone TEXT NOT NULL,"
            "endereco TEXT NOT NULL,"
            "data_cadastro DATETIME DEFAULT CURRENT_TIMESTAMP);";
        criarTabela("CLIENTES", sqlClientes);

        // 2. Produtos
        std::string sqlProdutos = 
            "CREATE TABLE IF NOT EXISTS produtos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nome TEXT NOT NULL,"
            "preco_unitario REAL NOT NULL,"
            "tipo TEXT NOT NULL);";
        criarTabela("PRODUTOS", sqlProdutos);

        // 3. Pedidos
        std::string sqlPedidos = 
            "CREATE TABLE IF NOT EXISTS pedidos ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "cliente_id INTEGER,"
            "data_pedido DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "status TEXT NOT NULL,"
            "total_pedido REAL,"
            "FOREIGN KEY(cliente_id) REFERENCES clientes(id));";
        criarTabela("PEDIDOS", sqlPedidos);

        // 4. Itens (Com a nova coluna qtd_devolvida)
        std::string sqlItens = 
            "CREATE TABLE IF NOT EXISTS itens_pedido ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pedido_id INTEGER,"
            "produto_id INTEGER,"
            "quantidade INTEGER NOT NULL,"
            "preco_momento REAL NOT NULL,"
            "qtd_devolvida INTEGER DEFAULT 0,"
            "FOREIGN KEY(pedido_id) REFERENCES pedidos(id),"
            "FOREIGN KEY(produto_id) REFERENCES produtos(id));";
        criarTabela("ITENS_PEDIDO", sqlItens);
    }

public:
    Database(const std::string& db_name) {
        // Tenta abrir/criar o arquivo
        rc = sqlite3_open(db_name.c_str(), &db);

        if (rc) {
            std::cerr << "ERRO CRITICO: Nao foi possivel abrir o arquivo do banco: " 
                      << sqlite3_errmsg(db) << std::endl;
            // Fecha se falhar na abertura
            sqlite3_close(db);
            exit(1); // Encerra o programa pois sem banco nada funciona
        } else {
            // Se abriu, roda o setup
            setup();
        }
    }

    ~Database() {
        sqlite3_close(db);
    }

    sqlite3* getConnection() {
        return db;
    }
    
    // Método extra caso precise rodar SQL avulso na main
    bool executarQuery(const std::string& sql) {
        rc = sqlite3_exec(db, sql.c_str(), nullptr, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Erro SQL: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            return false;
        }
        return true;
    }
};

#endif