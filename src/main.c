/*
 *--------------------------------------
 * Program Name: CookieClickerCE
 * Author: Josh
 * License: MIT
 * Description: A port of Cookie Clicker for the TI 84 Plus CE
 *--------------------------------------
*/

#include <graphx.h>
#include <compression.h>
#include <keypadc.h>
#include <stdio.h>
#include <time.h>
 
#include "gfx/gfx.h"

uint64_t quintillion = 1000000000000000000;
uint64_t quadrillion = 1000000000000000;
uint64_t trillion = 1000000000000;

bool click = false;
bool draw = true;
uint8_t page = 0;
uint64_t cookies = 0;
uint8_t scale = 2;
uint64_t cps = 10;
uint64_t cpc = 10;
char text[64];
int i;

uint8_t buildingProductionRateMultiplier = 4;
uint8_t buildingBaseCost = 15; // Base building cost
uint8_t buildingGrowthFactor = 2; // Amount cost changes for each additional building
uint8_t buildingScalingFactor = 3; // Amount cost changes for each type of building
uint8_t upgradeBaseCost = 50; // Base upgrade cost
uint8_t upgradeGrowthFactor = 3; // Amount cost changes for each additional upgrade
uint8_t upgradeScalingFactor = 2; // Amount cost changes for each type of upgrade

uint16_t buildingQuantities[5]; // Amount of each building
uint8_t upgradeQuantities[5]; // Amount of each upgrade

gfx_sprite_t *cookie;

uint64_t costCalculator(int type , int level) {
    if (type) {
        return upgradeBaseCost * (upgradeGrowthFactor ^ upgradeQuantities[level]) * (1 + (upgradeScalingFactor * level));
    } else {
        return buildingBaseCost * (buildingScalingFactor ^ level) * (buildingGrowthFactor ^ buildingQuantities[level]);
    }
}

void updateCps() {
    cps = 0;
    for (i = 0; i > 5; i++) {
        double upgradeMultiplier = 1;
        if (upgradeQuantities[i] == 1) {
            upgradeMultiplier = 1.25;
        } else if (upgradeQuantities[i] == 2) {
            upgradeMultiplier = 1.50;
        } else if (upgradeQuantities[i] == 3) {
            upgradeMultiplier = 2;
        }
        cps += (buildingProductionRateMultiplier ^ i) * buildingQuantities[i] * upgradeMultiplier;
    }
}

void textBeautifier(uint64_t numCookies, char addition[16]) {
    if (numCookies >= quintillion) {
        sprintf(text, "%.2f quintillion cookies%s", (double)numCookies / quintillion, addition);
    } else if (numCookies >= quadrillion) {
        sprintf(text, "%.2f quadrillion cookies%s", (double)numCookies / quadrillion, addition);
    } else if (numCookies >= trillion) {
        sprintf(text, "%.2f trillion cookies%s", (double)numCookies / trillion, addition);
    } else if (numCookies >= 1000000000) {
        sprintf(text, "%.2f billion cookies%s", (double)numCookies / 1000000000, addition);
    } else if (numCookies >= 1000000) {
        sprintf(text, "%.2f million cookies%s", (double)numCookies / 1000000, addition);
    } else if (numCookies >= 1000) {
        sprintf(text, "%.2f thousand cookies%s", (double)numCookies / 1000, addition);
    } else if (numCookies != 1) {
        sprintf(text, "%llu cookies%s", numCookies, addition);
    } else {
        sprintf(text, "%llu cookie%s", numCookies, addition);
    }
}

void redraw(void) {
    gfx_SetDrawBuffer();
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
        gfx_ScaledTransparentSprite_NoClip(cookie, 160 - (16 * scale), 120 - (16 * scale), scale, scale);

        textBeautifier(cookies, "");
        uint8_t width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 10);

        textBeautifier(cps, " per second");
        width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 20);

        textBeautifier(cpc, " per click");
        width = gfx_GetStringWidth(text) / 2;
        gfx_PrintStringXY(text, 160 - width, 30);

        width = gfx_GetStringWidth("Cookie   Buildings   Upgrades   Achiev.  Settings") / 2;
        gfx_PrintStringXY("Cookie   Buildings   Upgrades   Achiev.  Settings", 160 - width, 230);
    }

    gfx_SwapDraw();
    draw = false;
}

void handle_keypresses(void) {
    kb_Scan();

    if ((kb_Data[1] & kb_2nd) && !click) {
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
    if (kb_Data[1] & kb_Yequ) {
        page = 0;
        draw = true;
    } else if (kb_Data[1] & kb_Window) {
        page = 1;
        draw = true;
    } else if (kb_Data[1] & kb_Zoom) {
        page = 2;
        draw = true;
    } else if (kb_Data[1] & kb_Trace) {
        page = 3;
        draw = true;
    } else if (kb_Data[1] & kb_Graph) {
        page = 4;
        draw = true;
    }
}


int main(void) {

    cookie = gfx_MallocSprite(Cookie_width, Cookie_height);

    zx0_Decompress(cookie, Cookie_compressed);

    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetTextBGColor(3);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(2);
    gfx_SetTransparentColor(2);

    redraw();

    kb_Scan();

    clock_t start = clock();

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

    gfx_End();

    return 0;
}
