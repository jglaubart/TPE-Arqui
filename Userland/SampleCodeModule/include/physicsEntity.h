#ifndef PHYSICSENTITY_H
#define PHYSICSENTITY_H

#include <stddef.h>
#include "figures.h"

#define MAX_ENTITY_SHAPES     4
#define MAX_ENTITY_COLLIDERS  4

typedef enum { ENTITY_LIGHT, ENTITY_HEAVY } entityType;

typedef struct {
    vec2d      position;      // world-space origin (center)
    double     angle;         // orientation radians
    vec2d      velocity;      // units per second
    vec2d      acceleration;  // units per second^2
    double     friction;      // coefficient μ
    entityType type;          // light vs heavy
    int        collidable;    // 0 = no, 1 = yes

    Figure    *shapes[MAX_ENTITY_SHAPES];
    size_t     shapeCount;
    Figure    *colliders[MAX_ENTITY_COLLIDERS];
    size_t     colliderCount;
} physicsEntity;

void initPhysicsEntity(physicsEntity *e,
                       Figure **shapes, size_t shapeCount,
                       Figure **colliders, size_t colliderCount,
                       entityType type, double friction, int collidable);
void setVelocity(physicsEntity *e, vec2d v);
void setAcceleration(physicsEntity *e, vec2d a);
void updatePhysicsEntity(physicsEntity *e, double dt);
void drawPhysicsEntity(const physicsEntity *e);
int  circleCircleCollide(const physicsEntity *a, const physicsEntity *b);
double calculatePenetrationDepth(const physicsEntity *a, const physicsEntity *b);
void resolveCollisionSimple(physicsEntity *a, physicsEntity *b, double restitution);
void rotateEntity(physicsEntity * e, double angleOffset);
#endif // PHYSICSENTITY_H