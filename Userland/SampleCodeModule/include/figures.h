#ifndef _FIGURES_H
#define _FIGURES_H

#include <stdint.h>

// Operación común para todas las figuras
typedef enum { CIRCLE, RECTANGLE } FigureType;

typedef struct{
    double x;  // x coordinate (fractions of screen)
    double y;  // y coordinate (fractions of screen)
} vec2d;

// Tipo abstracto de datos Figure
typedef struct Figure {
    vec2d topLeft;         // bounding‐box top‐left (fractions of screen)
    vec2d bottomRight;     // bounding‐box bottom‐right (fractions of screen)
    vec2d topRight;        // bounding‐box top‐right (fractions of screen)
    vec2d bottomLeft;      // bounding‐box bottom‐left (fractions of screen)
    vec2d rotationCenter;  // point about which to rotate (fractions of screen)
    double angle;   // rotation angle
    uint32_t color;
    FigureType type;
    void (*draw)(struct Figure *self);
    void (*rotate)(struct Figure *self, double angle);
} Figure;

// Constructor de círculo
void newCircle(Figure *c, vec2d topLeft, vec2d bottomRight, uint32_t color);

// Constructor de rectángulo
void newRectangle(Figure *r, vec2d topLeft, vec2d bottomRight, uint32_t color);

void drawFigure(Figure *f);
void moveFigure(Figure *f, vec2d delta);
void rotateFigure(Figure *f, double deltaAngle);
FigureType getFigureType(Figure *f);
void moveVec2d(vec2d *v, vec2d delta);
void setRotationCenter(Figure *f, vec2d center);

#endif