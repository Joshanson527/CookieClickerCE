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
 
#include "gfx/gfx.h"

uint64_t quintillion = 1000000000000000000;
uint64_t quadrillion = 1000000000000000;
uint64_t trillion = 1000000000000;

bool click = false;
bool draw = true;
uint64_t cookies = 0;
uint8_t scale = 2;
uint64_t cps = 0;
uint64_t cpc = 0;
char text[64];

gfx_sprite_t *cookie;

void textBeautifier(void) {
    if (cookies >= quintillion) {
        sprintf(text, "%.2f quintillion cookies", (double)cookies / quintillion);
    } else if (cookies >= quadrillion) {
        sprintf(text, "%.2f quadrillion cookies", (double)cookies / quadrillion);
    } else if (cookies >= trillion) {
        sprintf(text, "%.2f trillion cookies", (double)cookies / trillion);
    } else if (cookies >= 1000000000) {
        sprintf(text, "%.2f billion cookies", (double)cookies / 1000000000);
    } else if (cookies >= 1000000) {
        sprintf(text, "%.2f million cookies", (double)cookies / 1000000);
    } else if (cookies >= 1000) {
        sprintf(text, "%.2f thousand cookies", (double)cookies / 1000);
    } else if (cookies != 1) {
        sprintf(text, "%llu cookies", cookies);
    } else {
        sprintf(text, "%llu cookie", cookies);
    }
}

void redraw(void) {
    gfx_SetDrawBuffer();
    for(int i = 0; i < 16; i++) {
        if (i % 2) {
            gfx_SetColor(3);
        } else {
            gfx_SetColor(4);
        }
        gfx_FillRectangle(i * 20, 0, 20, 240);
    }
    
    if (click) {
        scale = 2;
    } else {
        scale = 3;
    }
    gfx_ScaledTransparentSprite_NoClip(cookie, 160 - (16 * scale), 120 - (16 * scale), scale, scale);

    textBeautifier();
    uint8_t width = gfx_GetStringWidth(text) / 2;
    gfx_PrintStringXY(text, 160 - width, 10);

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
}


int main(void) {

    cookie = gfx_MallocSprite(cookie_width, cookie_height);

    zx0_Decompress(cookie, cookie_compressed);

    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetTextBGColor(3);
    gfx_SetTextFGColor(0);
    gfx_SetTextTransparentColor(2);
    gfx_SetTransparentColor(2);

    redraw();

    kb_Scan();

    while (!(kb_Data[6] & kb_Clear)) {
        if (draw) {redraw();}

        handle_keypresses();
    }

    free(cookie);

    gfx_End();

    return 0;
}
