#include <graphx.h>
#include <keypadc.h>
#include <ti/getcsc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "gfx.h"

struct Car {
    uint8_t lane; //u8 is only 1 byte, so ideal for smaller values
    int x;
    int y;
    int width;
    int height;
    uint8_t speed;
};

void draw();
void drawRoad();
void updateGame();
bool checkCollision(struct Car, struct Car);
void updateSpeed();
void endScreen();

