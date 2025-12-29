#ifndef PEDIDO_H
#define PEDIDO_H

#include <vector>
#include <string>
#include "Produto.hpp"

struct ItemPedido {
    Produto produto;
    int quantidade;
    double precoCobrado; // Importante: Salvar o preço DO DIA (se o produto aumentar depois, esse valor não muda)
};

class Pedido {
public:
    int id;
    int idCliente;
    double total;
    std::string status; // ABERTO, ENTREGUE, FINALIZADO
    std::vector<ItemPedido> itens;

    Pedido() : id(0), idCliente(0), total(0.0), status("ABERTO") {}

    // Função auxiliar para facilitar na main
    void adicionarItem(Produto p, int qtd) {
        ItemPedido item;
        item.produto = p;
        item.quantidade = qtd;
        item.precoCobrado = p.preco_unitario;
        
        itens.push_back(item);
        
        // Se for equipamento (preço 0), não soma no total financeiro
        total += (item.precoCobrado * qtd);
    }
};

#endif