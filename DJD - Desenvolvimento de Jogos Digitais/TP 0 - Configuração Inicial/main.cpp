#include <SDL.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Tamanho da janela
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct Color {
    Uint8 r, g, b, a;
};

void drawCircle(SDL_Renderer* renderer, int center_x, int center_y, int radius, int sides) {
    // Polígono tem pelo menos 3 lados
    if (sides < 3) {
        return;
    }

    // O ângulo entre cada vértice do polígono
    const double angle_step = 2.0 * M_PI / sides;

    // Começa no primeiro vértice (ângulo 0)
    double old_x = center_x + radius;
    double old_y = center_y;

    // Itera por todos os outros vértices
    for (int i = 1; i <= sides; ++i) {
        double angle = i * angle_step;
        // Calcula a posição (x, y) do vértice atual no círculo
        double new_x = center_x + radius * cos(angle);
        double new_y = center_y + radius * sin(angle);

        // Desenha a linha do vértice anterior para o atual
        SDL_RenderDrawLine(renderer, static_cast<int>(old_x), static_cast<int>(old_y), static_cast<int>(new_x), static_cast<int>(new_y));

        // O vértice atual se torna o anterior para a próxima iteração
        old_x = new_x;
        old_y = new_y;
    }
}

bool isPointInCircle(int point_x, int point_y, int circle_x, int circle_y, int radius) {
    int dx = point_x - circle_x;
    int dy = point_y - circle_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool isPointInRect(int point_x, int point_y, const SDL_Rect& rect) {
    return (point_x >= rect.x && point_x <= rect.x + rect.w &&
            point_y >= rect.y && point_y <= rect.y + rect.h);
}

int main(int argc, char* args[]) {
    // Inicializa gerador de números aleatórios
    srand(time(NULL));

    // Variaveis
    const int square_size = 100;
    int initial_mouse_x = 0;

    // Inicialização do subsistema de vídeo da SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Nao foi possivel inicializar o subsistema de video da SDL: %s", SDL_GetError());
        return -1;
    }

    // Criação da janela
    SDL_Window* window = SDL_CreateWindow(
            "TP 0 - Configuração Inicial - Vinicius Trindade",  // Titulo da Janela
            SDL_WINDOWPOS_CENTERED,                             // Posição X - Centro da tela
            SDL_WINDOWPOS_CENTERED,                             // Posição Y - Centro da tela
            WINDOW_WIDTH,                                       // Largura
            WINDOW_HEIGHT,                                      // Altura
            SDL_WINDOW_SHOWN                                    // Flag - Janela fica visivel quando criada
    );

    if (window == nullptr) {
        SDL_Log("Nao foi possivel criar a janela: %s", SDL_GetError());
        return -1;
    }

    // Criação do buffer de fundo
    SDL_Renderer* renderer = SDL_CreateRenderer(
                window,                                                     // Janela
                -1,                                                         // Indice
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC        // Flags - Aceleração Gráfica - Usa GPU
                                                                            //       - Sincronização Vertical - Sincroniza a atualização da tela com a taxa de atualização do monitor
        );

    if (renderer == nullptr) {
        SDL_Log("Nao foi possivel criar o renderer: %s", SDL_GetError());
        return -1;
    }

    // Quadrado 1
    Color square_1_color = {0, 255, 0, 255}; // Verde inicialmente
    const int square_1_x = (WINDOW_WIDTH / 2) - (square_size / 2);
    const int square_1_y = (WINDOW_HEIGHT / 2) - (square_size / 2);
    SDL_Rect square_1;
    square_1.w = square_size;
    square_1.h = square_size;
    square_1.x = square_1_x;
    square_1.y = square_1_y;
    bool is_dragging_s1 = false;
    bool is_resizing_s1 = false;
    int initial_s1_size = 0;

    // Quadrado 2
    Color square_2_color = {255, 0, 0, 255}; // Vermelho inicialmente
    SDL_Rect square_2;
    square_2.w = square_size/2;
    square_2.h = square_size/2;
    square_2.x = square_1_x + 1.5*square_size;
    square_2.y = square_1_y - square_size;
    bool is_dragging_s2 = false;
    bool is_resizing_s2 = false;
    int initial_s2_size = 0;

    // Círculo
    Color circle_color = {0, 255, 255, 255}; // Ciano inicialmente
    int circle_x = 200;
    int circle_y = 450;
    int circle_radius = 80;
    const int circle_sides = 64;
    bool is_dragging_circle = false;
    bool is_resizing_circle = false;
    int initial_circle_radius = 0;

    // Loop para processar eventos de entrada
    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        // Eventos
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: // Fechar janela
                    is_running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_RIGHT) { // Botão direito pressionado
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        if (isPointInRect(mouseX, mouseY, square_1)) {
                            is_dragging_s1 = true;
                        }
                        else if (isPointInRect(mouseX, mouseY, square_2)) {
                            is_dragging_s2 = true;
                        }
                        else if (isPointInCircle(mouseX, mouseY, circle_x, circle_y, circle_radius)) {
                            is_dragging_circle = true;
                        }
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouse_x = event.button.x;
                        int mouse_y = event.button.y;
                        if (event.button.clicks == 1) {
                            if (isPointInRect(mouse_x, mouse_y, square_1)) {
                                is_resizing_s1 = true;
                                initial_mouse_x = mouse_x;
                                initial_s1_size = square_1.w;
                            } else if (isPointInRect(mouse_x, mouse_y, square_2)) {
                                is_resizing_s2 = true;
                                initial_mouse_x = mouse_x;
                                initial_s2_size = square_2.w;
                            } else if (isPointInCircle(mouse_x, mouse_y, circle_x, circle_y, circle_radius)) {
                                is_resizing_circle = true;
                                initial_mouse_x = mouse_x;
                                initial_circle_radius = circle_radius; // Grava o raio inicial
                            }
                        }
                        else if (event.button.clicks == 2) {
                            // Clique duplo botão esquerdo
                            if (isPointInRect(mouse_x, mouse_y, square_1)) {
                                square_1_color = { (Uint8)(rand() % 256), (Uint8)(rand() % 256), (Uint8)(rand() % 256), 255 };
                            }
                            else if (isPointInRect(mouse_x, mouse_y, square_2)) {
                                square_2_color = { (Uint8)(rand() % 256), (Uint8)(rand() % 256), (Uint8)(rand() % 256), 255 };
                            }
                            else if (isPointInCircle(mouse_x, mouse_y, circle_x, circle_y, circle_radius)) {
                                circle_color = { (Uint8)(rand() % 256), (Uint8)(rand() % 256), (Uint8)(rand() % 256), 255 };
                            }
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_RIGHT) {      // Botão direito solto
                        is_dragging_s1 = false;
                        is_dragging_s2 = false;
                        is_dragging_circle = false;
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) {      // Botão esquerdo solto
                        is_resizing_s1 = false;
                        is_resizing_s2 = false;
                        is_resizing_circle = false;
                    }
                    break;
                case SDL_MOUSEMOTION: // Arrasto
                    if (is_dragging_s1) {
                        square_1.x = event.motion.x - (square_1.w / 2);
                        square_1.y = event.motion.y - (square_1.h / 2);
                    }
                    else if (is_dragging_s2) {
                        square_2.x = event.motion.x - (square_2.w / 2);
                        square_2.y = event.motion.y - (square_2.h / 2);
                    }
                    else if (is_dragging_circle) {
                        circle_x = event.motion.x;
                        circle_y = event.motion.y;
                    }
                    else if (is_resizing_s1) {
                        int delta_x = event.motion.x - initial_mouse_x;
                        int new_size = initial_s1_size + delta_x;
                        if (new_size < 10) new_size = 10;
                        square_1.x += (square_1.w - new_size) / 2;
                        square_1.y += (square_1.h - new_size) / 2;
                        square_1.w = new_size;
                        square_1.h = new_size;
                    }
                    else if (is_resizing_s2) {
                        int delta_x = event.motion.x - initial_mouse_x;
                        int new_size = initial_s2_size + delta_x;
                        if (new_size < 10) new_size = 10;
                        square_2.x += (square_2.w - new_size) / 2;
                        square_2.y += (square_2.h - new_size) / 2;
                        square_2.w = new_size;
                        square_2.h = new_size;
                    }
                    else if (is_resizing_circle) {
                        int delta_x = event.motion.x - initial_mouse_x;
                        int new_radius = initial_circle_radius + delta_x;
                        if (new_radius < 10) new_radius = 10; // Raio mínimo
                        circle_radius = new_radius;
                    }
                    break;
            }
        }

        // Limpa a tela com a cor de fundo
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Cinza escuro
        SDL_RenderClear(renderer);

        // Desenha o quadrado 1
        SDL_SetRenderDrawColor(renderer, square_1_color.r, square_1_color.g, square_1_color.b, square_1_color.a);
        SDL_RenderFillRect(renderer, &square_1);

        // Desenha o quadrado 2
        SDL_SetRenderDrawColor(renderer, square_2_color.r, square_2_color.g, square_2_color.b, square_2_color.a);
        SDL_RenderFillRect(renderer, &square_2);

        // Desenha o círculo
        SDL_SetRenderDrawColor(renderer, circle_color.r, circle_color.g, circle_color.b, circle_color.a);
        drawCircle(renderer, circle_x, circle_y, circle_radius, circle_sides);

        // Apresenta o desenho final na tela
        SDL_RenderPresent(renderer);
    }

    // Destruição e finalização
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}