#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "Entity.h"

bool error(const char *msg) {
    std::cerr << msg << std::endl;
    return false;
}

bool SDLerror(const char *msg) {
    std::cerr << msg << ": " << SDL_GetError() << std::endl;
    return false;
}

bool netError(const char *msg) {
    std::cerr << msg << ": " << SDLNet_GetError() << std::endl;
    return false;
}

double clamp(double set, double min, double max) {
    if (set > max) return max;
    if (set < min) return min;
    return set;
}

// This is dumb.
const char *itoa(int x) {
    std::stringstream ss;
    ss << x;
    return ss.str().c_str();
}

char *netReadLine(TCPsocket sock) {
    char *buffer = (char *)malloc(256);
    int i = 0;

    do {
        if (SDLNet_TCP_Recv(sock, buffer+i, 1) < 1) {
            netError("SDLNet_TCP_Recv");
            free(buffer);
            return NULL;
        }
        i++;
    } while (buffer[i-1] != '\n' && i < 256);

    SDLNet_TCP_Recv(sock, buffer+i, 1);
    return buffer;
}

bool checkCollision(Entity a, Entity b) {
    return (a.y + a.h >= b.y && a.y <= b.y + b.h &&
            a.x + a.w >= b.x && a.x <= b.x + b.w);
}
