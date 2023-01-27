#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos.h"

#define SCREEM_WIDTH 320
#define SCREEM_WEIGHT 200

uint8_t* heightmap = NULL;
uint8_t* colormap = NULL;

int main(int argc, char* args[]) {
  setvideomode(videomode_320x200);

  uint8_t palette[256 * 3];
  int map_width, map_height, pal_count;
  colormap = loadgif("files/colormap.gif", &map_width, &map_height, &pal_count, palette);
  heightmap = loadgif("files/heightmap.gif", &map_width, &map_height, NULL, NULL);

  for(int i = 0; i < pal_count; i++) {
    setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
  }
  setpal(0, 36, 36, 56);

  setdoublebuffer(1);
  uint8_t* framebuffer = screenbuffer();

  while(!shuttingdown()) {
    waitvbl();
    clearscreen();
    
    int x = 160;
    int y = 100;
    framebuffer[(SCREEM_WIDTH * y) + x] = 0x50;

    framebuffer = swapbuffers();

    if(keystate(KEY_ESCAPE))
      break;
  }
  return EXIT_SUCCESS;
}
