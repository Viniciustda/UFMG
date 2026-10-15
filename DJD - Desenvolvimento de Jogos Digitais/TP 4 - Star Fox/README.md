# TP4: Star Fox

Instruções: [https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp4-star-fox](https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp4-star-fox)

---

# 🚀 Funcionalidades
Esta seção detalha todas as implementações realizadas no jogo, organizadas por categoria.

## 🎮 Controles e Gameplay
- Controles de Jogo:

    - `W`, `A`, `S`, `D`: Movimentação da nave (para cima, baixo, esquerda e direita).

    - `Espaço`: Atirar laser.

    - `Enter` **(Durante o Jogo)**: Abre o Menu de Pausa.

    - `Enter` **(Nos Menus)**: Confirma a seleção.

    - `Setas Cima/Baixo`: Navega entre os botões nos menus.

## Velocidade da Nave:

- A velocidade de avanço da nave foi aumentada (de `400.0f` para `7000.0f`). O valor original foi corrigido para usar a variável `mSpeed`, pois a velocidade anterior tornava o jogo muito fácil.

## Rotação Dinâmica:

- A nave agora inclina suavemente ao se mover para a esquerda, direita, cima ou baixo, dando uma melhor sensação de voo.

## 🎵 Sistema de Áudio
- Música de Fundo Contínua:

    - A música de fundo (`Music.ogg`) é iniciada uma vez quando o jogo abre e toca continuamente, persistindo entre o `MainMenu`, o `Level1` e o `PauseMenu`.

- Gerenciamento de Sons:

    - Pausa: O Menu de Pausa (`Enter`) pausa todos os sons do jogo (tiros, alertas, motor da nave), exceto a música de fundo.

    - Limpeza de Cena: Ao sair do `Level1` (via "Exit" no `PauseMenu` ou `GameOver`), todos os sons da cena (como `ShipLoop.ogg` ou `DamageAlert.ogg`) são parados, evitando "vazamento" de áudio para o `MainMenu`. A música de fundo é a única mantida.

## 🧱 Nível e Obstáculos
- Geração de Obstáculos:

    - O jogo carrega 20 padrões de obstáculos pré-definidos do diretório `Assets/Blocks/`.

    - Após os 20 padrões iniciais serem usados, o jogo começa a selecionar padrões de forma aleatória, garantindo que o nível seja infinito e variado.

## 🖥️ UI e Troca de Cena
- Menu de Pausa:

    - Um menu de pausa funcional foi adicionado. Ele congela a ação do jogo e permite ao jogador "Continue" o  jogo ou "Exit" para o menu principal.

- Cross-Fade:

    - Foi implementado um efeito de cross-fade (fade-out e fade-in) para todas as trocas de cena (`MainMenu` -> `Level1`, `GameOver` -> `MainMenu`, etc.), tornando a transição mais suave.

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