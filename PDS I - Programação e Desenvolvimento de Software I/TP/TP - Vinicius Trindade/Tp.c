#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAM_MAX 100

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int GRASS_H = 60;

const int NAVE_W = 100;
const int NAVE_H = 50;
const float NAVE_VEL = 1;

const int ALIEN_W = 50;
const int ALIEN_H = 25;
const float VEL_ALIEN = 2;

const int TIRO_W = 4;
const int TIRO_H = 4;
const float TIRO_VEL = 2.25;

const float FPS = 100;

//matriz de aliens
const int n = 4;  //linhas
const int m = 6;  //colunas

typedef struct Nave {
    float x;
    float vel;
    int dir, esq;
    ALLEGRO_COLOR cor;
} Nave;

typedef struct Alien {
    float x, y;
    float x_vel, y_vel;
    int vivo;
    ALLEGRO_COLOR cor;
} Alien;

typedef struct Tiro {
    float x, y;
    float y_vel;
    int up;
    ALLEGRO_COLOR cor;
} Tiro;

void initNave(Nave *nave) {
    nave->x = SCREEN_W/2;
    nave->vel = NAVE_VEL;
    nave->dir = 0;
    nave->esq = 0;
    nave->cor = al_map_rgb(0, 0, 255);
}

void initAliens(Alien aliens[n][m]) {
    float posicaoX = 0;
    float posicaoY = 0;
    for (int i = 0; i < n; i++)
    {
        posicaoX = 0;
        for (int j = 0; j < m; j++)
        {
            aliens[i][j].x = posicaoX;
            aliens[i][j].y = posicaoY;
            aliens[i][j].x_vel = VEL_ALIEN;
            aliens[i][j].y_vel = ALIEN_H;
            aliens[i][j].cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
            aliens[i][j].vivo = 1;
            posicaoX += ALIEN_W * 2;
        }
        posicaoY += ALIEN_H * 2;
    }
}

void initTiro(Tiro *tiro){
    tiro->x = -100;
    tiro->y = -100;
    tiro->y_vel = TIRO_VEL;
    tiro->cor = al_map_rgb(255, 0, 0);
    tiro->up = 0;
}

void draw_scenario() {

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H,
                             SCREEN_W, SCREEN_H,
                             al_map_rgb(0, 255, 0));

}

void draw_nave(Nave nave) {
    float y_base = SCREEN_H - GRASS_H/2;
    al_draw_filled_triangle(nave.x, y_base - NAVE_H, 
                            nave.x - NAVE_W/2, y_base,
                            nave.x + NAVE_W/2, y_base, 
                            nave.cor);
}

void draw_alien(Alien aliens[n][m], int i, int j) {
    
     al_draw_filled_rectangle(aliens[i][j].x, aliens[i][j].y,
                            aliens[i][j].x + ALIEN_W, aliens[i][j].y + ALIEN_H,
                            aliens[i][j].cor);
        
}

void draw_tiro(Tiro *tiro) {
    al_draw_filled_rectangle(tiro->x, tiro->y,
                             tiro->x + TIRO_W, tiro->y + TIRO_H,
                             tiro->cor);
}

int colisao_alien_solo (Alien aliens[n][m]) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if(aliens[i][j].y + ALIEN_H > SCREEN_H - GRASS_H)
                return 1;
        }
    } 
    return 0;
}

int colisao_alien_nave (Alien aliens[n][m], Nave nave) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if((aliens[i][j].y + ALIEN_H > (SCREEN_H - GRASS_H/2)- NAVE_H) && (aliens[i][j].x + ALIEN_W >= nave.x + 15 || aliens[i][j].x + ALIEN_W >= nave.x - 15))
                return 1;
        }
    }
    return 0;
}

int colisao_tiro_alien (Tiro *tiro, Alien aliens[n][m]) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if((tiro->x + TIRO_W > aliens[i][j].x && tiro->x < aliens[i][j].x + ALIEN_W) && (tiro->y + TIRO_H > aliens[i][j].y && tiro->y < aliens[i][j].y + ALIEN_H)) {
                tiro->up = 0;
                tiro->x = -100;
                tiro->y = -100;
                aliens[i][j].vivo = 0;
                return 1;
            }
        }
    }
    return 0;
}

void update_nave (Nave *nave) {
    if(nave->dir && nave->x + nave->vel <= SCREEN_W) {
        nave->x += nave->vel;
    }
    if(nave->esq && nave->x - nave->vel >= 0) {
        nave->x -= nave->vel;
    }
}

void update_alien(Alien aliens[n][m]) {

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if(aliens[i][j].x + ALIEN_W + aliens[i][j].x_vel > SCREEN_W || aliens[i][j].x + aliens[i][j].x_vel < 0) {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < m; j++)
                    {
                        aliens[i][j].y += aliens[i][j].y_vel;
                        aliens[i][j].x_vel *= -1;
                    }
                }
            }
            aliens[i][j].x += aliens[i][j].x_vel;
        }
    }           
}

void update_tiro(Tiro *tiro){
    if(tiro->up == 1) {
        draw_tiro(tiro);
        tiro->y -= tiro->y_vel;
    }
    if(tiro->y <= 0){
        tiro->up = 0;
    }
}

int modificaRecorde(char nome_arquivo[], int cont) {
	int recorde = -1;
	char* palavra1;
    char* palavra2;
	char buf[TAM_MAX];
	FILE *arq, *temp;

	//abrir arquivo nome_arquivo para leitura o colocando na variavel arq:
	arq = fopen(nome_arquivo, "r");
	
	//abrir um arquivo temporario "temp" (ex: temp.txt) para escrita o colocando na variavel temp:
	temp = fopen("temp.txt", "w");
	
	fgets(buf, TAM_MAX, arq);
	while(!feof(arq)) {
		palavra1 = strtok(buf, " ");
		palavra2 = strtok(NULL, " ");
        recorde = atoi(strtok(NULL, "\n"));
		
		if (cont > recorde) {
			recorde = cont;
		}
			
		//escrever os dados lidos do arquivo "nome_arquivo" no arquivo temporario
		fprintf(temp, "%s %s %d\n", palavra1, palavra2, recorde);
        fgets(buf, TAM_MAX, arq);
	}
	
	fclose(arq);
	fclose(temp);
	
	//remove o arquivo antigo
	remove(nome_arquivo);
	//renomeia o arquivo temporario
	rename("temp.txt", nome_arquivo);

    return recorde;
	
}

int main() {

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    //inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

    //cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	

    //cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

    //cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

    //instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}



    //inicializa o modulo allegro que carrega as fontes
    al_init_font_addon();
	//inicializa o modulo allegro que entende arquivos tff de fontes
    al_init_ttf_addon();
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_30 = al_load_font("arial.ttf", 30, 1);

    //registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
    //registra na fila os eventos de teclado (ex: pressionar uma tecla)
    al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());  
    //registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

    Nave nave;
    initNave(&nave);

    Alien aliens[n][m];
    initAliens(aliens);

    Tiro tiro;
    initTiro(&tiro);

    int playing = 1;
    int perdeu = 0;
    int cont = 0;
    int recorde = 0;
    char frase[TAM_MAX];
    char nome_arquivo[15] = "recorde123.txt";

    //inicia o temporizador
	al_start_timer(timer);

    while(playing) {

        ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

        //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {

            draw_scenario();

            sprintf(frase, "Pontuacao: %d", cont);
            al_draw_text(size_30, al_map_rgb(0, 0, 205), 10, 506, 0, frase);

            update_nave(&nave);

            update_alien(aliens);

            update_tiro(&tiro);

            draw_nave(nave);

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    if(aliens[i][j].vivo == 1) {
                        draw_alien(aliens, i, j);
                    }
                }
            }

            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    if(aliens[i][j].vivo == 0) {
                        aliens[i][j].x = SCREEN_W/2;
                        aliens[i][j].y = -2000000;
                    }                
                }
            }

            if (colisao_tiro_alien(&tiro, aliens) == 1){
                cont++;
            }

            if(colisao_alien_solo(aliens) ||  colisao_alien_nave(aliens, nave)) {
                perdeu = 1;
                playing = 0;
            }
            if (cont >= n*m) {
                playing = 0;
            }

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}

        //se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            perdeu = 1;
			playing = 0;
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_A:
                nave.esq = 1;
                break;
            case ALLEGRO_KEY_D:
                nave.dir = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                if (tiro.up == 0) {
                    tiro.up = 1;
                    tiro.x = nave.x - TIRO_W/2;
                    tiro.y = (SCREEN_H - GRASS_H/2) - NAVE_H;
                }
                break;
            case ALLEGRO_KEY_R:
                cont = 0;
                initNave(&nave);
                initAliens(aliens);
                initTiro(&tiro);
                break;
            }   

		}

        else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_A:
                nave.esq = 0;
                break;
            case ALLEGRO_KEY_D:
                nave.dir = 0;
                break;
            }  

		}
    }

    recorde = modificaRecorde(nome_arquivo, cont);
	
	char my_text[TAM_MAX];
    char my_text2[TAM_MAX];
		
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(0,0,0));
	//imprime o texto armazenado em my_text na posicao x=10,y=10 e com a cor rgb(128,200,30)
	if(perdeu) {
        sprintf(my_text, "Perdeu :( Voce eliminou %d de %d aliens", cont, n*m);
		al_draw_text(size_30, al_map_rgb(200, 0, 30), SCREEN_W/4, SCREEN_H/2, 0, my_text);
        sprintf(my_text2, "Recorde atual: %d", recorde);
		al_draw_text(size_30, al_map_rgb(255, 255, 0), SCREEN_W/3, (SCREEN_H/4)*3, 0, my_text2);
	} else {
		sprintf(my_text, "Ganhou! Voce eliminou todos os %d aliens em %.2f segundos", n*m, al_get_timer_count(timer)/FPS);	
		al_draw_text(size_30, al_map_rgb(0, 200, 30), SCREEN_W/12, SCREEN_H/2, 0, my_text);
        sprintf(my_text2, "Recorde atual: %d", recorde);
		al_draw_text(size_30, al_map_rgb(255, 255, 0), SCREEN_W/3, (SCREEN_H/4)*3, 0, my_text2);
	}

    //reinicializa a tela
	al_flip_display();	
    al_rest(5);

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}