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
static Figure holeHitboxes[MAX_HOLES];     // Hitbox figures for penetration calculations
static int holeCount = 0;

// Global storage for balls (separate from entities for goal checking)
static Ball balls[2]; // Max 2 balls (one per player)
static Figure ballFigures[2];
static physicsEntity ballEntities[2]; // Physical entities for the balls
static int ballCount = 0;

// Game state variables
static int currentLevel = 1;
static int levelComplete = 0;
static int numPlayers = 2; // Number of players (1 or 2)

// Function pointer array for level setters (1-indexed, so index 0 is unused)
static void (*levelSetters[4])() = {NULL, setLevel1, setLevel2, setLevel3};
#define MAX_LEVELS 3


#define MENU_BG_COLOR 0x24892F      // Verde oscuro
#define GAME_BG_COLOR 0x228B22      // Verde
#define MENU_RECT_COLOR 0xFFFDD0    // Crema
#define MENU_TEXT_COLOR 0x083E13    // Negro

void showGolfMenu() {
    // Limpiar fondo verde
    setDrawBuffer(BACK_BUFFER);
    changeBackgroundColor(MENU_BG_COLOR);
    
    changeFontSize(4);
    putsColored("Mini Pongis Golf\n",0xFFD700 );
    changeFontSize(2);
    putsColored("Selecciona el modo de juego:\n", MENU_TEXT_COLOR);
    putsColored("1 - Un jugador\n", MENU_TEXT_COLOR);
    putsColored("2 - Multijugador\n", MENU_TEXT_COLOR);
    putsColored("Q - Salir\n", MENU_TEXT_COLOR);
    changeFontSize(0);


    showBackBuffer();

    // Esperar input
    while (1) {
        if (isPressed('1')) {
            // Un jugador
            numPlayers = 1;
            break;
        }
        if (isPressed('2')) {
            // Multijugador
            numPlayers = 2;
            break;
        }
        if (isPressed('q') || isPressed('Q')) {
            finishPongis();
            myprintf("Saliendo de Pongis Golf...\n");
            return;
        }
    }
    changeBackgroundColor(GAME_BG_COLOR);
}

void showLevelCompleteMenu(int winningPlayerId) {
    // Limpiar fondo verde
    setDrawBuffer(BACK_BUFFER);
    changeBackgroundColor(MENU_BG_COLOR);
    
    changeFontSize(4);
    putsColored("Nivel completado!\n", 0xFFD700);
    changeFontSize(2);
    if(numPlayers > 1) {
        myprintf("El jugador %d gano el nivel!\n", winningPlayerId);
    }
    myprintf("Presione ESPACIO para continuar!\n");
    myprintf("Presione Q para salir\n");
    changeFontSize(0);



    showBackBuffer();

    // Wait for player input to continue
    int waitingForInput = 1;
    while (waitingForInput) {
        if (isPressed(' ')) {
            if(currentLevel == MAX_LEVELS) {
                clearScreen();
                showEndMenu();
                showBackBuffer();
                return;
            }
            currentLevel++;
            waitingForInput = 0;
        } else if (isPressed('q') || isPressed('Q')) {
            finishPongis();
            return;
        }
    }
    changeBackgroundColor(GAME_BG_COLOR);
}

void showEndMenu(){
    // Clear the screen with the menu background color
    setDrawBuffer(BACK_BUFFER);
    changeBackgroundColor(MENU_BG_COLOR);
    
    changeFontSize(4);
    putsColored("Gracias por jugar Pongis Golf!\n", 0xFFD700);
    changeFontSize(2);
    putsColored("Presione ESPACIO para volver al nivel 1\n", MENU_TEXT_COLOR);
    putsColored("Presione Q para salir\n", MENU_TEXT_COLOR);
    changeFontSize(0);
    showBackBuffer();

    // Wait for player to press 'Q' to exit
    
    
    finishPongis();

    // Wait for restart or quit
            int waitingForInput = 1;
            while (waitingForInput) {
                if (isPressed(' ')) {
                    currentLevel = 1; // Restart from level 1
                    waitingForInput = 0;
                } else if (isPressed('q') || isPressed('Q')) {
                    finishPongis();
                    return;
                }
            }
}




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
    // We need both figures for shapes but only the circle for hitbox
    Figure *shapes[] = {playerFig, arrowFig};
    initPhysicsEntity(&player.entity, shapes, 2, playerFig, ENTITY_HEAVY, 0.25, 1);
    
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
    initPhysicsEntity(ballEntity, shapes, 1, ballFig, ENTITY_LIGHT, 0.1, 1);
    
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
    
    // Create hitbox figure for penetration calculations
    Figure* holeHitbox = &holeHitboxes[holeCount];
    newCircle(holeHitbox, holeTopLeft, holeBottomRight, holeColor);
    
    // Use the global holes array
    holes[holeCount].color = holeColor;
    holes[holeCount].position = position;
    holes[holeCount].radius = radius;
    holes[holeCount].borderFigure = borderFig;
    holes[holeCount].holeFigure = holeFig;
    
    // Initialize the hole's physics entity (non-collidable but with hitbox for penetration)
    Figure *shapes[] = {borderFig, holeFig};
    initPhysicsEntity(&holes[holeCount].holeEntity, shapes, 2, holeHitbox, ENTITY_HEAVY, 0.0, 0);
    holes[holeCount].holeEntity.position = position;
    
    holeCount++;
    return &holes[holeCount - 1];
}

int64_t checkGoal(Hole* hole, physicsEntity* ball) {
    if(!ball || !hole || !hole->holeFigure){
        return -1;
    }

    // Use the hole's physics entity for penetration calculation
    double penetration = calculatePenetrationDepth(ball, &hole->holeEntity);
    
    // Ball scores if it's significantly inside the hole (> 50% penetration)
    double ballRadius = (ball->hitbox->bottomRight.x - ball->hitbox->topLeft.x) * 0.5;
    double minPenetrationForGoal = ballRadius * 0.5; // Ball must be at least 50% inside
    
    return (penetration >= minPenetrationForGoal) ? 1 : 0;
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
    
    // Use consistent player and ball colors
    uint32_t player1Color = PLAYER1_COLOR; // Red
    uint32_t ball1Color = PLAYER1_COLOR;   // Red (same as player)
    
    uint32_t player2Color = PLAYER2_COLOR; // Green
    uint32_t ball2Color = PLAYER2_COLOR;   // Green (same as player)
    
    // Arrow color for Level 1 (consistent across all levels)
    uint32_t arrowColor = ARROW_COLOR; // Blue
    
    // Create Player 1 (left side)
    static Figure player1Fig, arrow1Fig;
    PlayerControls player1Controls = {'w', 'a', 'd', 's'};
    Player player1 = createPlayer((vec2d){0.2, 0.5}, player1Color, arrowColor, player1Controls, 1, &player1Fig, &arrow1Fig);
    addPlayer(player1);
    
    // Create Player 2 only if multiplayer mode
    if (numPlayers == 2) {
        static Figure player2Fig, arrow2Fig;
        PlayerControls player2Controls = {'i', 'j', 'l', 'k'};
        Player player2 = createPlayer((vec2d){0.8, 0.5}, player2Color, arrowColor, player2Controls, 2, &player2Fig, &arrow2Fig);
        addPlayer(player2);
    }
    
    // Create Ball 1 (for Player 1) - Same color as Player 1
    createBall((vec2d){0.3, 0.3}, ball1Color, &ballFigures[0], &ballEntities[0]);
    balls[0].ballEntity = &ballEntities[0];
    balls[0].playerId = 1;
    addEntity(&ballEntities[0]);
    ballCount++;
    
    // Create Ball 2 only if multiplayer mode
    if (numPlayers == 2) {
        createBall((vec2d){0.7, 0.7}, ball2Color, &ballFigures[1], &ballEntities[1]);
        balls[1].ballEntity = &ballEntities[1];
        balls[1].playerId = 2;
        addEntity(&ballEntities[1]);
        ballCount++;
    }
    
    // Create single hole - Level 1 has one hole in the center (largest/easiest)
    double level1HoleRadius = 0.035; // Large hole = easiest difficulty
    createHole((vec2d){0.5, 0.5}, level1HoleRadius, &borderFigures[0], &holeFigures[0]);
}

void setLevel2() {
    // Reset all arrays
    entityCount = 0;
    playerCount = 0;
    holeCount = 0;
    ballCount = 0;
    
    // Use consistent player and ball colors (same as Level 1)
    uint32_t player1Color = PLAYER1_COLOR; // Red
    uint32_t ball1Color = PLAYER1_COLOR;   // Red (same as player)
    
    uint32_t player2Color = PLAYER2_COLOR; // Green
    uint32_t ball2Color = PLAYER2_COLOR;   // Green (same as player)
    
    // Arrow color for Level 2 (consistent across all levels)
    uint32_t arrowColor = ARROW_COLOR; // Blue
    
    // Create Player 1 (left side)
    static Figure player1Fig, arrow1Fig;
    PlayerControls player1Controls = {'w', 'a', 'd', 's'};
    Player player1 = createPlayer((vec2d){0.15, 0.5}, player1Color, arrowColor, player1Controls, 1, &player1Fig, &arrow1Fig);
    addPlayer(player1);
    
    // Create Player 2 only if multiplayer mode
    if (numPlayers == 2) {
        static Figure player2Fig, arrow2Fig;
        PlayerControls player2Controls = {'i', 'j', 'l', 'k'};
        Player player2 = createPlayer((vec2d){0.85, 0.5}, player2Color, arrowColor, player2Controls, 2, &player2Fig, &arrow2Fig);
        addPlayer(player2);
    }
    
    // Create Ball 1 (for Player 1) - Same color as Player 1
    createBall((vec2d){0.25, 0.3}, ball1Color, &ballFigures[0], &ballEntities[0]);
    balls[0].ballEntity = &ballEntities[0];
    balls[0].playerId = 1;
    addEntity(&ballEntities[0]);
    ballCount++;
    
    // Create Ball 2 only if multiplayer mode
    if (numPlayers == 2) {
        createBall((vec2d){0.75, 0.7}, ball2Color, &ballFigures[1], &ballEntities[1]);
        balls[1].ballEntity = &ballEntities[1];
        balls[1].playerId = 2;
        addEntity(&ballEntities[1]);
        ballCount++;
    }
    
    // Create single hole - Level 2 has one hole off-center (medium difficulty)
    double level2HoleRadius = 0.03; // Medium hole = medium difficulty
    createHole((vec2d){0.6, 0.3}, level2HoleRadius, &borderFigures[0], &holeFigures[0]);
}

void setLevel3() {
    // Reset all arrays
    entityCount = 0;
    playerCount = 0;
    holeCount = 0;
    ballCount = 0;
    
    // Use consistent player and ball colors (same as previous levels)
    uint32_t player1Color = PLAYER1_COLOR; // Red
    uint32_t ball1Color = PLAYER1_COLOR;   // Red (same as player)
    
    uint32_t player2Color = PLAYER2_COLOR; // Green
    uint32_t ball2Color = PLAYER2_COLOR;   // Green (same as player)
    
    // Arrow color for Level 3 (consistent across all levels)
    uint32_t arrowColor = ARROW_COLOR; // Blue
    
    // Create Player 1 (left side, more challenging starting position)
    static Figure player1Fig, arrow1Fig;
    PlayerControls player1Controls = {'w', 'a', 'd', 's'};
    Player player1 = createPlayer((vec2d){0.1, 0.2}, player1Color, arrowColor, player1Controls, 1, &player1Fig, &arrow1Fig);
    addPlayer(player1);
    
    // Create Player 2 only if multiplayer mode
    if (numPlayers == 2) {
        static Figure player2Fig, arrow2Fig;
        PlayerControls player2Controls = {'i', 'j', 'l', 'k'};
        Player player2 = createPlayer((vec2d){0.9, 0.8}, player2Color, arrowColor, player2Controls, 2, &player2Fig, &arrow2Fig);
        addPlayer(player2);
    }
    
    // Create Ball 1 (for Player 1) - Same color as Player 1
    createBall((vec2d){0.2, 0.15}, ball1Color, &ballFigures[0], &ballEntities[0]);
    balls[0].ballEntity = &ballEntities[0];
    balls[0].playerId = 1;
    addEntity(&ballEntities[0]);
    ballCount++;
    
    // Create Ball 2 only if multiplayer mode
    if (numPlayers == 2) {
        createBall((vec2d){0.8, 0.85}, ball2Color, &ballFigures[1], &ballEntities[1]);
        balls[1].ballEntity = &ballEntities[1];
        balls[1].playerId = 2;
        addEntity(&ballEntities[1]);
        ballCount++;
    }
    
    // Create single hole - Level 3 has one small hole (hardest difficulty)
    double level3HoleRadius = 0.025; // Small hole = hardest difficulty
    createHole((vec2d){0.4, 0.7}, level3HoleRadius, &borderFigures[0], &holeFigures[0]);
}

void drawHoles() {
    for (int i = 0; i < holeCount; i++) {
        // Draw border first (underneath), then hole on top
        drawFigure(holes[i].borderFigure);
        drawFigure(holes[i].holeFigure);
    }
}

int checkAllGoals() {
    for (int i = 0; i < ballCount; i++) {
        for (int j = 0; j < holeCount; j++) {
            int64_t goalResult = checkGoal(&holes[j], balls[i].ballEntity);
            if (goalResult == 1) {
                // Return the player ID of the ball that went in the hole
                return balls[i].playerId;
            }
        }
    }
    return 0; // No goal scored
}

void pongisInit(){
    setDrawBuffer(BACK_BUFFER);
    showGolfMenu();
    while (currentLevel <= MAX_LEVELS) {
        levelComplete = 0;
        
        levelSetters[currentLevel]();

        int winningPlayerId = 0;
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
            winningPlayerId = checkAllGoals();
            if (winningPlayerId > 0) {
                levelComplete = 1;
            }

            // Check for quit input
            if (isPressed('q') || isPressed('Q')) {
                finishPongis();
                myprintf("Exiting pongis...\n");
                return;   
            }
            
            showBackBuffer();
        }
        
        // Level completed - wait for user input to continue or quit
        clearScreen();
        showLevelCompleteMenu(winningPlayerId);
        showBackBuffer();
    }
}

void checkCollisions() {
    for (int i = 0; i < entityCount; i++) {
        for (int j = i + 1; j < entityCount; j++) {
            if(resolveCollisionSimple(entities[i], entities[j], RESTITUTION)){
                beep(500, 100);
            }
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

void finishPongis() {
    setDrawBuffer(FRONT_BUFFER);
    changeBackgroundColor(0x000000);  // Black background
    changeFontSize(0);
    clearScreen();
}