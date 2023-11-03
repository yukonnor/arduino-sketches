#include "CompositeGraphics.h"
#include "CompositeOutput.h"

//PAL MAX, half: 324x268 full: 648x536
//NTSC MAX, half: 324x224 full: 648x448
const int XRES = 320;
const int YRES = 200;
const int balls = 100;

//Graphics using the defined resolution for the backbuffer
CompositeGraphics graphics(XRES, YRES);
//Composite output using the desired mode (PAL/NTSC) and twice the resolution. 
//It will center the displayed image automatically
CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

#include <soc/rtc.h>

void setup()
{
  //highest clockspeed needed
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
  
  //initializing DMA buffers and I2S
  composite.init();
  //initializing graphics double buffer
  graphics.init();

  //running composite output pinned to first core
  xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);
  //rendering the actual graphics in the main loop is done on the second core by default
}

void compositeCore(void *data)
{
  while (true)
  {
    //just send the graphics frontbuffer whithout any interruption 
    composite.sendFrameHalfResolution(&graphics.frame);
  }
}

void draw()
{
  // clearing background and starting to draw
  // argument = clear color (0==black, 55==white)
  graphics.begin(0);

  /*
  // attempt a redrawing previous frame (in order not to erase)
  for(int y = 0; y < YRES; y++)
        for(int x = 0; x < XRES; x++)
          graphics.backbuffer[y][x] = graphics.frame[y][x];
  */        
  graphics.dot(random(XRES),random(YRES),54); // draw white dot in random location
  graphics.dot(random(XRES),random(YRES),54); // draw white dot in random location
  graphics.dot(random(XRES),random(YRES),54); // draw white dot in random location
  
  /*
  //drawing some lines
  for(int i = 0; i <= 100; i++)
  {
    graphics.line(50, i + 60, 50 + i, 160, i / 2);
    graphics.line(150, 160 - i, 50 + i, 60, i / 2);
    delay(5);
  }
  */
  
  //finished drawing, swap back and front buffer to display it
  graphics.end();
  delay(20);
}

void loop()
{
  int x1 = 50;
  int x2 = 110;
  int x3 = 170;
  int x4 = 230;
  int y1 = graphics.yres / 2;
  int r1 = 10;
  int r2 = 30;
  unsigned int x[balls];
  unsigned int y[balls];
  char dx[balls];
  char dy[balls];
  char c[balls];
  char v[balls];
  char s1 = 1;
  char s2 = 1;
  int cnt = 0;

  
  graphics.begin(0);

  // bounce rects on screen borders
  for (cnt = 0; cnt < balls; cnt++)
  {
    graphics.fillRect(x[cnt], y[cnt], 10, 10, 70);
    if (dx[cnt] == 0){x[cnt]-= v[cnt];}else{x[cnt]+= v[cnt];}
    if (x[cnt] >= (graphics.xres - 5)) { x[cnt] = graphics.xres - 5; dx[cnt] = 0; v[cnt] = (rand() % 5) + 1;}
    if (x[cnt] <= 5) { x[cnt] = 5; dx[cnt] = 1; v[cnt] = (rand() % 5) + 1;}

    if (dy[cnt] == 0){y[cnt]-= v[cnt];}else{y[cnt]+= v[cnt];}
    if (y[cnt] >= (graphics.yres - 5)) { y[cnt] = graphics.yres - 5; dy[cnt] = 0; v[cnt] = (rand() % 5) + 1;}
    if (y[cnt] <= 5) { y[cnt] = 5; dy[cnt] = 1; v[cnt] = (rand() % 5) + 1;}
  }


  if (s1 == 1) r1++; else r1--;
  if (s2 == 1) r2++; else r2--;

  if (r1 >= 50) s1 = 0;
  if (r2 >= 50) s2 = 0;

  if (r1 <= 2) s1 = 1;
  if (r2 <= 2) s2 = 1;

  graphics.end();

}
