#include "physicsEntity.h"
#include "figures.h"
#include <usr_stdlib.h>

static double vec2d_length(const vec2d *v) {
    return my_sqrt(v->x*v->x + v->y*v->y);
}
static vec2d vec2d_normalize(vec2d v) {
    double len = vec2d_length(&v);
    return (len>0) ? (vec2d){v.x/len, v.y/len} : (vec2d){0,0};
}
static double vec2d_dot(vec2d a, vec2d b) {
    return a.x*b.x + a.y*b.y;
}
static vec2d vec2d_scale(vec2d v, double s) {
    return (vec2d){v.x*s, v.y*s};
}
static vec2d vec2d_add(vec2d a, vec2d b) {
    return (vec2d){a.x+b.x, a.y+b.y};
}

void initPhysicsEntity(physicsEntity *e,
                       Figure **shapes, size_t shapeCount,
                       Figure **colliders, size_t colliderCount,
                       entityType type, double friction, int collidable)
{
    e->position = (vec2d){0,0};
    e->angle = 0;
    e->velocity = (vec2d){0,0};
    e->acceleration = (vec2d){0,0};
    e->friction = friction;
    e->type = type;
    e->collidable = collidable ? 1 : 0;

    e->shapeCount = (shapeCount > MAX_ENTITY_SHAPES ? MAX_ENTITY_SHAPES : shapeCount);
    for (size_t i = 0; i < e->shapeCount; ++i)
        e->shapes[i] = shapes[i];

    e->colliderCount = (colliderCount > MAX_ENTITY_COLLIDERS ? MAX_ENTITY_COLLIDERS : colliderCount);
    for (size_t i = 0; i < e->colliderCount; ++i)
        e->colliders[i] = colliders[i];
}

void setVelocity(physicsEntity *e, vec2d v) {
    if (e) e->velocity = v;
}
void setAcceleration(physicsEntity *e, vec2d a) {
    if (e) e->acceleration = a;
}

void updatePhysicsEntity(physicsEntity *e, double dt) {
    if (!e) return;
    // integrate acceleration
    e->velocity = vec2d_add(e->velocity, vec2d_scale(e->acceleration, dt));
    // friction (kinetic): decelerate opposite velocity
    double speed = vec2d_length(&e->velocity);
    if (speed > 0) {
        double dec = e->friction * 9.81 * dt;
        if (dec >= speed){
            e->velocity = (vec2d){0,0};
        } else {
            vec2d n = vec2d_normalize(e->velocity);
            e->velocity = vec2d_add(e->velocity, vec2d_scale(n, -dec));
        }
    }
    // move shapes
    vec2d disp = vec2d_scale(e->velocity, dt);
    for (size_t i = 0; i < e->shapeCount; ++i)
        moveFigure(e->shapes[i], disp);

    // boundary bounce
    double fix;
    // X-axis
    for (size_t i = 0; i < e->colliderCount; ++i) {
        Figure *f = e->colliders[i];
        if ((fix = (f->topLeft.x < 0.0 ? -f->topLeft.x : 
                  (f->bottomRight.x > 1.0 ? 1.0 - f->bottomRight.x : 0))) != 0) {
            e->velocity.x = -e->velocity.x;
            for (size_t j = 0; j < e->shapeCount; ++j)
                moveFigure(e->shapes[j], (vec2d){fix, 0});
            break;
        }
    }
    // Y-axis
    for (size_t i = 0; i < e->colliderCount; ++i) {
        Figure *f = e->colliders[i];
        if ((fix = (f->topLeft.y < 0.0 ? -f->topLeft.y : 
                  (f->bottomRight.y > 1.0 ? 1.0 - f->bottomRight.y : 0))) != 0) {
            e->velocity.y = -e->velocity.y;
            for (size_t j = 0; j < e->shapeCount; ++j)
                moveFigure(e->shapes[j], (vec2d){0, fix});
            break;
        }
    }
    // update entity position
    if (e->colliderCount > 0) {
        Figure *f = e->colliders[0];
        e->position.x = (f->topLeft.x + f->bottomRight.x) * 0.5;
        e->position.y = (f->topLeft.y + f->bottomRight.y) * 0.5;
    }
}

void drawPhysicsEntity(const physicsEntity *e) {
    if (!e) return;
    for (size_t i = 0; i < e->shapeCount; ++i)
        drawFigure(e->shapes[i]);
}

int circleCircleCollide(const physicsEntity *a, const physicsEntity *b) {
    if (!a || !b || !a->collidable || !b->collidable) return 0;
    double dx = a->position.x - b->position.x;
    double dy = a->position.y - b->position.y;
    double r1 = (a->colliders[0]->bottomRight.x - a->colliders[0]->topLeft.x) * 0.5;
    double r2 = (b->colliders[0]->bottomRight.x - b->colliders[0]->topLeft.x) * 0.5;
    double r = r1 + r2;
    return (dx*dx + dy*dy) <= (r*r);
}

void resolveCollisionSimple(physicsEntity *a, physicsEntity *b, double restitution) {
    if (!circleCircleCollide(a,b)) return;
    vec2d n = vec2d_normalize((vec2d){
        b->position.x - a->position.x,
        b->position.y - a->position.y
    });
    if (a->type == ENTITY_HEAVY && b->type == ENTITY_LIGHT) {
        double vn = vec2d_dot(b->velocity, n);
        b->velocity = vec2d_add(b->velocity,
            vec2d_scale(n, -(1+restitution)*vn));
        return;
    }
    if (a->type == ENTITY_LIGHT && b->type == ENTITY_HEAVY) {
        double vn = vec2d_dot(a->velocity, n);
        a->velocity = vec2d_add(a->velocity,
            vec2d_scale(n, -(1+restitution)*vn));
        return;
    }
    double va_n = vec2d_dot(a->velocity, n);
    double vb_n = vec2d_dot(b->velocity, n);
    double dA = vb_n - va_n;
    double dB = va_n - vb_n;
    a->velocity = vec2d_add(a->velocity,
        vec2d_scale(n, dA*(1+restitution)));
    b->velocity = vec2d_add(b->velocity,
        vec2d_scale(n, dB*(1+restitution)));
}

void rotateEntity(physicsEntity * e, double angleOffset){
    if(!e)
        return;
    e->angle += angleOffset;
    for(int i = 0; i < e->shapeCount; i++){
        rotateFigure(e->shapes[i], angleOffset);
    }
}