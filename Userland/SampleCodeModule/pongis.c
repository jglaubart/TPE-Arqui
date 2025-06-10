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

// Global storage for holes
static Hole holes[MAX_HOLES];
static Figure holeFigures[MAX_HOLES];      // Black inner figures
static Figure borderFigures[MAX_HOLES];    // White border figures
static int holeCount = 0;

// Global storage for balls (separate from entities for goal checking)
static physicsEntity balls[2]; // Max 2 balls (one per player)
static Figure ballFigures[2];
static int ballCount = 0;

// Game state variables
static int currentLevel = 1;
static int levelComplete = 0;

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

Hole* createHole(vec2d position, double radius, Figure* borderFig, Figure* holeFig) {
    // Note: Hole radius is configurable to allow different difficulty levels
    // Smaller holes = harder level, larger holes = easier level
    if (holeCount >= MAX_HOLES) return 0; // No more holes can be created
    
    // Create white border circle (slightly larger than the hole)
    double borderRadius = radius + 0.005; // Border is 0.005 units wider
    uint32_t borderColor = 0xFFFFFF; // White color for the border
    vec2d borderTopLeft = {position.x - borderRadius, position.y - borderRadius};
    vec2d borderBottomRight = {position.x + borderRadius, position.y + borderRadius};
    newCircle(borderFig, borderTopLeft, borderBottomRight, borderColor);
    
    // Create black hole circle (the actual hole)
    uint32_t holeColor = 0x000000; // Black color for the hole
    vec2d holeTopLeft = {position.x - radius, position.y - radius};
    vec2d holeBottomRight = {position.x + radius, position.y + radius};
    newCircle(holeFig, holeTopLeft, holeBottomRight, holeColor);
    
    // Use the global holes array
    holes[holeCount].color = holeColor;
    holes[holeCount].position = position;
    holes[holeCount].radius = radius;
    holes[holeCount].borderFigure = borderFig;
    holes[holeCount].holeFigure = holeFig;
    
    holeCount++;
    return &holes[holeCount - 1];
}

uint64_t isInGoalPosition(vec2d holePosition, double radius, vec2d ballPosition) {
    // Calculate the distance from the ball to the hole center
    double dx = ballPosition.x - holePosition.x;
    double dy = ballPosition.y - holePosition.y;
    double distanceSquared = dx * dx + dy * dy;
    
    // Check if the distance is less than or equal to the radius squared
    return (distanceSquared <= radius * radius);
}

int64_t checkGoal(Hole* hole, physicsEntity* ball) {
    if(!ball || !hole || !hole->holeFigure){
        return -1;
    }

    // Check if the ball is within the hole's radius
    if(isInGoalPosition(hole->position, hole->radius, ball->position)) {
        // Ball is in the hole
        return 1; // Goal scored
    }
    
    return 0; // No goal    
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

void setLevel1() {
    // Reset all arrays
    entityCount = 0;
    playerCount = 0;
    holeCount = 0;
    ballCount = 0;
    
    // Create Player 1 (left side)
    static Figure player1Fig, arrow1Fig;
    PlayerControls player1Controls = {'w', 'a', 'd', 's'};
    Player player1 = createPlayer((vec2d){0.2, 0.5}, 0xFF0000, 0x0000FF, player1Controls, 1, &player1Fig, &arrow1Fig);
    addPlayer(player1);
    
    // Create Player 2 (right side)
    static Figure player2Fig, arrow2Fig;
    PlayerControls player2Controls = {'i', 'j', 'l', 'k'};
    Player player2 = createPlayer((vec2d){0.8, 0.5}, 0x00FF00, 0x0000FF, player2Controls, 2, &player2Fig, &arrow2Fig);
    addPlayer(player2);
    
    // Create Ball 1 (for Player 1)
    createBall((vec2d){0.3, 0.3},  0xFF0000, &ballFigures[0], &balls[0]);
    addEntity(&balls[0]);
    ballCount++;
    
    // Create Ball 2 (for Player 2)  
    createBall((vec2d){0.7, 0.7}, 0x00FF00, &ballFigures[1], &balls[1]);
    addEntity(&balls[1]);
    ballCount++;
    
    // Create single hole - Level 1 has one hole in the center
    double level1HoleRadius = 0.035; // Medium difficulty
    createHole((vec2d){0.5, 0.5}, level1HoleRadius, &borderFigures[0], &holeFigures[0]);
}

void setLevel2() {
    // Reset all arrays
    entityCount = 0;
    playerCount = 0;
    holeCount = 0;
    ballCount = 0;
    
    // Create Player 1 (left side)
    static Figure player1Fig, arrow1Fig;
    PlayerControls player1Controls = {'w', 'a', 'd', 's'};
    Player player1 = createPlayer((vec2d){0.15, 0.5}, 0x0000FF, 0xFF0000, player1Controls, 1, &player1Fig, &arrow1Fig);
    addPlayer(player1);
    
    // Create Player 2 (right side)
    static Figure player2Fig, arrow2Fig;
    PlayerControls player2Controls = {'i', 'j', 'l', 'k'};
    Player player2 = createPlayer((vec2d){0.85, 0.5}, 0xFF0000, 0x0000FF, player2Controls, 2, &player2Fig, &arrow2Fig);
    addPlayer(player2);
    
    // Create Ball 1 (for Player 1)
    createBall((vec2d){0.25, 0.3}, 0x00FF00, &ballFigures[0], &balls[0]);
    addEntity(&balls[0]);
    ballCount++;
    
    // Create Ball 2 (for Player 2)  
    createBall((vec2d){0.75, 0.7}, 0x00FF00, &ballFigures[1], &balls[1]);
    addEntity(&balls[1]);
    ballCount++;
    
    // Create single hole - Level 2 has one hole off-center (harder)
    double level2HoleRadius = 0.03; // Smaller hole = harder difficulty
    createHole((vec2d){0.6, 0.3}, level2HoleRadius, &borderFigures[0], &holeFigures[0]);
}

void drawHoles() {
    for (int i = 0; i < holeCount; i++) {
        // Draw border first (underneath), then hole on top
        drawFigure(holes[i].borderFigure);
        drawFigure(holes[i].holeFigure);
    }
}

void checkAllGoals() {
    for (int i = 0; i < ballCount; i++) {
        for (int j = 0; j < holeCount; j++) {
            int64_t goalResult = checkGoal(&holes[j], &balls[i]);
            if (goalResult == 1) {
                // Determine which player scored based on which ball went in
                int winningPlayerId = i + 1; // Ball 0 = Player 1, Ball 1 = Player 2
                Player* winningPlayer = getPlayerById(winningPlayerId);
                
                if (winningPlayer) {
                    myprintf("LEVEL %d COMPLETE!\n", currentLevel);
                    myprintf("Player %d WINS with ball in hole!\n", winningPlayerId);
                    
                    // Mark level as complete
                    levelComplete = 1;
                }
                return; // Exit immediately when someone scores
            }
        }
    }
}

void pongisInit(){
    setDrawBuffer(BACK_BUFFER);

    // Main game loop - continues until user quits
    while (1) {
        // Set up the current level before starting the frame loop
        levelComplete = 0; // Reset level completion flag
        
        // Load the appropriate level
        if (currentLevel == 1) {
            myprintf("Starting Level 1...\n");
            setLevel1();
        } else if (currentLevel == 2) {
            myprintf("Starting Level 2...\n");
            setLevel2();
        } else {
            // For levels 3+, cycle back to level 1 for now
            myprintf("Starting Level %d (using Level 1 layout)...\n", currentLevel);
            setLevel1();
        }

        // Frame loop - continues until level is complete
        while (!levelComplete) {
            clearScreen();
            
            // Draw holes first (so they appear underneath other entities)
            drawHoles();
            
            // Handle input for both players
            handlePlayerInput(&players[0]); // Player 1
            if (playerCount > 1) {
                handlePlayerInput(&players[1]); // Player 2
            }
            
            // Update physics for all entities
            checkCollisions(); // Handle collisions between all entities
            updateEntities();
            
            // Draw all entities (players and balls)
            drawEntities();
            
            // Check for goals
            checkAllGoals();

            // Check for quit input
            if (isPressed('q') || isPressed('Q')) {
                setDrawBuffer(FRONT_BUFFER);
                clearScreen();
                myprintf("Exiting pongis...\n");
                return;   
            }
            
            showBackBuffer();
        }
        
        // Level completed - wait for user input to continue or quit
        myprintf("Press SPACE to continue to next level or Q to quit...\n");
        
        // Wait for player input to continue
        int waitingForInput = 1;
        while (waitingForInput) {
            if (isPressed(' ')) {
                currentLevel++;
                waitingForInput = 0; // Exit waiting loop to start next level
            } else if (isPressed('q') || isPressed('Q')) {
                setDrawBuffer(FRONT_BUFFER);
                clearScreen();
                myprintf("Exiting pongis...\n");
                return;
            }
        }
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