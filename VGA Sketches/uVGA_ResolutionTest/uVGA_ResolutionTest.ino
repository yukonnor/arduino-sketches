// ResolutionTest
// test the resolution of the output on the screen

// the delay() calls are here only to let you see lines misplacement when DMA is delayed (if they exists)
// Despite this, the image stays visible and the monitor is not losing its sync
#include <uVGA.h>

uVGA uvga;

// Defining this (UVGA_DEFAULT_REZ) will use the default resolution set for your CPU frequency.
// You can choose a different modeline by changing the define. Accepted values are in uVGA_valid_settings.h

#define UVGA_DEFAULT_REZ
#include <uVGA_valid_settings.h>

void setup()
{
  
  int ret; 
  ret = uvga.begin(&modeline);

  Serial.println(ret);

  if(ret != 0)
  {
    Serial.println("fatal error");
    while(1);
  }

  int fb_width, fb_height;
  uvga.get_frame_buffer_size(&fb_width, &fb_height);
  uvga.print("Frame Buffer H size = "); uvga.println(fb_width);
  uvga.print("Frame Buffer V size = "); uvga.println(fb_height);
  delay(3000);
}

float a=0;

int XRES = 640;
int YRES = 480;

void loop()
{ 
  int fb_width, fb_height;
  static uvga_text_direction dir[] = { UVGA_DIR_RIGHT, UVGA_DIR_TOP, UVGA_DIR_LEFT, UVGA_DIR_BOTTOM };

  uvga.get_frame_buffer_size(&fb_width, &fb_height);

  // Draw horizontal lines 100px appart
  uvga.drawLine(0,XRES,0,0,random(512)-256);
  uvga.drawLine(0,XRES,100,100,random(512)-256);
  uvga.drawLine(0,XRES,200,200,random(512)-256);
  uvga.drawLine(0,XRES,300,300,random(512)-256);
  uvga.drawLine(0,XRES,400,400,random(512)-256);
  uvga.drawLine(0,XRES,500,500,random(512)-256);
  
  // Draw vertical lines 100px appart
  uvga.drawLine(0,0,0,YRES,random(512)-256);
  uvga.drawLine(100,100,0,YRES,random(512)-256);
  uvga.drawLine(200,200,0,YRES,random(512)-256);
  uvga.drawLine(300,300,0,YRES,random(512)-256);
  uvga.drawLine(400,400,0,YRES,random(512)-256);
  uvga.drawLine(500,500,0,YRES,random(512)-256);
  uvga.drawLine(600,600,0,YRES,random(512)-256);
  uvga.drawLine(700,700,0,YRES,random(512)-256);

  //delay(100);
  
  //delay(1000/60);
}
