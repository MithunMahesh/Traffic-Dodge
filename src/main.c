#include <graphx.h>
#include <keypadc.h>
#include <ti/getcsc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "gfx.h"


#define LANE_SIZE (GFX_LCD_WIDTH / 5)
#define CAR_HEIGHT (80)
#define NUM_TRAFFIC_CARS (4)

struct Car {
    uint8_t lane; //u8 is only 1 byte, so ideal for smaller values
    int x;
    int y;
    int width;
    int height;
    uint8_t speed;
};

struct Car player_car = {0};
//struct Car traffic_car = {0};
//struct Car traffic_car2 = {0};
struct Car traffic_cars[NUM_TRAFFIC_CARS] = {0};
int score;
int score_increase;
int lastLaneUsed = -1;
int lane = 0;
bool validLane;
int attempts;

void draw();
void drawRoad();
void updateGame();
bool checkCollision(struct Car, struct Car);
void updateSpeed();
void endScreen();

bool leftPressed = false;
bool rightPressed = false;

char buffer[20];

int main(void) {
    gfx_Begin();

    gfx_SetPalette(mypalette, sizeof(mypalette), 0);
    player_car.lane = 2;
    player_car.x = LANE_SIZE;
    player_car.y = GFX_LCD_HEIGHT - CAR_HEIGHT;
    player_car.width = LANE_SIZE;
    player_car.height = CAR_HEIGHT;
    player_car.speed = 0;

    //Random seed based on time
    srand((unsigned)time(NULL));

    for (int i = 0; i < NUM_TRAFFIC_CARS; i++) {
        // Check against ALL other cars to avoid overlaps
        validLane = false;
        attempts = 0;

        while (!validLane && attempts < 10) {  // Prevent infinite loop
            lane = rand() % 4;  // Random lane 0-4
            validLane = true;
            
            // Check if this lane conflicts with any other car
            for (int j = 0; j < NUM_TRAFFIC_CARS; j++) {
                if (j != i && traffic_cars[j].lane == lane && 
                    abs(traffic_cars[j].y - traffic_cars[i].y) < CAR_HEIGHT * 2) {
                    validLane = false;
                    break;
                }
            }
            attempts++;
        }

        if (validLane) {
            traffic_cars[i].lane = lane;
        } else {
            traffic_cars[i].lane = rand() % 5;
        }

        traffic_cars[i].x = LANE_SIZE;
        //Stagger cars more
        traffic_cars[i].y = -CAR_HEIGHT - (i * 150) - (rand() % 100); 
        traffic_cars[i].width = LANE_SIZE;
        traffic_cars[i].height = CAR_HEIGHT;
        traffic_cars[i].speed = 15;
    }
    /*
    traffic_cars[1].lane = 0;
    traffic_cars[1].x = 0;
    traffic_cars[1].y = -(CAR_HEIGHT + 20);
    traffic_cars[1].width = LANE_SIZE;
    traffic_cars[1].height = CAR_HEIGHT;
    traffic_cars[1].speed = 20;
    */

    //traffic_cars[0] = traffic_car;
    //traffic_cars[1] = traffic_car2;

    int frame_count = 0;
    int threshold = 200;
    bool collision = false;

    draw();

    while (1) {
        kb_Scan();

        if (kb_Data[7] & kb_Left) {
            if (!leftPressed && player_car.lane > 0) {
                player_car.lane -= 1;
            }
            leftPressed = true;
            draw();
        } else {
            leftPressed = false;
        }

        if (kb_Data[7] & kb_Right) {
            if (!rightPressed && player_car.lane < 4) {
                player_car.lane += 1;
            }
            rightPressed = true;
            draw();
        } else {
            rightPressed = false;
        }

        if (kb_Data[6] & kb_Clear) {
            break;
        }

        for (int j = 0; j < NUM_TRAFFIC_CARS; j++) {
            if (checkCollision(player_car, traffic_cars[j])) {
                collision = true;
            }
        }

        if (collision) {
            endScreen();
            break;
        }

        //Maybe modify frame rate thresholds for speed instead for smoother performacne
        frame_count++;
        if (frame_count > threshold) {
            updateGame();
            draw();
            frame_count = 0;
        }

        if (score_increase >= 3) {
            if (threshold > 50) {
                threshold -= 50;
            }
            score_increase = 0;
        }

        gfx_Wait();        
    }

    gfx_End();
    return 0;
}

void updateGame() {
    for (int j = 0; j < NUM_TRAFFIC_CARS; j++) {
        traffic_cars[j].y += traffic_cars[j].speed;
    }

    /*
    //Respawning traffic cars when they move off screen
    for (int i = 0; i < NUM_TRAFFIC_CARS; i++) {
        if (traffic_cars[i].y > GFX_LCD_HEIGHT) {
            traffic_cars[i].y = -CAR_HEIGHT - (i * 150) - (rand() % 100);
    
            //Do while prevents car from spawning in same lane
            do {
                lane = (rand() % (4 - 0 + 1)) + 0;
            } while (lane == lastLaneUsed);

            traffic_cars[i].lane = lane;
            lastLaneUsed = lane;
            score++;
            score_increase++;
        }
    }*/

    //Respawning traffic cars when they move off screen
    for (int i = 0; i < NUM_TRAFFIC_CARS; i++) {
        if (traffic_cars[i].y > GFX_LCD_HEIGHT) {
            traffic_cars[i].y = -CAR_HEIGHT - (i * 150) - (rand() % 100);

            // Check against ALL other cars to avoid overlaps
            validLane = false;
            attempts = 0;
        
            while (!validLane && attempts < 10) {  // Prevent infinite loop
                lane = rand() % 4;  // Random lane 0-4
                validLane = true;
            
                // Check if this lane conflicts with any other car
                for (int j = 0; j < NUM_TRAFFIC_CARS; j++) {
                    if (j != i && traffic_cars[j].lane == lane && 
                        abs(traffic_cars[j].y - traffic_cars[i].y) < CAR_HEIGHT * 2) {
                        validLane = false;
                        break;
                    }
                }
            attempts++;
        }

        if (validLane) {
            traffic_cars[i].lane = lane;
        } else {
            traffic_cars[i].lane = rand() % 5;
        }
        
        score++;
        score_increase++;
    }
}
}

void draw() {
    drawRoad();
    //gfx_SetColor(0x07);
    //gfx_FillRectangle((player_car.lane * player_car.width) + 3, player_car.y, player_car.width - 3, player_car.height);

    gfx_TransparentSprite(player, (player_car.lane * player_car.width) + 3, player_car.y);

    //gfx_SetColor(0xE0);
    for (int i = 0; i < NUM_TRAFFIC_CARS; i++) {
        gfx_TransparentSprite(traffic, (traffic_cars[i].lane * traffic_cars[i].width) + 3, traffic_cars[i].y);        
        //gfx_FillRectangle((traffic_cars[i].lane * traffic_cars[i].width) + 3, traffic_cars[i].y, traffic_cars[i].width - 3, traffic_cars[i].height);
    }

    gfx_SetTextFGColor(0);
    sprintf(buffer, "Score: %d", score);
    gfx_PrintStringXY(buffer, 134, 3);
    gfx_SwapDraw(); 
}

void drawRoad() {
    gfx_FillScreen(202);

    gfx_SetColor(255);
    
    //Draw dashed lines using rectangles (to make lines thicker)

    for (int a = 1; a < 5; a++) {
        for (int i = 10; i < GFX_LCD_HEIGHT; i += 30) {
            gfx_FillRectangle(LANE_SIZE * a, i, 3, 15);
        }
    }

}

bool checkCollision(struct Car car1, struct Car car2) {
    if ((car1.lane == car2.lane) && 
        (abs(car1.y - car2.y) < CAR_HEIGHT)) {
        return true;
    }
    return false;
}

void updateSpeed() {
    for (int i = 0; i < NUM_TRAFFIC_CARS; i++) {
        traffic_cars[i].speed += 1;
    }
}


//Fix centering and format
void endScreen() {
    gfx_FillScreen(255);
    gfx_SetTextFGColor(0);
    
    // Game Over text
    char* gameOverText = "Game Over!";
    int gameOverWidth = gfx_GetStringWidth(gameOverText);
    gfx_PrintStringXY(gameOverText, (320 - gameOverWidth) / 2, 80);
    
    // Score text  
    sprintf(buffer, "Score: %d", score);
    int scoreWidth = gfx_GetStringWidth(buffer);
    gfx_PrintStringXY(buffer, (320 - scoreWidth) / 2, 120);
    
    // Exit instruction
    char* exitText = "Press any key to exit.";
    int exitWidth = gfx_GetStringWidth(exitText);
    gfx_PrintStringXY(exitText, (320 - exitWidth) / 2, 180);
    
    gfx_SwapDraw();
    while (!os_GetCSC());
}
