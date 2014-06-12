#ifndef PING_UTILITY_H
#define PING_UTILITY_H

#include <SDL2/SDL_net.h>
#include "Entity.h"

bool error(const char *msg);
bool SDLerror(const char *msg);
bool netError(const char *msg);

double clamp(double set, double min, double max);
const char *itoa(int x);
char *netReadLine(TCPsocket sock);
bool checkCollision(Entity a, Entity b);

#endif
