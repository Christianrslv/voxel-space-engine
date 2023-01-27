#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos.h"

#define SCREEM_WIDTH 320
#define SCREEM_WEIGHT 200
#define SCALE_FACTOR 100.0

uint8_t* heightmap = NULL;
uint8_t* colormap = NULL;

typedef struct {
  float x;
  float y;
  float height;
  float zfar;
} camera_t;

camera_t camera = {
  .x = 512,
  .y = 512,
  .height = 150.0,
  .zfar = 400
};

int main(int argc, char* args[]) {
  setvideomode(videomode_320x200);

  uint8_t palette[256 * 3];
  int map_width, map_height, pal_count;
  colormap = loadgif("maps/colormap.gif", &map_width, &map_height, &pal_count, palette);
  heightmap = loadgif("maps/heightmap.gif", &map_width, &map_height, NULL, NULL);

  for(int i = 0; i < pal_count; i++) {
    setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
  }
  setpal(0, 36, 36, 56);

  setdoublebuffer(1);
  uint8_t* framebuffer = screenbuffer();

  while(!shuttingdown()) {
    waitvbl();
    clearscreen();
    
    float plx = -camera.zfar;
    float ply = +camera.zfar;

    float prx = +camera.zfar;
    float pry = +camera.zfar;

    for(int i = 0; i < SCREEM_WIDTH; i++) {
      float delta_x = (plx + (prx - plx) / SCREEM_WIDTH * i) / camera.zfar;
      float delta_y = (ply + (pry - ply) / SCREEM_WIDTH * i) / camera.zfar; 
      
      float rx = camera.x;
      float ry = camera.y;

      float max_height = SCREEM_WEIGHT;

      for(int z = 1; z < camera.zfar; z++) {
        rx += delta_x;
        ry += delta_y;

        int mapoffset = ((1024 * (int)(ry)) + (int)(rx));

        int heightonscreen = (int)((camera.height - heightmap[mapoffset]) / z * SCALE_FACTOR);

        if(heightonscreen < 0) {
          heightonscreen = 0;
        }
        if(heightonscreen > SCREEM_WEIGHT) {
          heightonscreen = SCREEM_WEIGHT - 1;
        }

        if(heightonscreen < max_height) {
          for(int y = heightonscreen; y < max_height; y++) {
            framebuffer[(SCREEM_WIDTH * y) + i] = (uint8_t)colormap[mapoffset];
          }
          max_height = heightonscreen;
        }

        //framebuffer[(SCREEM_WIDTH * (int)(ry/4)) + (int)(rx/4)] = 0x19;
      }
    }

    framebuffer = swapbuffers();

    if(keystate(KEY_ESCAPE))
      break;
  }
  return EXIT_SUCCESS;
}
