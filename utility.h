#ifndef PING_UTILITY_H
#define PING_UTILITY_H

#include <SDL2/SDL_net.h>
#include "GameManager.h"
#include "Entity.h"

bool error(const char *msg);
bool SDLerror(const char *msg);
bool errorScreen(GameManager *m, const char *msg);
bool errorWithScreen(GameManager *m, const char *msg);

void debug(const char *msg);

double clamp(double set, double min, double max);
char *itoa(int x, char *buf, int size);
char *netReadLine(TCPsocket sock);
bool checkCollision(Entity a, Entity b);

#endif
