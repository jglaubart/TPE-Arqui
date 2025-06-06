#include "pongis.h"
#include <usr_stdlib.h>
#include "figures.h"

void pongisInit(){
    Figure rect1;
    vec2d topLeft = {-0.25, 0.25};
    vec2d bottomRight = {0.25, 0.75};
    newRectangle(&rect1, topLeft, bottomRight, 0x00FF00);
    Figure rect2;
    vec2d topLeft2 = {0.75, 0.25};
    vec2d bottomRight2 = {0.9, 0.75};
    newRectangle(&rect2, topLeft2, bottomRight2, 0xFF0000);
    setRotationCenter(&rect2, (vec2d){0.5, 0.5}); // Set rotation center for rect2
    Figure circle1;
    vec2d topLeft3 = {0.25, 0.25};
    vec2d bottomRight3 = {0.75, 0.75};
    newCircle(&circle1, topLeft3, bottomRight3, 0x0000FF);
    Figure figures[] = {rect1, rect2, circle1};
    while(1){
        clearScreen();
        for (int i = 0; i < 3; i++) {
            drawFigure(&figures[i]);
            rotateFigure(&figures[i], 0.01); // Rotate each figure slightly
            moveFigure(&figures[i], (vec2d){0.01, 0.01}); // Move each figure slightly
        }
    }
}