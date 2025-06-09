#include "pongis.h"
#include <usr_stdlib.h>
#include <syscalls.h>
#include "figures.h"
#include "physicsEntity.h"

static physicsEntity *entities[MAX_ENTITIES];
int entityCount = 0;

// Global storage for players
static Player players[MAX_ENTITIES];
static int playerCount = 0;

Player createPlayer(vec2d center, uint32_t circleColor, uint32_t arrowColor, PlayerControls controls, int playerId, Figure* playerFig, Figure* arrowFig) {
    // Create the circle figure (player body)
    vec2d circleTopLeft = {center.x - PLAYER_RADIUS, center.y - PLAYER_RADIUS};
    vec2d circleBottomRight = {center.x + PLAYER_RADIUS, center.y + PLAYER_RADIUS};
    newCircle(playerFig, circleTopLeft, circleBottomRight, circleColor);
    
    // Create the arrow figure (direction indicator)
    vec2d arrowTopLeft = {center.x + PLAYER_RADIUS, center.y - ARROW_HEIGHT/2};
    vec2d arrowBottomRight = {center.x + PLAYER_RADIUS + ARROW_WIDTH, center.y + ARROW_HEIGHT/2};
    newRectangle(arrowFig, arrowTopLeft, arrowBottomRight, arrowColor);
    
    // Set rotation center for arrow to be same as player center
    setRotationCenter(arrowFig, center);
    
    // Create the Player struct
    Player player;
    
    // Initialize the physics entity with the provided figures
    // We need both figures for shapes but only the circle for collision
    Figure *shapes[] = {playerFig, arrowFig};
    Figure *colliders[] = {playerFig};
    initPhysicsEntity(&player.entity, shapes, 2, colliders, 1, ENTITY_HEAVY, 0.25, 1);
    
    // Set player properties
    player.controls = controls;
    player.playerId = playerId;
    player.score = 0;
    
    return player;
}

physicsEntity* createBall(vec2d center, uint32_t color, Figure* ballFig, physicsEntity* ballEntity) {
    // Create the ball figure (small circle)
    vec2d ballTopLeft = {center.x - BALL_RADIUS, center.y - BALL_RADIUS};
    vec2d ballBottomRight = {center.x + BALL_RADIUS, center.y + BALL_RADIUS};
    newCircle(ballFig, ballTopLeft, ballBottomRight, color);
    
    // Initialize the physics entity with the ball figure
    Figure *shapes[] = {ballFig};
    Figure *colliders[] = {ballFig};
    initPhysicsEntity(ballEntity, shapes, 1, colliders, 1, ENTITY_LIGHT, 0.1, 1);
    
    return ballEntity;
}

// Helper function to add any entity to the global entities array
void addEntity(physicsEntity* entity) {
    if (entityCount < MAX_ENTITIES && entity) {
        entities[entityCount] = entity;
        entityCount++;
    }
}

// Player management functions
void addPlayer(Player player) {
    if (playerCount < MAX_ENTITIES) {
        players[playerCount] = player;
        
        // Add the physics entity to the global entities array for collision detection
        addEntity(&players[playerCount].entity);
        
        playerCount++;
    }
}

void handlePlayerInput(Player* player) {
    if (!player) return;
    
    vec2d accel = {0, 0};
    
    // Check for movement input
    if (isPressed(player->controls.forward) || isPressed(player->controls.forward - 32)) { // Handle both cases
        accel.x = cos_taylor(player->entity.angle) * ACCEL_MAG;
        accel.y = sin_taylor(player->entity.angle) * ACCEL_MAG;
    }
    if (isPressed(player->controls.backward) || isPressed(player->controls.backward - 32)) {
        accel.x = -cos_taylor(player->entity.angle) * ACCEL_MAG;
        accel.y = -sin_taylor(player->entity.angle) * ACCEL_MAG;
    }
    if (isPressed(player->controls.left) || isPressed(player->controls.left - 32)) {
        rotateEntity(&player->entity, LEFT_ROTATION);
    }
    if (isPressed(player->controls.right) || isPressed(player->controls.right - 32)) {
        rotateEntity(&player->entity, RIGHT_ROTATION);
    }
    
    setAcceleration(&player->entity, accel);
}

Player* getPlayerById(int playerId) {
    for (int i = 0; i < playerCount; i++) {
        if (players[i].playerId == playerId) {
            return &players[i];
        }
    }
    return 0; // Player not found
}

void pongisInit(){
    setDrawBuffer(BACK_BUFFER);

    // Example: Creating a single player using the new system
    Figure playerFig, arrowFig;
    PlayerControls singlePlayerControls = {'w', 'a', 'd', 's'};
    Player singlePlayer = createPlayer((vec2d){0.5, 0.5}, 0x0000FF, 0xFF0000, singlePlayerControls, 1, &playerFig, &arrowFig);
    addPlayer(singlePlayer);

    // Add a light ball for testing using the new createBall function
    Figure ballFig;
    physicsEntity ball;
    createBall((vec2d){0.3, 0.3}, 0x00FF00, &ballFig, &ball);
    addEntity(&ball);

    while (1) {
        clearScreen();
        
        // Handle input for the player
        handlePlayerInput(&players[0]);
        
        // Update physics for all entities
        checkCollisions(); // Handle collisions between all entities
        updateEntities();
        drawEntities();

        if (isPressed('q') || isPressed('Q')) {
            setDrawBuffer(FRONT_BUFFER);
            clearScreen();
            myprintf("Exiting pongis...\n");
            return;   
        }
        
        showBackBuffer();
    }
}

void checkCollisions() {
    for (int i = 0; i < entityCount; i++) {
        for (int j = i + 1; j < entityCount; j++) {
            resolveCollisionSimple(entities[i], entities[j], RESTITUTION);
        }
    }
}

void updateEntities() {
    for (int i = 0; i < entityCount; i++) {
        updatePhysicsEntity(entities[i], TIME_STEP);
        setAcceleration(entities[i], (vec2d){0, 0}); // Reset acceleration after update
        drawPhysicsEntity(entities[i]);
    }
}

void drawEntities() {
    for (int i = 0; i < entityCount; i++) {
        drawPhysicsEntity(entities[i]);
    }
}