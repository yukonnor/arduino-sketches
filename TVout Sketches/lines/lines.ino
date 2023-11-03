#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  //Serial.begin(9600);

  
  TV.clear_screen(); 
    
}



void loop() {

int x1 = random(20,40);
int x2 = random(55,70);
int x3 = random(85,90);

int y =  random(15,40);
int y1 = random(70,90);

int yrow = random(15,85);

TV.draw_row(yrow,0,120,INVERT);
yrow = random(15,85);
TV.draw_row(yrow,0,120,INVERT);
yrow = random(15,85);
TV.draw_row(yrow,0,120,INVERT);
yrow = random(15,85);
TV.draw_row(yrow,0,120,INVERT);
yrow = random(15,85);
TV.draw_row(yrow,0,120,INVERT);
yrow = random(15,85);
TV.draw_row(yrow,0,120,INVERT);\
yrow = random(15,85);

int xrect = random(15,40);
int xrect1 = 120-(2*xrect);
//TV.draw_rect(xrect,15,xrect1,70,WHITE,INVERT);

TV.draw_line(0,0,xrect,15,INVERT);
//TV.draw_line(0,0,(xrect1+xrect),15,INVERT);
//TV.draw_line(0,0,xrect,85,INVERT);
TV.draw_line(0,0,(xrect1+xrect),85,INVERT);

TV.draw_line(120,96,xrect,15,INVERT);
//TV.draw_line(120,96,(xrect1+xrect),15,INVERT);
//TV.draw_line(120,96,xrect,85,INVERT);
TV.draw_line(120,96,(xrect1+xrect),85,INVERT);


TV.delay_frame(1);  
TV.clear_screen(); 

}




/* DOPE JUST WITH THESE TWO LINES
int x1 = random(20,40);
int x2 = random (55,70);
int x3 = random (85,90);

int y = random(15,40);

int y1 = y;
int y2 = y;
int y3 = y;
  
  
TV.clear_screen(); 
//TV.delay_frame(1);



TV.draw_line(0,0,x1,y1,WHITE);
TV.draw_line(120,96,x1,y1,WHITE);
TV.draw_line(0,0,x1,y1,WHITE);
TV.draw_line(0,0,x1,y1,WHITE);

*/
