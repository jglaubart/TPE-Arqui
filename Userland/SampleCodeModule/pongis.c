#include "pongis.h"
#include <usr_stdlib.h>
#include "figures.h"
#include "physicsEntity.h"

#define ACCEL_MAG   4.0    // acceleration per input
#define TIME_STEP   0.05    // seconds per frame
#define RIGHT_ROTATION 0.1
#define LEFT_ROTATION -RIGHT_ROTATION

void pongisInit(){
    Figure flechita;
    vec2d topLeft2 = {0.3, 0.2375};
    vec2d bottomRight2 = {0.4, 0.2875};
    newRectangle(&flechita, topLeft2, bottomRight2, 0xFF0000);
    setRotationCenter(&flechita, (vec2d){0.275, 0.275}); // Set rotation center for rect2
    Figure jugador;
    vec2d topLeft3 = {0.25, 0.25};
    vec2d bottomRight3 = {0.3, 0.3};
    newCircle(&jugador, topLeft3, bottomRight3, 0x0000FF);
    Figure *figuresp[] = {&flechita, &jugador};
    /* while(1){
        clearScreen();
        for (int i = 0; i < 2; i++) {
            drawFigure(figuresp[i]);
        }

        // Check for user input using getchar (non-blocking)
        int ch = getChar();
        if (ch == 'w') {
            // flechita direction
            double angle = figuresp[1]->angle; // Assuming Figure has an 'angle' field
            vec2d dir = {0.02 * cos_taylor(angle), 0.02 * sin_taylor(angle)};
            moveFigure(&flechita, dir); // Move flechita

            // jugador direction (assuming same angle as flechita)
            moveFigure(&jugador, dir); // Move jugador
        }
        else if(ch == 'a'){
            rotateFigure(&flechita, -0.1); // Rotate flechita left
            rotateFigure(&jugador, -0.1); // Rotate jugador left
        }
        else if(ch == 'd'){
            rotateFigure(&flechita, 0.1); // Rotate flechita right
            rotateFigure(&jugador, 0.1); // Rotate jugador right
        }
        else if(ch == 's'){
            vec2d dir = {0.02 * cos_taylor(figuresp[1]->angle), 0.02 * sin_taylor(figuresp[1]->angle)};
            moveFigure(&flechita, (vec2d){-dir.x, -dir.y}); // Move flechita back
            moveFigure(&jugador, (vec2d){-dir.x, -dir.y}); // Move jugador back
        }
        else if(ch == 'q'){
            break; // Exit the loop if 'q' is pressed
        }
    }
 */

    Figure playerFig;
    newCircle(&playerFig,
        (vec2d){0.45,0.45}, (vec2d){0.55,0.55}, 0x0000FF);
        
    Figure arrowFig;
    newRectangle(&arrowFig, (vec2d){0.55, 0.475}, (vec2d){0.65, 0.525}, 0xFF0000);
    setRotationCenter(&arrowFig, (vec2d){0.5, 0.5}); //same rotation center as playerFig

    Figure *shapes[]    = { &playerFig, &arrowFig };
    Figure *colliders[] = { &playerFig };
    physicsEntity player;
    // friction non-zero so it slows down: e.g., μ = 0.5
    initPhysicsEntity(&player,
                      shapes,    2,
                      colliders, 1,
                      ENTITY_HEAVY, 0.25, 1);

    while (1) {
        clearScreen();
        drawPhysicsEntity(&player);

        char c = 0;
        readChar(&c);
        if (c=='q'||c=='Q'){
            clearScreen();
            break;   
        }

        vec2d accel = {0,0};
        if (c=='w'||c=='W'){
            accel.x = cos_taylor(player.angle) * ACCEL_MAG;
            accel.y = sin_taylor(player.angle) * ACCEL_MAG;
        } 
        else if (c=='a'||c=='A') rotateEntity(&player, LEFT_ROTATION);
        else if (c=='d'||c=='D') rotateEntity(&player, RIGHT_ROTATION);

        setAcceleration(&player, accel);
        updatePhysicsEntity(&player, TIME_STEP);
        setAcceleration(&player, (vec2d){0,0});
    }
}