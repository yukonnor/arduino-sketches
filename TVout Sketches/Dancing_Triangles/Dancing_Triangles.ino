#include <TVout.h>
#include <video_gen.h>

#include <TVout.h>
#include <video_gen.h>

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;
int interval = 1000;
int prnt = 0;
int NO_FILL = -1;
int FUZZ = -2;
int LINES = -3;

/*      BLACK	=0
 *	WHITE	=1
 *	INVERT	=2
 *      No Fill =-1
 *      Fuzz Fill =-2
 *      LINES = -3
 */

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  Serial.begin(9600);
  //circles
  TV.clear_screen();
 // TV.delay(500);
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/4,WHITE,WHITE);
  TV.delay(500);
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/2,WHITE,NO_FILL);//NO FILL
  TV.delay(500);
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/2,WHITE,BLACK); //BLACK FILL
  TV.delay(1000);
  TV.clear_screen(); 
    
}

void loop() {
TV.clear_screen();
//TV.delay_frame(5);
int potRead = analogRead(A0); //use 10k pot - Values should be 0 - 1023
int x = 40+(potRead*.04);
int y = 48;
int s = 70;
float p = 0.15+(potRead*-.0008);   //p should be between -.2 and .2

    //equi_tri(60,48,70,WHITE,WHITE);    
   
   hallway_effect(x,y,s,p,WHITE,LINES);
   //delay(16);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_tri(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, char c, char fc) {
  unsigned char i = y1;
  float xrow0 = (x1 + (((y1-i)*(x1-x3))/(y1+y3)));
  float xrow1 = (x1 + (((y1-i)*(x1-x2))/(y1+y2)));

  TV.draw_line(x1,y1,x2,y2,c);
  TV.draw_line(x2,y2,x3,y3,c);
  TV.draw_line(x1,y1,x3,y3,c);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void equi_tri(uint8_t x, uint8_t y, uint8_t s, char c, char fc) {
  //1=bottom right
  //2=bottom left
  //3=top center
  
  uint8_t x1 = (x+(s/2));
  uint8_t y1 = (y+(s/(2*sqrt(3))));
  uint8_t x2 = (x-(s/2));
  uint8_t y2 = (y+(s/(2*sqrt(3))));
  uint8_t x3 = x;
  uint8_t y3 = (y-(2*(s/(2*sqrt(3)))));
  
  //Fill Color
  if (fc > -1 && fc < 3) {
    for (unsigned char i = y3; i < y1; i++) {
        uint8_t xrow0 = x3 - (0.57735*(i-y3));
        uint8_t xrow1 = x3 + (0.57735*(i-y3));
        TV.draw_row(i,xrow0,xrow1,fc); }
  }
  
  //Fuzz Fill 
  if (fc == -2) {
    for (unsigned char i = y3; i < y1; i++) {
        uint8_t xrow0 = x3 - (0.57735*(i-y3));
        uint8_t xrow1 = x3 + (0.57735*(i-y3));
        uint8_t xrand = random(xrow0,xrow1); 
        TV.set_pixel(xrand,i,WHITE);
        xrand = random(xrow0,xrow1);
        TV.set_pixel(xrand,i,WHITE);
        xrand = random(xrow0,xrow1);
        TV.set_pixel(xrand,i,WHITE);  }}   //ADDED 3 Set_pixels to fill more of the tiangle

  //LINES Fill
  if (fc = -3) {
      for (unsigned char i = y3; i < y1; i++) {
        uint8_t xrow0 = x3 - (0.57735*(i-y3));
        uint8_t xrow1 = x3 + (0.57735*(i-y3));
        if ((i%2)==0) {TV.draw_row(i,xrow0,xrow1,WHITE);}  
          }
  } 
 
  TV.draw_line(x1,y1,x2,y2,c);
  TV.draw_line(x2,y2,x3,y3,c);
  TV.draw_line(x1,y1,x3,y3,c);

  
} // end of equi_tri


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* hallway_effect: draw a series of triangles to add a 'hallway' perspective
 * 
 * Arguments:
 *    x,y:      The x,y coordinates of centerpoint of the main equi triangle
 *    
 *      s:      The length of sides of the main equilateral triangle
 *              All other triangles will use a fraction of this for their side length
 *
 *      p:      Perspective ratio. How far each triangle is from eachother.
 *
 *      c:      The color of the triange lines.
 *    
 *     fc:      The fill color of the triangle
 *     
 *     CONSIDER: Adding all shapes to hallway_effect. Not sure how this would affect performance. Would have to add variable for shape
*/
void hallway_effect (uint8_t x, uint8_t y, uint8_t s, float p, char c, char fc) {
  uint8_t REPEATS = 8;
  float ratio = .75;
  
  uint8_t s_r[REPEATS];
  uint8_t s_r_last = s;
  uint8_t x_r[REPEATS];
  uint8_t x_r_last = x;
  uint8_t y_r[REPEATS];
  uint8_t y_r_last = y;
  uint8_t  i;       // counter
  
  int alternate = 0; //0 is off, 1 is alternate
  int fc_r = fc;
  int fc_alt = WHITE;
  int fc_alt_last = fc;
  
  int dtime = 1;
  TV.delay_frame(1);

 
    if (alternate == 0) {
    for (i=0; i<REPEATS; i++) {
      //equi_tri(x_r_last,y_r_last,s_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
       
      s_r[i]=s_r_last*.75; //s*(pow(ratio,i))*(pow(ratio,(i-1)));
      x_r[i]=x_r_last+(p*(s_r_last/1.5));
      y_r[i]=y_r_last+(p*(s_r_last/2));

      s_r_last = s_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];

      equi_tri(x_r[i],y_r[i],s_r[i],c,fc);
      } }

    if (alternate == 1) {
    for (i=0; i<REPEATS; i++) {
      equi_tri(x_r_last,y_r_last,s_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
      
      s_r[i]=s_r_last*.75; //s*(pow(ratio,i))*(pow(ratio,(i-1)));
      x_r[i]=x_r_last+(p*(s_r_last/1.5));
      y_r[i]=y_r_last+(p*(s_r_last/2));

      s_r_last = s_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];
      
      //Alternate background colors if i is even.
      if ((i % 2) == 0) {
              fc_r = fc_alt;
            } 
            else {
              fc_r = fc;
            }  
      equi_tri(x_r[i],y_r[i],s_r[i],c,fc_r);
      } }
  
} 

/*
void hallway_effect_sq(uint8_t x0, uint8_t y0, uint8_t w, float p, char c, char fc) {
  uint8_t REPEATS = 8;
  float ratio = .75;
  
  uint8_t w_r[REPEATS];
  uint8_t w_r_last = w;
  uint8_t x_r[REPEATS];
  uint8_t x_r_last = x0;
  uint8_t y_r[REPEATS];
  uint8_t y_r_last = y0;
  uint8_t  i;       // counter
  
  int alternate = 1; //0 is off, 1 is alternate
  int fc_r = fc;
  int fc_alt = WHITE;
  int fc_alt_last = fc;
  
  int dtime = 1;

 TV.delay_frame(1);
 
    if (alternate == 0) {
    for (i=0; i<REPEATS; i++) {
      TV.draw_rect(x_r_last,y_r_last,w_r_last,w_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
      
      w_r[i]=w_r_last*.75; 
      x_r[i]=x_r_last+(p*(w_r_last/1.5));
      y_r[i]=y_r_last+(p*(w_r_last/2));

      w_r_last = w_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];

      TV.draw_rect(x_r[i],y_r[i],w_r[i],c,fc);
      } }

    if (alternate == 1) {
    for (i=0; i<REPEATS; i++) {
      equi_tri(x_r_last,y_r_last,s_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
      
      s_r[i]=s_r_last*.75; //s*(pow(ratio,i))*(pow(ratio,(i-1)));
      x_r[i]=x_r_last+(p*(s_r_last/1.5));
      y_r[i]=y_r_last+(p*(s_r_last/2));

      s_r_last = s_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];

      if ((i % 2) == 0) {fc_r = fc_alt;} else {fc_r = fc;} //Alternate background colors if i is even. 
      equi_tri(x_r[i],y_r[i],s_r[i],c,fc_r);
      } }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* OLD LOOP 
 * //int potRead = analogRead(A0); //use 10k pot - Values should be 0 - 1023

//Triangle 1 Initial State
int t1x1 = 10;
int t1y1 = 10;
int t1x2 = 20;
int t1y2 = 80;
int t1x3 = 80;    //For now, picked consant. Should aim for values between 80 - 100
int t1y3 = 20; //int((t1y1+(potRead*(t1y2/1023))));   //Should aim for values between t1y1 and t1y2

//Triangle 2 Initial State
int t2x1 = (120-t1x1); //Needs to be between 10 and 110 - GOOD
int t2y1 = t1y1+10;
int t2x2 = 90;
int t2y2 = t2y1+25;
int t2x3 = (120-t1x3);
int t2y3 = t1y3+25;

draw_tri(t1x1,t1y1,t1x2,t1y2,t1x3,t1y3,WHITE,BLACK); //draw t1
draw_tri(t2x1,t2y1,t2x2,t2y2,t2x3,t2y3,WHITE,BLACK); //draw t2
delay(500);

for(t1x1=10; t1x1<110; t1x1++) {  //RUNS
int a= t1x1;
int b= t1y1;
int c= t1x2;
int d= t1y2;
int e= t1x3;   
int f= t1y3;

int g= (t2x1-(t1x1/1.5)); 
int h= t2y1;
int j= t2x1;
int k= (30+(t1x1/2));
int l= t1x1;
int m= t2y3;
  
  draw_tri(a,b,c,d,e,f,WHITE,BLACK);
  TV.draw_circle(t1x2,t1y2,(20),WHITE,INVERT);
  draw_tri(g,h,j,k,l,m,WHITE,BLACK);
  delay(16); //Control rate of animation with pot value
  TV.clear_screen();
}

Serial.print(t1y2);

for(t1y2=80; t1y2>5; t1y2--) { 
int a= (50+t1y2);
  a = constrain (a,10,100);
int b= (-t1y2/2);
  b = constrain (b,10,86);
int c= t1x2;
  c = constrain (c,10,100);
int d= t1y2;
  d = constrain (d,10,86);
int e= t1x3;
  e = constrain (e,10,100);   
int f= (-t1y2/2);
  f = constrain (f,10,86);

int g= (t2x1-(t1x1/1.5)); 
int h= t2y1;
int j= t2x1;
int k= (30+(t1x1/2));
int l= t1x1;
int m= t2y3;
  
  draw_tri(a,b,c,d,e,f,WHITE,BLACK); //small fast moving up
  draw_tri(t2x1,(t1y1-(t1y2)),(t1x2+(t1y2/2)),t2y2,(t2x3+(t1y2/4)),t1y2,WHITE,BLACK); //big horizontal
  TV.draw_circle(75,(95-(t1y2/4)),(15-(t1y2/10)),WHITE,INVERT);
  delay(16);
  TV.clear_screen();
}

Serial.print(t1x2);

/*for(t1x2; t1x2<80; t1x2++) {  //RUNS
  draw_tri(t1x1,t1y1,t1x2,t1y2,t1x3,t1y3,WHITE,BLACK);
  TV.draw_circle(t1x2,t1y2,(10),WHITE,BLACK);
  draw_tri(t2x1,t2y1,(110-(t1x2/3)),(35+t1x2/10),t2x3,t2y3,WHITE,BLACK);
  delay(16);
  TV.clear_screen();
}*/


/*for(t1y2; t1y2>20; t1y2--) {
  draw_tri(t1x1,t1y1,t1x2,t1y2,t1x3,t1y3,WHITE,BLACK);
  draw_tri(t2x1,(15-(t1y2/4)),t2x2,t2y2,(t1y2/25),t2y3,WHITE,BLACK);
  delay(16);
  TV.clear_screen();

}*/



/*
  if(prnt==0) {
  Serial.print(y3);
  Serial.print("break");
  Serial.print(xrow0);
  Serial.print("break");
  Serial.print(xrow1);
  prnt = 1; }
  */
  
  /* OLD HALLWAY EFFECT STYFF
  
   /*
  if (alternate == 0) {
    //TV.delay_frame(0);
    equi_tri(x, y, s, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x1, he_y1, s1, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x2, he_y2, s2, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x3, he_y3, s3, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x4, he_y4, s4, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x5, he_y5, s5, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x6, he_y6, s6, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x7, he_y7, s7, c, fc);
    //TV.delay_frame(1); 
     }
  
  if (alternate == 1) {
    //TV.delay_frame(0);
    equi_tri(x, y, s, c, fc);
    TV.delay_frame(dtime);
    equi_tri(he_x1, he_y1, s1, c, fc_alt);
    //TV.delay_frame(dtime);
    equi_tri(he_x2, he_y2, s2, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x3, he_y3, s3, c, fc_alt);
    TV.delay_frame(dtime);
    equi_tri(he_x4, he_y4, s4, c, fc);
    //TV.delay_frame(dtime);
    equi_tri(he_x5, he_y5, s5, c, fc_alt);
    //TV.delay_frame(dtime);
    equi_tri(he_x6, he_y6, s6, c, fc);
    TV.delay_frame(dtime);
    equi_tri(he_x7, he_y7, s7, c, fc_alt);
    //TV.delay_frame(1); 
     }   */
  
   /*int s1 = s*.75;
  int s2 = s1*.75;
  int s3 = s2*.75;
  int s4 = s3*.75;
  int s5 = s4*.75;
  int s6 = s5*.75;
  int s7 = s6*.75;
  
  int x1 = x+(p*(s/1.5));
  int x2 = he_x1+(p*(s1/1.5));
  int x3 = he_x2+(p*(s2/1.5));
  int x4 = he_x3+(p*(s3/1.5));
  int x5 = he_x4+(p*(s4/1.5));
  int x6 = he_x5+(p*(s5/1.5));
  int x7 = he_x6+(p*(s6/1.5));
  
  
  
  int y1 = y+(p*(s/2));
  int y2 = y1+(p*(s1/2));
  int y3 = y2+(p*(s2/2));
  int y4 = y3+(p*(s3/2));
  int y5 = y4+(p*(s4/2));
  int y6 = y5+(p*(s5/2));
  int y7 = y6+(p*(s6/2)); */
  
