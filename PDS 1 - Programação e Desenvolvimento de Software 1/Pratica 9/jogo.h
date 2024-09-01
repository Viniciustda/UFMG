typedef struct Guerreiro { 
    int ataque; 
    int defesa; 
    int carisma; 
    int pontos_vida;
    int id_guerreiro;
} Guerreiro; 

int rolaDados();

void criaGuerreiro(Guerreiro *x);

int bonusCarisma(int carisma);

void ataca(Guerreiro *a, Guerreiro *b);

void jogo();