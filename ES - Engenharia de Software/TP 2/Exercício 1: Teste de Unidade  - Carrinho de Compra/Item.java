/**
* Engenharia de Software Moderna - Testes  (Cap. 8)
* Prof. Marco Tulio Valente
* 
* Exemplo simples de teste (ShoppingCart)
*
*/

/* Itens que serão armazenados no carrinho de compras */

public class Item {

  private String name;
  private double price;

  public Item(String name, double price) {
    this.name = name;
    this.price = price;
  }

  public String getName() {
    return name;
  }

  public double getPrice() {
    return price;
  }
}
