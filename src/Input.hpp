#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <string>
#include <limits>
#include <sstream>

class Input {
public:
    static std::string lerString(const std::string& mensagem) {
        std::cout << mensagem;
        std::string linha;
        std::getline(std::cin, linha);
        return linha;
    }

    static int lerInteiro(const std::string& mensagem) {
        while (true) {
            std::cout << mensagem;
            std::string linha;
            std::getline(std::cin, linha);

            try {
                size_t pos;
                int numero = std::stoi(linha, &pos);
                
                if (pos == linha.length()) {
                    return numero;
                }
            } catch (...) {

            }
            
            std::cout << ">> Entrada invalida! Digite apenas numeros inteiros.\n";
        }
    }

    static double lerDecimal(const std::string& mensagem) {
        while (true) {
            std::cout << mensagem;
            std::string linha;
            std::getline(std::cin, linha);

            try {
                size_t pos;
                double numero = std::stod(linha, &pos);
                if (pos == linha.length()) {
                    return numero;
                }
            } catch (...) {}

            std::cout << ">> Valor invalido! Use ponto para decimais (ex: 15.50).\n";
        }
    }

    static void pausar() {
        std::cout << "\nPressione ENTER para continuar...";
        std::string lixo;
        std::getline(std::cin, lixo);
    }
    
    static void limparTela() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
};

#endif