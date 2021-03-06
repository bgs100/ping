#ifndef PING_UTILITY_H
#define PING_UTILITY_H

#include <SDL2/SDL_net.h>
#include "Entity.h"

const double pi = 3.14159265358979323846;

bool error(const char *msg);
bool SDLerror(const char *msg);

void debug(const char *msg);

double clamp(double set, double min, double max);
char *itoa(int x, char *buf, int size);

Uint64 ntoh64(Uint64 input);
Uint64 hton64(Uint64 input);
double ntohd(double input);
double htond(double input);

const char *getShortKeyName(SDL_Keycode key, int maxLen=0, int cutLen=0);

#endif
