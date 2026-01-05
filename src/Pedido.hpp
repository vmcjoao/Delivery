#ifndef PEDIDO_H
#define PEDIDO_H

#include <vector>
#include <string>
#include "Produto.hpp"

struct ItemPedido {
    Produto produto;
    int quantidade;
    double precoNoMomento; // Preço na hora da venda
};

class Pedido {
public:
    int id;
    int idCliente;
    
    // Dados Logísticos
    std::string dataEntrega;
    std::string dataRecolhimento;
    int nrPessoas;
    std::string observacao;
    std::string status;           // ABERTO, EM_ROTA, ENTREGUE, FINALIZADO

    // Dados Financeiros
    double totalPrevisto;         // Soma dos itens
    double totalFinal;            // Após recolhimento
    double desconto;
    std::string formaPagamento;
    std::string statusPagamento;  // PENDENTE, PAGO

    std::vector<ItemPedido> itens;

    Pedido() : id(0), idCliente(0), nrPessoas(0), status("ABERTO"), 
               totalPrevisto(0.0), totalFinal(0.0), desconto(0.0), statusPagamento("PENDENTE") {}

    void adicionarItem(Produto p, int qtd) {
        ItemPedido item;
        item.produto = p;
        item.quantidade = qtd;
        item.precoNoMomento = p.precoBase;
        itens.push_back(item);
        
        // Atualiza o total previsto
        totalPrevisto += (item.precoNoMomento * qtd);
    }
};

#endif