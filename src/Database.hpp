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