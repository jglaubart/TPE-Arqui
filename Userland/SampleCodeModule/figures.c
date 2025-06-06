// ADT for Figure, Circle, and Rectangle, with move() and rotate() operations.

#include "usr_stdlib.h"
#include "syscalls.h"
#include "figures.h"

static void drawCircle(Figure *self);
static void drawRectangle(Figure *self);
static void rotateCircle(Figure *f, double deltaAngle);
static void rotateRectangle(Figure *f, double deltaAngle);

static uint64_t xFracToScreen(float xFrac);
static uint64_t yFracToScreen(float yFrac);

void moveVec2d(vec2d *v, vec2d delta) {
    if (!v) return;
    v->x += delta.x;
    v->y += delta.y;
}

// Move: translate any Figure by delta
void moveFigure(Figure *f, vec2d delta) {
    if (!f) return;

    moveVec2d(&f->topLeft, delta);
    moveVec2d(&f->bottomRight, delta);
    moveVec2d(&f->rotationCenter, delta);
}


void rotateCircle(Figure *f, double deltaAngle) {
    if (!f) return;
    f->angle += deltaAngle;
}

void rotateRectangle(Figure *f, double deltaAngle) {
    if (!f) return;
    f->angle += deltaAngle;
    f->angle = normalize_angle(f->angle); // Normalize angle to [-π, π]
    // Precompute sin and cos of deltaAngle
    double s = sin_taylor(deltaAngle);
    double c = cos_taylor(deltaAngle);

    vec2d corners[4] = {
        f->topLeft,
        f->topRight,
        f->bottomRight,
        f->bottomLeft
    };

    for (int i = 0; i < 4; i++) {
        // Translate point to origin (relative to center)
        double dx = corners[i].x - f->rotationCenter.x;
        double dy = corners[i].y - f->rotationCenter.y;

        // Rotate
        double rx = dx * c - dy * s;
        double ry = dx * s + dy * c;

        // Translate back
        corners[i].x = f->rotationCenter.x + rx;
        corners[i].y = f->rotationCenter.y + ry;
    }

    f->topLeft = corners[0];
    f->topRight = corners[1];
    f->bottomRight = corners[2];
    f->bottomLeft = corners[3];
}

void newCircle(Figure *c, vec2d topLeft, vec2d bottomRight, double angle, uint32_t color){
    if (!c) return;

    c->topLeft = topLeft;
    c->bottomRight = bottomRight;
    c->rotationCenter.x = (topLeft.x + bottomRight.x) / 2;
    c->rotationCenter.y = (topLeft.y + bottomRight.y) / 2;
    c->angle = angle;
    c->color = color;
    c->type = CIRCLE;

    // Point draw() to stub (to be implemented later)
    c->draw = drawCircle;
    c->rotate = rotateCircle;
}


void newRectangle(Figure *r, vec2d topLeft, vec2d bottomRight, double angle, uint32_t color) {
    if (!r) return;

    r->topLeft       = topLeft;
    r->bottomRight   = bottomRight;
    r->rotationCenter.x = (topLeft.x + bottomRight.x) / 2;
    r->rotationCenter.y = (topLeft.y + bottomRight.y) / 2;
    r->angle         = angle;
    r->color         = color;
    r->type          = RECTANGLE;

    // Point draw() to stub (to be implemented later)
    r->draw = drawRectangle;
    r->rotate = rotateRectangle;
}


// ------------------------
// Draw stubs (not implemented yet)
static void drawCircle(Figure *self) {
    if (!self) return;
    uint64_t corners[2][2] = {
        {xFracToScreen(self->topLeft.x), yFracToScreen(self->topLeft.y)},
        {xFracToScreen(self->bottomRight.x), yFracToScreen(self->bottomRight.y)}
    };
    sys_call(SYS_DRAW_CIRCLE_ID, corners, self->color, 0, 0);
}

static void drawRectangle(Figure *self) {
    if (!self) return;
    int64_t corners[4][2] = {
        {xFracToScreen(self->topLeft.x), yFracToScreen(self->topLeft.y)},
        {xFracToScreen(self->topRight.x), yFracToScreen(self->topRight.y)},
        {xFracToScreen(self->bottomRight.x), yFracToScreen(self->bottomRight.y)},
        {xFracToScreen(self->bottomLeft.x), yFracToScreen(self->bottomLeft.y)}
    };
    sys_call(SYS_DRAW_RECTANGLE_ID, corners, self->color, 0, 0);
}

void drawFigure(Figure *f) {
    if (!f || !f->draw) return;
    f->draw(f);
}

void rotateFigure(Figure *f, double angle) {
    if (!f || !f->rotate) return;
    f->rotate(f, angle);
}

static uint64_t xFracToScreen(float xFrac) {
    int64_t screenWidth = getScreenWidth();
    return (uint64_t)(xFrac * screenWidth);
}
static uint64_t yFracToScreen(float yFrac) {
    int64_t screenHeight = getScreenHeight();
    return (uint64_t)(yFrac * screenHeight);
}