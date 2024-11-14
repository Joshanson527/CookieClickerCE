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
#include "tilemap/tilemap.c"

#define TILE_WIDTH          16
#define TILE_HEIGHT         16
#define TILEMAP_WIDTH       4
#define TILEMAP_HEIGHT      3

unit64_t octillion = 1000000000000000000000000000;
unit64_t septillion = 1000000000000000000000000;
unit64_t sextillion = 1000000000000000000000;
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
unsigned int i;

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

uint8_t tilemap_map[TILEMAP_WIDTH * TILEMAP_HEIGHT];
gfx_sprite_t *tileset_tiles[128];
gfx_tilemap_t tilemap;
gfx_sprite_t *tmp_ptr;
gfx_sprite_t *tmp_ptr;
uint8_t tile_x_pos = 0;
uint8_t tile_y_pos = 0;

uint64_t costCalculator(int type , int level) {
    if (type) {
        return upgradeBaseCost * (upgradeGrowthFactor ^ upgradeQuantities[level]) * (1 + (upgradeScalingFactor * level));
    } else {
        return buildingBaseCost * (buildingScalingFactor ^ level) * (buildingGrowthFactor ^ buildingQuantities[level]);
    }
}

void updateCps() {
    cps = 0;
    for (i = 0; i < 5; i++) {
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

void textBeautifier(uint64_t numCookies, char addition[25]) {
    if (numCookies >= octillion) {
        sprintf(text, "%.2f octillion cookies%s", (double)numCookies / octillion, addition);
     } else if (numCookies >= septillion) {
        sprintf(text, "%.2f septillion cookies%s", (double)numCookies / septillion, addition);
     } else if (numCookies >= sextillion) {
        sprintf(text, "%.2f sextillion cookies%s", (double)numCookies / sextillion, addition);
     } else if (numCookies >= quintillion) {
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

void printSprite(uint8_t sprite[2], uint8_t pos[2]) {
    tile_x_pos = pos[0];
    tile_y_pos = pos[1];
    gfx_TransparentTilemap(&tilemap, sprite[0] * TILE_WIDTH, sprite[1] * TILE_HEIGHT);
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
    } else if (page == 1) {
        for (uint8_t x = 0; x < 2; x++) {
            for (uint8_t y = 0; y < 10; y++) {
                gfx_SetColor(4);
                gfx_FillRectangle((x * 160) + 2, (y * 24) + 2, 156, 20);
                gfx_SetColor(3);
                gfx_Rectangle((x * 160) + 2, (y * 24) + 2, 156, 20);
            }
        }
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

    tilemap.map         = tilemap_map;
    tilemap.tiles       = tileset_tiles;
    tilemap.type_width  = gfx_tile_16_pixel;
    tilemap.type_height = gfx_tile_16_pixel;
    tilemap.tile_height = TILE_HEIGHT;
    tilemap.tile_width  = TILE_WIDTH;
    tilemap.draw_height = 1;
    tilemap.draw_width  = 1;
    tilemap.height      = TILEMAP_HEIGHT;
    tilemap.width       = TILEMAP_WIDTH;
    tilemap.y_loc       = tile_x_pos;
    tilemap.x_loc       = tile_y_pos;

    
    for (i = 0; i < sizeof(tileset_tiles) / sizeof(gfx_sprite_t*); i++)
    {
        tmp_ptr = gfx_MallocSprite(TILE_WIDTH, TILE_HEIGHT);
        zx7_Decompress(tmp_ptr, Tileset_tiles_compressed[i]);
        //tileset_tiles[i] = tmp_ptr;
    }
    

    zx7_Decompress(cookie, Cookie_compressed);

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
