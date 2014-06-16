#include <iostream>
#include <stdlib.h>
#include <stdio.h>
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

double clamp(double set, double min, double max) {
    if (set > max) return max;
    if (set < min) return min;
    return set;
}

char *itoa(int x, char *buf, int size) {
    snprintf(buf, size, "%d", x);
    return buf;
}

char *netReadLine(TCPsocket sock) {
    char *buffer = (char *)malloc(256);
    int i = 0;

    do {
        if (SDLNet_TCP_Recv(sock, buffer+i, 1) < 1) {
            SDLerror("SDLNet_TCP_Recv");
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
