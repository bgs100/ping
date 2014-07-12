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

// maxLen and cutLen default to 0 (see utility.h).
const char *getShortKeyName(SDL_Keycode key, int maxLen, int cutLen) {
    static std::string name;
    const char *longName = SDL_GetKeyName(key);
    name = longName;

    size_t pos;
    if ((pos = name.find("Left")) != std::string::npos)
        name.replace(pos, 4, "L");
    else if ((pos = name.find("Right")) != std::string::npos)
        name.replace(pos, 5, "R");
    else if ((pos = name.find("Keypad")) != std::string::npos)
        name.replace(pos, 6, "KP");

    if (maxLen > 0 && (int)name.size() > maxLen) {
        if (cutLen < 1)
            cutLen = maxLen - 3;
        name.erase(name.size() - (name.size() - cutLen), cutLen - name.size());
        name += "...";
    }

    return name.c_str();
}
