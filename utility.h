#ifndef PING_UTILITY_H
#define PING_UTILITY_H

#include <SDL2/SDL_net.h>
#include "Entity.h"

bool error(const char *msg);
bool SDLerror(const char *msg);

void debug(const char *msg);

double clamp(double set, double min, double max);
char *itoa(int x, char *buf, int size);

Uint64 ntoh64(Uint64 input);
Uint64 hton64(Uint64 input);
double ntohd(double input);
double htond(double input);

char *netReadLine(TCPsocket sock);

bool checkCollision(Entity a, Entity b);

#endif
