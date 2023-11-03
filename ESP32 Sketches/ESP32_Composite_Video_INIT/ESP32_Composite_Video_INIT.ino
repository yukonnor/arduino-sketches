#include "CompositeGraphics.h"
#include "CompositeOutput.h"

const int XRES = 320;
const int YRES = 240;

char frame_buffer[XRES][YRES];

CompositeGraphics graphics(XRES, YRES);
CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

// images included here
#include "Image.h"
#include "luni.h"
Image<CompositeGraphics> luni0(luni::xres, luni::yres, luni::pixels);

#include <soc/rtc.h>

void compositeCore(void *data)
{
  while (true)
  {
    //just send the graphics frontbuffer whithout any interruption 
    composite.sendFrameHalfResolution(&graphics.frame);
  }
}

void fastStart(){
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
  composite.init();
  graphics.init();
  xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);

  graphics.begin(0);
  graphics.end();
}

void fillBuffer(){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      //fill the buffer whith what's currently displayed on the frame
      frame_buffer[i][j] = graphics.frame[j][i];
    }
  }
}

void dumpBuffer(){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      //fill the buffer whith what's currently displayed on the frame
      graphics.backbuffer[j][i] = frame_buffer[i][j];
    }
  }
}

void clearScreen(int c = 0){
  graphics.begin(c);
  graphics.end();
}

//////////////////////////////////////////////////////////////////////////////////////////

void draw()
{
  graphics.begin(0);
  //dump frame buffer here if ya want
  dumpBuffer();

  graphics.scrollSreen(10,0,0);
  
  //luni0.draw(graphics, 200, 10);

  //graphics.drawLine(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),54);
  //graphics.drawCircle(random(0,XRES),random(0,YRES),random(60),54);
  //graphics.fillCircle(random(0,XRES),random(0,YRES),random(20),20);
  //graphics.fillCircleGrad(random(0,XRES),random(0,YRES),random(60),0,0,30);
  //graphics.drawEllipse(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),54);
  //graphics.fillEllipse(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),20);
  //graphics.fillEllipseGrad(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),0,30,0);
  //graphics.drawArc(30,30,random(10,30),random(4),54);
  //graphics.drawThicArc(200,200,20,random(4),4,54);

  graphics.drawRect(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),54);
  //graphics.fillRect(random(0,XRES/2),random(0,YRES/2),random(XRES/2,XRES),random(YRES/2,YRES),20);
  graphics.fillRectGrad(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),0);
  graphics.fillRectGrad(random(0,XRES),random(0,YRES),random(0,XRES),random(0,YRES),1);
  
  graphics.end();
  //fill frame buffer here if you want to keep what you drew
  fillBuffer();
}

void setup()
{
  fastStart();
  fillBuffer();
  Serial.begin(115200);
}


void loop()
{
  draw();
  delay(200);

}
