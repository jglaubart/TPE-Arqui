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

// Fixed sizes for player components
#define PLAYER_RADIUS 0.05    // circle radius in screen fractions
#define ARROW_WIDTH 0.1       // arrow width in screen fractions  
#define ARROW_HEIGHT 0.025    // arrow height in screen fractions

// Fixed sizes for ball components
#define BALL_RADIUS 0.02      // ball radius in screen fractions (smaller than player)

/* typedef struct{
    physicsEntity * entity;
} player; */

typedef struct {
    char forward;   // e.g., 'w' for player 1
    char left;      // e.g., 'a' for player 1
    char right;     // e.g., 'd' for player 1
    char backward;  // e.g., 's' for player 1
} PlayerControls;

typedef struct {
    physicsEntity entity;    // The physics entity (circle + arrow)
    PlayerControls controls; // Input controls for this player
    int playerId;           // Unique identifier for the player
    int score;              // Player's current score
} Player;

void pongisInit();
void pongisDraw();
void pongisUpdate();
void checkCollisions();
void updateEntities();
void drawEntities();
Player createPlayer(vec2d center, uint32_t circleColor, uint32_t arrowColor, PlayerControls controls, int playerId, Figure* playerFig, Figure* arrowFig);
physicsEntity* createBall(vec2d center, uint32_t color, Figure* ballFig, physicsEntity* ballEntity);
void addEntity(physicsEntity* entity);

// Player management functions
void addPlayer(Player player);
void handlePlayerInput(Player* player);
Player* getPlayerById(int playerId);

#endif // PONGIS_H