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
  //Serial.begin(9600);
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
//sqrt of 3 is 1.732


void loop() {
TV.clear_screen();
//TV.delay_frame(5);
int potRead1 = analogRead(A0); //use 10k pot - A0 Values will be 0 - 1023
int potRead2 = analogRead(A1);
int potRead3 = analogRead(A2);

int s = 70;
int x = (s/2)+(potRead1*0.048); //Should be between 35 and 85   (120-s)/1023
int y = 30+(potRead2*.035);//y should be between 30 --- (1.5*(s/(2*sqrt(3)))) and 66 --96-(1.5*(s/(2*sqrt(3))))

float p = -10+(potRead3*0.0195);   //p should be between -10 and 10

   //equi_tri(x,y,70,WHITE,WHITE);    
   
   hallway_effect(x,y,s,p,WHITE,BLACK);
   //delay(16);
 Serial.println(potRead1);
 
 //(y-(2*(s/(2*sqrt(3))))); top   (y-(2*(s/(2*sqrt(3)))))==0 when pot = 0
 //(y+(s/(2*sqrt(3)))); bottom
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

/*
 *    x,y is center point of triange. Halfway x and y directions
 *    if s is 2, 
 */
void equi_tri(uint8_t x, uint8_t y, uint8_t s, char c, char fc) {
  //1=bottom right
  //2=bottom left
  //3=top center
  
  uint8_t x1 = x+(s/2);
  uint8_t y1 = y+(1.5*(s/(2*sqrt(3))));
  uint8_t x2 = x-(s/2);
  uint8_t y2 = y+(1.5*(s/(2*sqrt(3))));
  uint8_t x3 = x;
  uint8_t y3 = y-(1.5*(s/(2*sqrt(3))));
  
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
  if (fc == -3) {
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
 *    x,y:      The x,y coordinates of top point of the main equi triangle
 *    
 *      s:      The length of sides of the main equilateral triangle
 *              All other triangles will use a fraction of this for their side length
 *
 *      p:      Perspective ratio. How far each triangle is from eachother in x direction
 .
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
  int qx[REPEATS];
  int qx_last = (60-x);
  uint8_t y_r[REPEATS];
  uint8_t y_r_last = y;
  int qy[REPEATS];
  int qy_last = (48-y);
  uint8_t  i;       // counter
  
  int alternate = 1; //0 is off, 1 is alternate
  int fc_r = fc;
  int fc_alt = WHITE;
  int fc_alt_last = fc;
  
  int dtime = 1;
  TV.delay_frame(1);

  equi_tri(x,y,s,c,fc);
     
    if (alternate == 0) {
    for (i=0; i<REPEATS; i++) {
      //equi_tri(x_r_last,y_r_last,s_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
       
      s_r[i]=s_r_last*.75; //s*(pow(ratio,i))*(pow(ratio,(i-1)));
      x_r[i]=x_r_last+(qx_last);   //60 = 84 + (-24)
      y_r[i]=y_r_last+(qy_last); //y+((48-y)*(i*.25)); //y_r_last+(s_r[i]/3.46);  //y Should be between 30 and 66
      qx[i]= (qx_last)*0.75;  // (-24 * .75) = -18
      qy[i]= qy_last*.75;
      
      //Serial.println(y);
      Serial.print("x_r[i]="); Serial.println(x_r[i]);
      Serial.println("done");
      
      s_r_last = s_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];
      qx_last = qx[i]; // = -18
      qy_last = qy[i];

      equi_tri(x_r[i],y_r[i],s_r[i],c,fc);
      } }

    if (alternate == 1) {
    for (i=0; i<REPEATS; i++) {
      //equi_tri(x_r_last,y_r,s_r_last,BLACK,BLACK); //Attempt clearing individual triangles rather than clear screen 
      
      s_r[i]=s_r_last*.75; //s*(pow(ratio,i))*(pow(ratio,(i-1)));
      x_r[i]=x_r_last+(qx_last);   //60 = 84 + (-24)
      y_r[i]=y_r_last+(qy_last); //y+((48-y)*(i*.25)); //y_r_last+(s_r[i]/3.46);  //y Should be between 30 and 66
      qx[i]= (qx_last)*0.75;  // (-24 * .75) = -18
      qy[i]= qy_last*.75;
      
      //Serial.println(y);
      Serial.print("x_r[i]="); Serial.println(x_r[i]);
      Serial.println("done");
      
      s_r_last = s_r[i];
      x_r_last = x_r[i];
      y_r_last = y_r[i];
      qx_last = qx[i]; // = -18
      qy_last = qy[i];
      
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
}   */

 
