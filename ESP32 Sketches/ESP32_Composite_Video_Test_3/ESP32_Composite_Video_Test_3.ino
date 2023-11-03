#include "CompositeGraphics.h"
#include "CompositeOutput.h"

//PAL MAX, half: 324x268 full: 648x536
//NTSC MAX, half: 324x224 full: 648x448
const int XRES = 320;
const int YRES = 200;

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
  draw();

}
