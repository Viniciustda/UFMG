# TP3: Super Mario Bros

Instruções: [https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp3-smb](https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp3-smb)

---

## 🚀 Novas Funcionalidades e Alterações

Esta seção detalha todas as mudanças e implementações realizadas no jogo, organizadas por categoria.

### 🍄 Atores (Mario & Inimigos)

- **Power-ups (Mario / Super Mario):**
  - Ao coletar um cogumelo, o Mario faz "PowerUp", mudando seu *collider* e *animator* para o sprite sheet do Super Mario.
  - Ao ser atingido como Super Mario, ele retorna ao estado normal (Power-down).
  - As animações de ambos os estados foram ajustadas para mapear corretamente os índices dos novos sprite sheets (JSON).

- **Invencibilidade Temporária:**
  - Após perder um power-up (de Super Mario para Mario), o jogador fica temporariamente invencível para evitar mortes imediatas.

- **Animação de Morte (Mario):**
  - Ao morrer, Mario executa um pequeno pulo e cai pela tela, ignorando todas as colisões.

- **Animações de Morte (Goomba):**
  - **Pulo:** Se o Mario pula sobre o Goomba, ele é "esmagado".
  - **Bloco:** Se o Goomba é derrotado por um bloco atingido por baixo, ele é lançado na diagonal, de cabeça para baixo, para fora da tela.

### 🧱 Blocos e Interações

- **Comportamento de Blocos ("Bump"):**
  - Blocos de interrogação (`?`) agora usam o mesmo comportamento de "bump" (colisão por baixo) que os blocos de tijolo.
  - Ao serem ativados, o bloco guarda o índice do frame (`mFrameIndex`) e troca sua animação para o estado "usado".

- **Quebra de Blocos:**
  - O Super Mario (estado com power-up) pode quebrar blocos de tijolo ao atingi-los por baixo.

- **Interação Bloco-Inimigo:**
  - Se o Mario atinge um bloco por baixo e um Goomba está posicionado exatamente sobre ele, o Goomba é derrotado (com a animação de ser lançado).

### 💰 Coletáveis (Cogumelos e Moedas)

- **Cogumelo (`Mushroom`):**
  - A classe `Mushroom` foi separada em seus próprios arquivos (`.h`/`.cpp`).
  - O cogumelo é um item **sólido** que colide fisicamente com o cenário e atores.
  - **Spawning:**
    - No Tilemap (CSV), o ID do cogumelo é posicionado na célula do bloco `?` que deve contê-lo.
    - O código interpreta isso e cria um `Block` de interrogação com a flag `hasMushroom = true`.
    - Ao ser ativado (Bump), o bloco instancia o `Mushroom` atrás do bloco `?` e sobe, dando a impressão de estar saindo do bloco.

- **Moedas (`Coin`):**
  - A classe `Coin` foi separada em seus próprios arquivos (`.h`/`.cpp`).
  - **Tipos de Moeda:**
    1.  **Blocos:** Blocos `?` que não contêm cogumelos geram uma moeda (com efeito visual) ao serem atingidos.
    2.  **Estáticas:** Moedas estáticas (ID `3` no CSV) são posicionadas diretamente no mapa.
  - **Comportamento:** Moedas **NÃO SÃO SÓLIDAS**. Mario e inimigos as atravessam.
  - **Coleta:** Ao tocar na moeda, ela é coletada (desaparece).
  - **Contagem:** O número de moedas coletadas é armazenado em uma variável (atualmente não exibida na UI, mas pronta para uso futuro).

### 🎮 Gameplay e Controles

- **Pulo sobre Inimigos:**
  - Ao pular sobre um inimigo (Goomba), o Mario realiza um "quique" (um pulo menor), replicando o comportamento do jogo original.

- **Controles do Jogo:**
  - **`ESC`**: Fecha o jogo.
  - **`Enter`**: Reinicia a fase após a morte do jogador.

### 🛠️ Estrutura do Projeto e Assets

- **Novos Níveis:**
  - Foi criada a nova fase `Level1-2` (localizada em `Assets/Levels/Level1-2/level1-2.csv`).
  - A fase original `Level1-1` foi mantida como backup.

- **Gerenciamento de Assets:**
  - O projeto foi atualizado para parar de usar PNGs individuais para animações.
  - Agora, os seguintes elementos usam **Sprite Sheets** e seus respectivos arquivos **JSON** para mapeamento:
    - `Blocks`
    - `Collectables`
    - `Mario` / `SuperMario`
    - `Goomba`

- **Build (CMake):**
  - O `CMakeLists.txt` foi atualizado para incluir os novos arquivos da classe `Mushroom` (`Mushroom.cpp` e `Mushroom.h`) na compilação.

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