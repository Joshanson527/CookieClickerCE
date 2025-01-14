/*
 *--------------------------------------
 * Program Name: CookieClickerCE
 * Author: Joshanson537 and LawfulDmcBoo
 * License: MIT
 * Description: A port of Cookie Clicker for the TI-84 Plus CE
 *--------------------------------------
*/

// Library includes

#include <graphx.h>
#include <compression.h>
#include <keypadc.h>
#include <stdio.h>
#include <time.h>
#include <debug.h>
#include <math.h>
 
#include "gfx/gfx.h"

uint64_t quintillion = 1000000000000000000; // Typing the number instead of the variable doesn't work, it gets rounded
uint64_t quadrillion = 1000000000000000;
uint64_t trillion = 1000000000000;

bool click = false;
bool keyDown = false;
bool draw = true;
uint8_t page = 0;
uint64_t cookies = 0;
uint8_t scale = 2;
double cps = 0;
uint64_t cpc = 1;
char text[64];
unsigned int i;

uint8_t cursorx = 0;
uint8_t cursory = 0;

uint8_t buildingProductionRateMultiplier = 4;
uint8_t buildingBaseCost = 15; // Base building cost
uint8_t buildingGrowthFactor = 2; // Amount cost changes for each additional building
uint8_t buildingScalingFactor = 3; // Amount cost changes for each type of building
uint8_t upgradeBaseCost = 150; // Base upgrade cost
uint8_t upgradeGrowthFactor = 3; // Amount cost changes for each additional upgrade
uint8_t upgradeScalingFactor = 3; // Amount cost changes for each type of upgrade

uint16_t buildingQuantities[16]; // Amount of each building
uint8_t upgradeQuantities[16]; // Amount of each upgrade

gfx_sprite_t *cookie;

// Calculates cost to buy another building/upgrade
// type is 0 for building, 1 for upgrade
// Returns number of cookies needed to buy
uint64_t costCalculator(uint8_t type, uint8_t level) {
    if (type) {
        return upgradeBaseCost * (int) pow(upgradeGrowthFactor, upgradeQuantities[level]) * (1 + (upgradeScalingFactor * level));
    } else {
        return buildingBaseCost * (int) pow(buildingScalingFactor, level) * (int) pow(buildingGrowthFactor, buildingQuantities[level]);
    }
}

// Updates the cps bariable with current cps, runs when something is bought
void updateCps() {
    cps = 0;
    for (i = 0; i < 16; i++) {
        double upgradeMultiplier = 1;
        if (upgradeQuantities[i] == 1) {
            upgradeMultiplier = 1.25;
        } else if (upgradeQuantities[i] == 2) {
            upgradeMultiplier = 1.50;
        } else if (upgradeQuantities[i] == 3) {
            upgradeMultiplier = 2;
        }
        cps += pow(buildingProductionRateMultiplier, i) * buildingQuantities[i] * upgradeMultiplier;
    }
}

// Takes a big number (uint64_t) and reformats it, also optionally adds a string to the end
// Stores string in variable 'text'
void textBeautifier(uint64_t numCookies, char addition[16]) {
    if (numCookies >= quintillion) {
        sprintf(text, "%.2f quint. cookies%s", (double) numCookies / quintillion, addition);
    } else if (numCookies >= quadrillion) {
        sprintf(text, "%.2f quad. cookies%s", (double) numCookies / quadrillion, addition);
    } else if (numCookies >= trillion) {
        sprintf(text, "%.2f T cookies%s", (double) numCookies / trillion, addition);
    } else if (numCookies >= 1000000000) {
        sprintf(text, "%.2f B cookies%s", (double) numCookies / 1000000000, addition);
    } else if (numCookies >= 1000000) {
        sprintf(text, "%.2f M cookies%s", (double) numCookies / 1000000, addition);
    } else if (numCookies >= 1000) {
        sprintf(text, "%.2f K cookies%s", (double) numCookies / 1000, addition);
    } else if (numCookies != 1) {
        sprintf(text, "%llu cookies%s", numCookies, addition);
    } else {
        sprintf(text, "%llu cookie%s", numCookies, addition);
    }
}

// Displays a tile from the tilemap on the screen
// type is 0 for building, 1 for upgrade
void printSprite(uint8_t type, uint8_t index, uint8_t xpos, uint8_t ypos) {
    if (type) {
        gfx_TransparentSprite(tileset_tiles[index], xpos, ypos); 
    } else {
        gfx_TransparentSprite(tileset_tiles[index], xpos, ypos); 
    }
}

// Draws ALL the graphics, only run when needed
// To run, set draw to true and it will run next gametick
void redraw(void) {
    uint8_t width;

    gfx_SetDrawBuffer(); // Draw to the buffer (virtual screen), not the actual screen

    // Draw striped background
    for(i = 0; i < 16; i++) {
        if (i % 2) {
            gfx_SetColor(3);
        } else {
            gfx_SetColor(4);
        }
        gfx_FillRectangle(i * 20, 0, 20, 240);
    }
    
    if (page == 0) {
            if (click) {
            scale = 2;
        } else {
            scale = 3;
        }
        gfx_ScaledTransparentSprite_NoClip(cookie, 160 - (16 * scale), 120 - (16 * scale), scale, scale); // Draw big cookie

        // Print cookie info
        textBeautifier(cookies, "");
        width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 10);

        textBeautifier(cps, " per second");
        width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 20);

        textBeautifier(cpc, " per click");
        width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 30);

    } else if (page == 1 || page == 2) {

        // Draw menu items
        for (uint8_t x = 0; x < 2; x++) {
            for (uint8_t y = 0; y < 8; y++) {

                // Draw the boxes and highlight if selected
                gfx_SetColor(4);
                gfx_FillRectangle((x * 160) + 2, (y * 28) + 2, 156, 26);
                if (cursorx == x && cursory == y) {
                    if (click) {
                        gfx_SetColor(5);
                    } else {    
                        gfx_SetColor(0);
                    }
                } else {
                    gfx_SetColor(3);
                }
                gfx_Rectangle((x * 160) + 2, (y * 28) + 2, 156, 26);
                
                if (((x * 8) + y) < 16) {
                    printSprite(page - 1, (x * 8) + y, (x * 160) + 4, (y * 28) + 4); // Draw icon
                    
                    gfx_SetTextBGColor(4);

                    // Check if you already have all 3 upgrades
                    if (page == 2 && upgradeQuantities[(x * 8) + y] == 3) {
                        // Tell the user they can't buy any more
                        gfx_SetTextFGColor(0);
                        sprintf(text, "Max level!");
                    } else {
                        // Display cost
                        if (costCalculator(page - 1, (x * 8) + y) <= cookies) {
                            gfx_SetTextFGColor(5);
                        } else {
                            gfx_SetTextFGColor(6);
                        }
                        textBeautifier(costCalculator(page - 1, (x * 8) + y), "");
                    }
                    gfx_PrintStringXY(text, (x * 160) + 20, (y * 28) + 6);

                    // Display amount owned
                    gfx_SetTextFGColor(0);
                    if (page == 1) {
                        sprintf(text, "You have %u", buildingQuantities[(x * 8) + y]);
                    } else {
                        sprintf(text, "You have %u", upgradeQuantities[(x * 8) + y]);
                    }
                    gfx_PrintStringXY(text, (x * 160) + 20, (y * 28) + 16);

                }
            }
        }

        // Reset text colors
        gfx_SetTextFGColor(0);
        gfx_SetTextBGColor(3);
    }

    // Print nav bar at bottom
    width = gfx_GetStringWidth("Cookie   Buildings   Upgrades   Achiev.  Settings") / 2;
    gfx_PrintStringXY("Cookie   Buildings   Upgrades   Achiev.  Settings", 160 - width, 230);

    gfx_SwapDraw(); // Output the buffer to the screen
    draw = false;
}

// Handles all the keypresses and does something with them
// See keypadhelp.c in the project folder on how to detect keys
void handle_keypresses(void) {
    kb_Scan();

    // Detect cookie clicks on page 0
    if ((kb_Data[1] & kb_2nd) && !click && page == 0) {
        if ((cookies + cpc) > quintillion) {
            cookies = quintillion;
        } else {
            cookies += cpc;
        }
        click = true;
        draw = true;
    }
    if (!(kb_Data[1] & kb_2nd)) {
        if (click) {
            draw = true;
        }
        click = false;
    }

    // Detect cursor movements in pages 1 and 2
    if ((kb_Data[7] & kb_Down) && cursory != 7 && !keyDown) {
        cursory++;
        keyDown = true;
        draw = true;
    }
    if ((kb_Data[7] & kb_Up) && cursory != 0 && !keyDown) {
        cursory--;
        keyDown = true;
        draw = true;
    }
    if ((kb_Data[7] & kb_Right) && cursorx != 1 && !keyDown) {
        cursorx++;
        keyDown = true;
        draw = true;
    }
    if ((kb_Data[7] & kb_Left) && cursorx != 0 && !keyDown) {
        cursorx--;
        keyDown = true;
        draw = true;
    }
    if (!(kb_Data[7] & kb_Down) && !(kb_Data[7] & kb_Up) && !(kb_Data[7] & kb_Right) && !(kb_Data[7] & kb_Left)) {
        keyDown = false;
    }

    if ((kb_Data[1] & kb_2nd) && (page == 1 || page == 2) && costCalculator(page - 1, (cursorx * 8) + cursory) <= cookies && !click) {
        if ( page == 2 && upgradeQuantities[(cursorx * 8) + cursory] > 3) {
        } else {
            cookies -= costCalculator(page - 1, (cursorx * 8) + cursory);
            if (page == 1) {
                buildingQuantities[(cursorx * 8) + cursory]++;
            } else {
                upgradeQuantities[(cursorx * 8) + cursory]++;
            }
            updateCps();
            click = true;
            draw = true;
        }
    }

    // Detect top row of keys to change pages
    if (kb_Data[1] & kb_Yequ) {
        page = 0;
        draw = true;
    } else if (kb_Data[1] & kb_Window) {
        page = 1;
        draw = true;
        cursorx = 0;
        cursory = 0;
    } else if (kb_Data[1] & kb_Zoom) {
        page = 2;
        draw = true;
        cursorx = 0;
        cursory = 0;
    } else if (kb_Data[1] & kb_Trace) {
        page = 3;
        draw = true;
    } else if (kb_Data[1] & kb_Graph) {
        page = 4;
        draw = true;
    }
}

// Main function
// Runs once on program start
int main(void) {

    cookie = gfx_MallocSprite(Cookie_width, Cookie_height);

    zx7_Decompress(cookie, Cookie_compressed); // Decompress cookie sprite

    // Initialize grapgics stuff
    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetTextBGColor(3);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(2);
    gfx_SetTransparentColor(2);

    redraw();

    kb_Scan();

    clock_t start = clock();

    // Gameloop
    while (!(kb_Data[6] & kb_Clear)) {
        if (draw) {redraw();}

        clock_t now = clock();
        if (((float)(now - start) / CLOCKS_PER_SEC) > 1) {
            start = clock();
            cookies += cps;
            draw = true;
        }

        handle_keypresses();
    }

    free(cookie);

    gfx_End(); // Exit graphics library

    return 0;
}
