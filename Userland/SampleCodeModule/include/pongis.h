#ifndef PONGIS_H
#define PONGIS_H
#include <physicsEntity.h>
#include <stdint.h>

#define ACCEL_MAG   4.0    // acceleration per input
#define TIME_STEP   0.05    // seconds per frame
#define RIGHT_ROTATION 0.1
#define LEFT_ROTATION -RIGHT_ROTATION
#define RESTITUTION 1.0

#define MAX_ENTITIES 10
#define MAX_HOLES 4     // Maximum number of holes per level

// Fixed sizes for player components
#define PLAYER_RADIUS 0.05    // circle radius in screen fractions
#define ARROW_WIDTH 0.075       // arrow width in screen fractions  
#define ARROW_HEIGHT 0.02    // arrow height in screen fractions

// Fixed sizes for ball components
#define BALL_RADIUS 0.02      // ball radius in screen fractions (smaller than player)

// Note: Hole radius is not fixed - it varies by level and is passed as parameter to createHole()

// Player and ball color definitions (consistent across all levels)
#define PLAYER1_COLOR 0xFF0000    // Red for Player 1
#define PLAYER2_COLOR 0x00FF00    // Green for Player 2
#define ARROW_COLOR 0x0000FF      // Blue arrows for all levels (consistent)

/* typedef struct{
    physicsEntity * entity;
} player; */

typedef struct {
    char forward;
    char left;
    char right;
    char backward;
} PlayerControls;

typedef struct {
    physicsEntity entity;
    PlayerControls controls;
    int playerId;
} Player;

typedef struct {
    physicsEntity * ballEntity;
    int playerId;
} Ball;
typedef struct {
    vec2d position;
    double radius;
    uint32_t color;
    Figure* borderFigure;
    Figure* holeFigure;
    physicsEntity holeEntity;  // Physics entity for penetration calculations
} Hole;


void pongisInit();
void finishPongis();
void pongisDraw();
void pongisUpdate();
void checkCollisions();
void updateEntities();
void drawEntities();
Player createPlayer(vec2d center, uint32_t circleColor, uint32_t arrowColor, PlayerControls controls, int playerId, Figure* playerFig, Figure* arrowFig);
physicsEntity* createBall(vec2d center, uint32_t color, Figure* ballFig, physicsEntity* ballEntity);
Hole* createHole(vec2d position, double radius, Figure* borderFig, Figure* holeFig);
int64_t checkGoal(Hole* hole, physicsEntity* ball);
void addEntity(physicsEntity* entity);

// Player management functions
void addPlayer(Player player);
void handlePlayerInput(Player* player);
Player* getPlayerById(int playerId);

// Level management functions  
void setLevel1();
void setLevel2();
void setLevel3();
void drawHoles();
int checkAllGoals();
void printWinner(int winningPlayerId);

#endif // PONGIS_H