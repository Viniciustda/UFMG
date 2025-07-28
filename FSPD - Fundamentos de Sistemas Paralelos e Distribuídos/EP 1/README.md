# Exercício de Programação 1 - FSPD

Este projeto simula movimentação sincronizada de múltiplas threads sobre um tabuleiro. Desenvolvido em C com POSIX Threads (Pthreads).

## 📁 Estrutura

```
EP 1/
├── src/          # Código-fonte
├── include/      # Arquivos .h
├── build/        # Arquivos objeto e executável
├── Makefile
└── README
```

## ⚙️ Compilação

Para compilar o projeto, execute:

```bash
make
```

ou

```bash
make build
```

O executável será gerado em `build/ex1`.

## 🧹 Limpeza

Para remover todos os arquivos gerados:

```bash
make clean
```

## ▶️ Execução

A execução do programa pode ser feita de diferentes formas, dependendo de como você deseja fornecer a entrada e visualizar a saída.

### 1. 📂 Usando redirecionamento com um arquivo de teste

Se você já tem um arquivo de entrada (ex: `teste.txt`):

```bash
./build/ex1 < teste.txt
```

A saída será exibida diretamente no terminal.

### 2. ⌨️ Digitando manualmente

Você pode rodar o programa e digitar a entrada diretamente no terminal:

```bash
./build/ex1
```

Depois de inserir todos os dados necessários, pressione `Ctrl+D` (em Linux/macOS) ou `Ctrl+Z` seguido de `Enter` (no Windows) para sinalizar o fim da entrada.

### 3. 🔀 Redirecionamento com saída ordenada

Se quiser garantir que a saída seja ordenada numericamente (por timestamp), você pode usar `sort -n`:

```bash
./build/ex1 < teste.txt | sort -n
```

A saída ordenada será exibida no terminal. Isso ajuda a compatibilizar com a saída esperada nos testes automáticos, mesmo que as linhas com o mesmo timestamp possam aparecer em ordem diferente.

### 4. ⌨️ Entrada manual com saída ordenada

Você também pode digitar os dados manualmente e ordenar a saída ao mesmo tempo:

```bash
./build/ex1 | sort -n
```

Depois de digitar a entrada, finalize como no exemplo 2. A saída ordenada será exibida diretamente no terminal.

## 📄 Formato do Arquivo de Entrada

O arquivo deve conter:

1. A dimensão do tabuleiro (N)
2. A quantidade de threads
3. Para cada thread:
   - identificador, grupo, número de posições
   - sequência de posições: x y tempo

Exemplo:

```
4 2
11      99  4
1       0       5
1       1       20
1       2       10
1       3       15
22      98  4
0       2       10
1       2       10
2       2       10
3       2       10
```

## ✏️ Autor

Vinicius Trindade - Ciência da Computação, UFMG