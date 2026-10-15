
/**
* Engenharia de Software Moderna - Testes  (Cap. 8)
* Prof. Marco Tulio Valente
* 
* Exemplo de Teste de um Sistema de Bibliotecas. O código 
* contém bugs propositalmente incluídos para falhar os
* casos de teste.
*
*/

import org.junit.Test;
import org.junit.Before;
import org.junit.After;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertEquals;

import java.util.List;

public class BibliotecaTest {

  Biblioteca bib = new Biblioteca(new RepositorioMemoria());
  Livro livro1 = new Livro("isbn1", "ESM");
  Livro livro2 = new Livro("isbn2", "GoF");
  Livro livro3 = new Livro("isbn3", "XP");
  Usuario usuario1 = new Usuario("usu1", "Joao");
  Usuario usuario2 = new Usuario("usu2", "Maria");

  @Before
  public void init() {
    bib.adicionarLivroAcervo(livro1);
    bib.adicionarLivroAcervo(livro2);
    bib.adicionarLivroAcervo(livro3);
  }

  @Test
  public void testaEmprestimo() throws ExcecaoLivroEmprestado {
    bib.emprestarLivro(livro1, usuario1);
    bib.emprestarLivro(livro2, usuario1);
    List<Livro> emprestados = bib.livrosEmprestadosUsuario(usuario1);
    assertEquals(emprestados.size(), 2);
    assertTrue(emprestados.contains(livro1));
    assertTrue(emprestados.contains(livro2));
  }

  @Test
  public void testaEmprestimoVazio() throws ExcecaoLivroEmprestado {
    List<Livro> emprestados = bib.livrosEmprestadosUsuario(usuario1);
    assertEquals(emprestados.size(), 0);
  }

  @Test
  public void testaEmprestimoEDevolucao() throws ExcecaoLivroEmprestado {
    bib.emprestarLivro(livro1, usuario1);
    bib.emprestarLivro(livro2, usuario1);
    bib.receberLivroEmprestado(livro1);
    List<Livro> emprestados = bib.livrosEmprestadosUsuario(usuario1);
    assertEquals(emprestados.size(), 1);
    assertFalse(emprestados.contains(livro1));
    assertTrue(emprestados.contains(livro2));
  }

  @Test(expected = ExcecaoLivroEmprestado.class)
  public void testaEmprestarLivroJaEmprestado() throws ExcecaoLivroEmprestado {
    bib.emprestarLivro(livro1, usuario1);
    bib.emprestarLivro(livro1, usuario2);
  }
}