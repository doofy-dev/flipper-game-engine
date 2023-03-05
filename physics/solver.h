#pragma once
#include "collision.h"

void fix_position(CollisionInfo *info);
void resolve_bouncing(CollisionInfo *info);
void resolve_friction(CollisionInfo *info);