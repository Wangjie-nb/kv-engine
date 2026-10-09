#ifndef RANDOM_H
#define RANDOM_H

#include <cstdlib>
#include <ctime>

class Random {
public:
    static void init() {
        srand((unsigned)time(nullptr));
    }

    static int randomLevel(int maxLevel) {
        int lvl = 1;
        while(rand() % 2 == 0 && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }
};

#endif
