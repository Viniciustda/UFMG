# TP1: Pong

Instruções: [https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp1-pong](https://lucasnfe.github.io/dcc192-2025-2/avaliacoes/tp1-pong)

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

3. **OpenGL**
    - Precisei adicionar o comando para localizar o `OpenGL` no `CMakeLists.txt`.
      ```txt
      find_package(OpenGL REQUIRED)
      ``` 

Essas mudanças foram **necessárias apenas para compilar no meu ambiente** (Windows + CLion + Visual Studio/MSVC).