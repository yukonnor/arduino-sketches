/*
 * What this does:
 * - Draws random shapes
 */

#include <VGA.h>

// DrawingTest
// test of all drawing functions

int h_res_buff = 790;
int v_res_buff = 590;

void setup() {
  VGA.begin(800,600,VGA_MONO);
}


void loop() {
  VGA.clear();
  int x0=random(10,h_res_buff),y0=random(10,v_res_buff),x1=random(10,h_res_buff),y1=random(10,v_res_buff),x2=random(10,h_res_buff),y2=random(10,v_res_buff);
  VGA.fillTri(x0,y0,x1,y1,y2,y2,random(-1,2));
  VGA.drawTri(x0,y0,x1,y1,y2,y2,-1); // -1 color = invert

  x0=random(h_res_buff);y0=random(v_res_buff);x1=random(h_res_buff);y1=random(v_res_buff);
  VGA.fillRect(x0,y0,x1,y1,random(-1,2));
  VGA.drawRect(x0,y0,x1,y1,-1);   
  
  x0=random(h_res_buff);y0=random(v_res_buff);x1=random(h_res_buff);y1=random(v_res_buff);
  VGA.fillEllipse(x0,y0,x1,y1,random(-1,2));
  VGA.drawEllipse(x0,y0,x1,y1,-1);
    
  x0=random(h_res_buff);y0=random(v_res_buff);int r=random(70);
  VGA.fillCircle(x0,y0,r,random(-1,2));
  VGA.drawCircle(x0,y0,r,-1);
  
  delay(2000);
}
