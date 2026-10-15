# TP2: Asteroids

Instruções: [https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp2-asteroids](https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp2-asteroids)

---

## ⚙️ Ajustes Locais

Durante a configuração do projeto na minha máquina, precisei realizar alterações para conseguir compilar e rodar corretamente:

1. **GLEW**
    - O `CMakeLists.txt` original não localizou automaticamente o GLEW.
    - Adicionei manualmente os caminhos de include e library para que o CMake conseguisse encontrar o `glew32.lib` e os headers do GLEW.

2. **SDL2 Includes**
    - Os includes estavam no formato:
      ```cpp
      #include <SDL2/SDL_stdinc.h>
      ```
    - Na minha instalação do SDL2 (Windows), os headers não estão dentro de uma subpasta `SDL2/`.
    - Por isso, substituí todos os includes para o formato:
      ```cpp
      #include <SDL_stdinc.h>
      ```

Essas mudanças foram **necessárias apenas para compilar no meu ambiente** (Windows + CLion + Visual Studio/MSVC).

---

## 🚀 Funcionalidades Adicionais

Além dos requisitos básicos, foram implementadas as seguintes funcionalidades para aprimorar a jogabilidade:

### Sistema de Ondas Progressivas

O jogo não se limita a um número fixo de asteroides. Ele agora opera em um sistema de ondas com dificuldade progressiva, seguindo a **sequência de Fibonacci**:

-   O jogo começa com **1** asteroide.
-   Quando todos os asteroides da tela (incluindo os fragmentos) são destruídos, uma nova onda se inicia.
-   O número de asteroides na nova onda segue a sequência: **1, 2, 3, 5, 8, 13, 21**.
-   Após a onda com 21 asteroides, o ciclo reinicia, voltando para uma onda com apenas 1 asteroide.

### Zona de Spawn Segura

Para evitar inícios de jogo injustos onde um asteroide aparece imediatamente em cima da nave, foi implementada uma "zona segura". Ao iniciar cada onda, os asteroides são gerados em posições aleatórias que estão garantidamente fora de um raio seguro ao redor da nave, dando ao jogador tempo para reagir.