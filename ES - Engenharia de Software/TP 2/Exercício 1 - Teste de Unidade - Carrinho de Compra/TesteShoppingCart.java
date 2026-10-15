/**
* Engenharia de Software Moderna - Testes  (Cap. 8)
* Prof. Marco Tulio Valente
* 
* Exercício simples de teste de unidade (ShoppingCart)
*
*/

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;
import java.util.ArrayList;
import java.util.List;

public class TesteShoppingCart {

  private ShoppingCart shoppingCart;

  @Before
  public void setUp() {
    shoppingCart = new ShoppingCart();
    shoppingCart.addItem(new Item("ESM", 65.0));
    shoppingCart.addItem(new Item("GoF", 120.0));
  }

  @Test
  public void testAddItem() {
    int initialSize = shoppingCart.getItems().size();
    shoppingCart.addItem(new Item("UFMG", 80.0));
    assertEquals(initialSize + 1, shoppingCart.getItems().size());
    assertEquals("UFMG", shoppingCart.getItems().get(initialSize).getName());
  }

  @Test
  public void testRemoveItem() {
    Item itemToRemove = shoppingCart.getItems().get(0);
    shoppingCart.removeItem(itemToRemove);
    assertFalse(shoppingCart.getItems().contains(itemToRemove));
  }

  @Test
  public void testGetTotalPrice() {
    double totalPrice = shoppingCart.getTotalPrice();
    assertEquals(185.0, totalPrice, 0.001);
  }

  @Test
  public void testClearCart() {
    shoppingCart.clearCart();
    assertTrue(shoppingCart.getItems().isEmpty());
  }
}
