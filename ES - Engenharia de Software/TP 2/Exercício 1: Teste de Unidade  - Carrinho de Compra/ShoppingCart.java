/**
* Engenharia de Software Moderna - Testes  (Cap. 8)
* Prof. Marco Tulio Valente
* 
* Exemplo simples de teste (ShoppingCart)
*
*/

/**
* Classe que será testada
*/

import java.util.ArrayList;
import java.util.List;

public class ShoppingCart {

  private List<Item> items;

  public ShoppingCart() {
    items = new ArrayList<>();
  }

  public void addItem(Item item) {
    items.add(item);
  }

  public void removeItem(Item item) {
    items.remove(item);
  }

  public void clearCart() {
    items.clear();
  }

  public int getItemCount() {
    return items.size();
  }

  public double getTotalPrice() {
    double totalPrice = 0.0;
    for (Item item : items) {
        totalPrice += item.getPrice();
    }
    return totalPrice;
  }

  public List<Item> getItems() {
    return items;
  }
}
