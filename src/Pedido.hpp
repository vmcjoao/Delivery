#ifndef PEDIDO_H
#define PEDIDO_H

#include <vector>
#include <string>
#include "Produto.hpp"

struct ItemPedido {
    Produto produto;
    int quantidade;
    double precoCobrado;
};

class Pedido {
public:
    int id;
    int idCliente;
    double total;
    std::string status; 
    
    // --- NOVOS CAMPOS ---
    std::string dataEntrega;      // Formato sugerido: YYYY-MM-DD
    std::string dataRecolhimento; // Formato sugerido: YYYY-MM-DD
    std::string observacao;       // Ex: "Sítio, estrada de terra"
    double taxaEntrega;           // Ex: 50.00
    // --------------------

    std::vector<ItemPedido> itens;

    // Construtor atualizado
    Pedido() : id(0), idCliente(0), total(0.0), status("ABERTO"), taxaEntrega(0.0) {}

    void adicionarItem(Produto p, int qtd) {
        ItemPedido item;
        item.produto = p;
        item.quantidade = qtd;
        item.precoCobrado = p.preco_unitario;
        itens.push_back(item);
        
        // Soma apenas os itens ao total (a taxa será somada depois)
        total += (item.precoCobrado * qtd);
    }
    
    // Método para calcular o total final com a taxa
    double getTotalComTaxa() const {
        return total + taxaEntrega;
    }
};

#endif