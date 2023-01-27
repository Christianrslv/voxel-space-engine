#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos.h"

#define SCREEM_WIDTH 320
#define SCREEM_HEIGHT 200
#define SCALE_FACTOR 100.0

uint8_t* heightmap = NULL;
uint8_t* colormap = NULL;

typedef struct {
  float x;
  float y;
  float height;
  float angle;
  float horizon;
  float tilt;
  float zfar;
} camera_t;

camera_t camera = {
  .x = 512,
  .y = 512,
  .height = 150.0,
  .angle = 1.5 * 3.141592,
  .horizon = 100.0,
  .tilt = 0.0,
  .zfar = 1000
};

void processinput() {
  if(keystate(KEY_W)) {
    camera.x += cos(camera.angle);
    camera.y += sin(camera.angle);
  }
  if(keystate(KEY_S)) {
    camera.x -= cos(camera.angle);
    camera.y -= sin(camera.angle);
  }
  if(keystate(KEY_D)) {
    camera.x += cos(camera.angle + (0.5 * 3.141592));
    camera.y += sin(camera.angle + (0.5 * 3.141592));
  }
  if(keystate(KEY_A)) {
    camera.x += cos(camera.angle + (1.5 * 3.141592));
    camera.y += sin(camera.angle + (1.5 * 3.141592));
  }
  if(keystate(KEY_LEFT)) {
    camera.angle -= 0.01;
    if(camera.tilt < 1) camera.tilt += 0.05;
  } else {
    if(camera.tilt > 0) camera.tilt -= 0.05;
  }
  if(keystate(KEY_RIGHT)) {
    camera.angle += 0.01;
    if(camera.tilt > -1) camera.tilt -= 0.05;
  } else {
    if(camera.tilt < 0) camera.tilt += 0.05;
  }
  if(keystate(KEY_SHIFT)) camera.height++;
  if(keystate(KEY_LCONTROL)) camera.height--;
  if(keystate(KEY_UP)) camera.horizon += 1.5;
  if(keystate(KEY_DOWN)) camera.horizon -= 1.5;
}

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
    
    processinput();

    float sinangle = sin(camera.angle);
    float cosangle = cos(camera.angle);

    float plx = cosangle * camera.zfar + sinangle * camera.zfar;
    float ply = sinangle * camera.zfar - cosangle * camera.zfar;

    float prx = cosangle * camera.zfar - sinangle * camera.zfar;
    float pry = sinangle * camera.zfar + cosangle * camera.zfar;

    for(int i = 0; i < SCREEM_WIDTH; i++) {
      float delta_x = (plx + (prx - plx) / SCREEM_WIDTH * i) / camera.zfar;
      float delta_y = (ply + (pry - ply) / SCREEM_WIDTH * i) / camera.zfar; 
      
      float rx = camera.x;
      float ry = camera.y;

      float max_height = SCREEM_HEIGHT;

      for(int z = 1; z < camera.zfar; z++) {
        rx += delta_x;
        ry += delta_y;

        int mapoffset = ((1024 * ((int)(ry) & 1023)) + ((int)(rx) & 1023));

        int proj_height = (int)((camera.height - heightmap[mapoffset]) / z * SCALE_FACTOR + camera.horizon);

        if(proj_height < 0) {
          proj_height = 0;
        }
        if(proj_height > SCREEM_HEIGHT) {
          proj_height = SCREEM_HEIGHT - 1;
        }

        if(proj_height < max_height) {
          float lean = (camera.tilt * (i / (float)SCREEM_WIDTH - 0.5) + 0.5) * SCREEM_HEIGHT / 6;
          for(int y = (proj_height + lean); y < (max_height + lean); y++) {
            if(y >= 0) {
              framebuffer[(SCREEM_WIDTH * y) + i] = (uint8_t)colormap[mapoffset];
            }  
          }
          max_height = proj_height;
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
