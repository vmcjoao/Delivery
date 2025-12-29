#include "Database.hpp"

int main() {
    Database meuBanco("delivery.db");

    // Teste
    std::string sqlInsert = "INSERT INTO clientes (nome, telefone, endereco) " \
                            "VALUES ('Joao do Chopp', '3199999-9999', 'Rua Sei la das Quantas, 123');";

    if (meuBanco.executarQuery(sqlInsert)) {
        std::cout << "Cliente inserido com sucesso!" << std::endl;
    }

    return 0;
}