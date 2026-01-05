#ifndef ATIVO_H
#define ATIVO_H

#include <string>

class Ativo {
public:
    int id;
    std::string codigoSerial;   // Ex: "B-045", "CH-02"
    int tipoAtivoId;              // Vincula ao tipo de produto
    std::string status;         // "DISPONIVEL", "EM_USO", "MANUTENCAO"
    std::string observacao;     // Ex: "Amassado na lateral"

    Ativo() : id(0), tipoAtivoId(0), status("DISPONIVEL") {}

    Ativo(std::string serial, int tId, std::string obs = "") 
        : id(0), codigoSerial(serial), tipoAtivoId(tId), status("DISPONIVEL"), observacao(obs) {}
};

#endif