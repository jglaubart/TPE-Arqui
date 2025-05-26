#include <eliminator.h>
#include <usr_stdlib.h>
#include <colores.h>
#include <sys_calls.h>
#include <sound.h>


#define WIDTH 40
#define HEIGHT 30
#define MAXDIM 100

static Color BLACK = {0,0,0};
static Color WHITE = {255,255,255};
static Color RED = {0,0,255};
static Color BLUE = {255,0,0};
static Color ORANGE = {16,160,255};
static Color YELLOW = {30,224,255};

#define PIXELWIDTH (getScreenWidth() / WIDTH)
#define PIXELHEIGHT (getScreenHeight() / HEIGHT)
#define MAX_BUFFER 254

#define PLAYER1_UP 'w'
#define PLAYER1_DOWN 's'
#define PLAYER1_LEFT 'a'
#define PLAYER1_RIGHT 'd'

#define PLAYER2_UP 'i'
#define PLAYER2_DOWN 'k'
#define PLAYER2_LEFT 'j'
#define PLAYER2_RIGHT 'l'


static unsigned long int next = 1;
int gameover;
int scoreP0 = 0;
int scoreP1 = 0;
int scoreP2 = 0;
int playersAmount = 0;

struct Position {
    int i;
    int j;
};

struct Player {
    struct Position vecPos[MAXDIM];
    int direction;
    char number;
    int alive;
    int length;
    int posX;
    int posY;
    Color playerColor;
};


void fillCell(char game[HEIGHT][WIDTH], int i, int j, char playerNumber) {
    game[i][j] = playerNumber;
}

void printBoard(char game[HEIGHT][WIDTH], struct Player *player) {
    Color currentColor;
    int i, j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (game[i][j] == player->number) {
                currentColor = player->playerColor;
                paintRectangle(j * PIXELWIDTH, i * PIXELHEIGHT, PIXELWIDTH - 1, PIXELHEIGHT - 1, currentColor);
            } 
        }
    }
}

void startGame(char game[HEIGHT][WIDTH], struct Player *player) {
    player->direction = PLAYER1_RIGHT;
    player->number = '1';
    player->playerColor = BLUE;
    player->posX = WIDTH / 2;
    player->posY = HEIGHT / 2;
    player->alive = 1;
    player->length = 5;

    game[player->posY][player->posX] = player->number;
    int i, j;
    for (i = 0; i < HEIGHT - 2; i++) {
        for (j = 0; j < WIDTH; j++) {
                game[i][j] = ' ';
                paintRectangle(j * PIXELWIDTH, i * PIXELHEIGHT, PIXELWIDTH - 1, PIXELHEIGHT - 1, BLACK);
        }
    }
}



void readKeyboardInput(struct Player *player, char s1, char s2, char s3, char s4) {
    char c;
    c = getChar();

    if (c == s1 && player->direction != s2) {
        player->direction = s1;
    } else if (c == s2 && player->direction != s1) {
        player->direction = s2;
    } else if (c == s3 && player->direction != s4) {
        player->direction = s3;
    } else if (c == s4 && player->direction != s3) {
        player->direction = s4;
    }
}


int getNextY(int dir, char s1, char s2, char s3, char s4){
    if (dir == s1) {
        return -1;
    } else if (dir == s2) {
        return 1;
    }else{
        return 0;
    }
}

int getNextX(int dir, char s1, char s2, char s3, char s4){
    if (dir == s3) {
        return -1;
    } else if (dir == s4) {
        return 1;
    }else{
        return 0;
    }
}


void gameLogic(char game[HEIGHT][WIDTH], struct Player * player, char s1, char s2, char s3, char s4){
                                                                    //up    down    left     right

    player->posY += getNextY(player-> direction,s1,s2,s3,s4);
    player->posX += getNextX(player-> direction,s1,s2,s3,s4);

    
    if(game[player->posY ][player->posX ] != ' '){
        player->alive = 0;
    }
    
    // Verifica si la moto choca con el borde de la pantalla
    if (player->posX < 0 || player->posX >= WIDTH  || player->posY < 0 || player->posY >= HEIGHT ) {
        player->alive = 0;
    }
    

    if (!player->alive) {
        gameover = 1;
        player->playerColor = BLACK;
        if(playersAmount == 2){
            if(player->number == '1'){
                scoreP2++;
            }else if(player->number == '2'){
                scoreP1++;
            }
        }
    }



    if (player->alive) {
        game[player->posY][player->posX] = player->number;
        fillCell(game, player->posY, player->posX, player->number);
    }
    
}

void logic(char game[HEIGHT][WIDTH], struct Player *player, char s1, char s2, char s3, char s4) {
    gameLogic(game,player,s1,s2,s3,s4);

    printBoard(game, player);
}

void eliminatorGame() {
    char game[HEIGHT][WIDTH];
    scoreP0 = 0;
    struct Player player;
    startGame(game, &player);
    gameover = 0;
    for(int i=0 ; i<41 ; i++){
        printString("\n", MAX_BUFFER);
    }
    printString("_________________________________________________________________________________________________________________\n", MAX_BUFFER);
    printString("Score:    ", MAX_BUFFER); 
    
    while (!gameover) {
        readKeyboardInput(&player,PLAYER1_UP,PLAYER1_DOWN,PLAYER1_LEFT,PLAYER1_RIGHT);
        logic(game, &player,PLAYER1_UP,PLAYER1_DOWN,PLAYER1_LEFT,PLAYER1_RIGHT);
        scoreP0++;
        int oldScoreDigits = 0;
        int aux = scoreP0;
        for(int aux = scoreP0; aux > 0; aux /= 10){
            oldScoreDigits++;
        }
        if(oldScoreDigits > 0){
            for(int i = 0; i < oldScoreDigits; i++){
                printChar('\b');
            }
        }
        if(scoreP0 % 50 == 0){
            scoreSound();
        }
        printDec(scoreP0);
        wait(100);
    }
    captureRegisters();

    deadSound();
    
    clear_scr();
    paintRectangle(0, 0, getScreenWidth() / 2, getScreenHeight() / 8, BLACK);
    printString("\nGame Over. Presione espacio para salir\n", MAX_BUFFER);
    while (getChar() != ' ') {
        continue;
    }

    clear_scr();
}

void scoreSound(){
    sys_playSound(2093);
    sys_playSound(1300);
}

void deadSound(){
    sys_playSound(2637);
    sys_playSound(2093);
    sys_playSound(1568);
    sys_playSound(1318);
    sys_playSound(1046);
    sys_playSound(784);
    sys_playSound(659);
    sys_playSound(523);
    sys_playSound(392);
}



/* MODO 2 JUGADORES */


struct Player player1;
struct Player player2;

void startGame2Players(char game[HEIGHT][WIDTH], struct Player *player1, struct Player *player2) {
    player1->direction = PLAYER1_DOWN;
    player1->number = '1';
    player1->playerColor = BLUE;
    player1->posX = WIDTH / 4;
    player1->posY = HEIGHT / 4;
    player1->alive = 1;
    player1->length = 2;

    player2->direction = PLAYER2_UP;
    player2->number = '2';
    player2->playerColor = ORANGE;
    player2->posX = 3 * WIDTH / 4;
    player2->posY = 3 * HEIGHT / 4;
    player2->alive = 1;
    player2->length = 2;

    game[player1->posY][player1->posX] = player1->number;
    game[player2->posY][player2->posX] = player2->number;

    // Inicializa el tablero
    int i, j;
    for (i = 0; i < HEIGHT - 2; i++) {
        for (j = 0; j < WIDTH; j++) {
            game[i][j] = ' ';
        }
    }

   
}

void logic2(char game[HEIGHT][WIDTH], struct Player *player,char s1,char s2,char s3,char s4) {
    gameLogic(game,player,s1,s2,s3,s4);
}



void drawBoard2(char game[HEIGHT][WIDTH], struct Player *player1, struct Player *player2) {
    Color currentColor;
    int i, j;
    for (i = 0; i < HEIGHT - 2; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (game[i][j] == ' ') {
                currentColor = BLACK;
            } else if (game[i][j] == player1->number) {
                currentColor = player1->playerColor;
            } else if (game[i][j] == player2->number) {
                currentColor = player2->playerColor;
            } 
            paintRectangle(j * PIXELWIDTH, i * PIXELHEIGHT, PIXELWIDTH - 1, PIXELHEIGHT - 1, currentColor);
        }
    }
}

void eliminatorGame2Players() {
    char game[HEIGHT][WIDTH];
    startGame2Players(game, &player1, &player2);
    gameover = 0;

    while (!gameover) {

        readKeyboardInput(&player1, PLAYER1_UP, PLAYER1_DOWN, PLAYER1_LEFT, PLAYER1_RIGHT);
        logic2(game, &player1, PLAYER1_UP, PLAYER1_DOWN, PLAYER1_LEFT, PLAYER1_RIGHT);

        readKeyboardInput(&player2, PLAYER2_UP, PLAYER2_DOWN, PLAYER2_LEFT, PLAYER2_RIGHT);
        logic2(game, &player2, PLAYER2_UP, PLAYER2_DOWN, PLAYER2_LEFT, PLAYER2_RIGHT);

        drawBoard2(game, &player1, &player2);
        wait(100);

    }
    captureRegisters();
    deadSound();
    paintRectangle(0, 0, getScreenWidth() / 2, getScreenHeight() / 8, BLACK);
    printString("\nGame Over. Presione espacio para salir\n", MAX_BUFFER);
    while (getChar() != ' ') {
        continue;
    }

    clear_scr();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int startEliminator(int option) {
    clear_scr();

    if (option == 1) {
        printString("\nModo 1 jugador\n", MAX_BUFFER);
        eliminatorGame();
    } else if (option == 2) {
        printString("\nModo 2 jugadores\n", MAX_BUFFER);
        eliminatorGame2Players();
    } else {
        return 0;
    }
    
    return 1;
}
