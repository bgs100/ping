#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "utility.h"

bool error(const char *msg) {
    std::cerr << msg << std::endl;
    return false;
}

bool SDLerror(const char *msg) {
    std::cerr << msg << ": " << SDL_GetError() << std::endl;
    return false;
}

void debug(const char *msg) {
    std::cerr << msg << std::endl;
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

Uint64 ntoh64(Uint64 input) {
    Uint64 val;
    char *data = (char *)&val;

    data[0] = input >> 56;
    data[1] = input >> 48;
    data[2] = input >> 40;
    data[3] = input >> 32;
    data[4] = input >> 24;
    data[5] = input >> 16;
    data[6] = input >> 8;
    data[7] = input >> 0;

    return val;
}

Uint64 hton64(Uint64 input) {
    return ntoh64(input);
}

double ntohd(double input) {
    Uint64 tmp = ntoh64(*(Uint64 *)&input);
    return *(double *)&tmp;
}

double htond(double input) {
    return ntohd(input);
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
