#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos.h"

#define SCREEM_WIDTH 320
#define SCREEM_WEIGHT 200

int main(int argc, char* args[]) {
  setvideomode(videomode_320x200);

  setdoublebuffer(1);
  uint8_t* framebuffer = screenbuffer();

  while(!shuttingdown()) {
    waitubl();
    clearscreen();
    //
    //
    //
    //
    framebuffer = swapbuffers();

    if(keystate(KEY_ESCAPE))
      break;
  }
  return EXIT_SUCCESS;
}
