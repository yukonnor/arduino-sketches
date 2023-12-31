/*
 * SKETCH DESCRIPTION: Boucing equilateral triangle wiping. Creates mountains and valleys.
 * -----------------
 * 
 * todo: clear old wipes.
 *        - clear after certain time before current time (ex: 10s)
 *          ---> If it hasn't been covered since by triange (hard to figure out)
 *          ---> Clear pixels older than 10s that have NOT been covered by pixel drawn <10s ago
 *                          --->If x,y(>10s) is greater than x,y(ymax<10s), set pixel to black
 *        - clear after time based on pot value (map range 1s - 20s)
 * 
 */

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

//Name TVout class as "TV" to make easier to type
TVout TV;

//Define Analog Potentiometer Pins
const int pot1Pin = 0;
const int pot2Pin = 2; //note here that the stripboard wires cross, hence out of order
const int pot3Pin = 1;
const int pot4Pin = 3;

//Define Digital Button Pins
const int button1Pin = 13;
const int button2Pin = 12;
const int button3Pin = 11;
const int button4Pin = 10;

//Initialize button value variables:
int button1 = 0;
int button2 = 0;
int button3 = 0;
int button4 = 0;

//Define time variables
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

//Define Shape Location & Size Variables
uint8_t x; //x loction
uint8_t y; //y location
uint8_t s; //size of sides
int8_t xm; //x movement distance
int8_t ym; //y movement distance

//define 'fill' variable values
const int NO_FILL = -1;
const int FUZZ    = -2;
const int LINES   = -3;

/*  BLACK     = 0
 *  WHITE     = 1
 *  INVERT    = 2
 *  No Fill   = -1
 *  Fuzz Fill = -2
 *  LINES     = -3
 */

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  //Serial.begin(9600);
  
  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  
  TV.clear_screen();    

  //velocity and direction x & y move
  xm = random(-1,1);
  ym = random(-1,1); 
    //remove 0s
    if (xm == 0) {xm=1;}
    if (ym == 0) {ym=1;}

  //set initial location and size of triange
  x = random(35,80); //Should be between 35 and 85  
  y = random(35,60); //Should be between 30 and 66
  s = 50; //side length aka size of triange  
}

void loop() {

  //record values of pots 
  //use 10k pots
  int potRead1 = analogRead(pot1Pin); 
  int potRead2 = analogRead(pot2Pin);
  int potRead3 = analogRead(pot3Pin);

  //record values of buttons
  int buttonRead1 = digitalRead(button1Pin); //clear screen button

  //map pot1 values from 10 to 100 
  //controls tail length
  potRead1 = map(potRead1,0,1023,10,100);

  //map pot2 values from 1 to 3
  //controls x speed
  potRead2 = map(potRead2,0,1023,1,3);

  //map pot3 values from 1 to 3
  //controls y speed
  potRead3 = map(potRead3,0,1023,1,3);
  
  // Draw  repeating triangles
  for (int j=0; j<potRead1; j++) //j is how long the 'tail' goes for until it starts over 
  {
     //draw light triangles in old position (trails behind black triangle)
     equi_tri(x,y,s,WHITE,BLACK);
     
     x = x + (xm*potRead2);  //move x left or right
     if (x<=(s/2) || x>(TV.hres()-1)-(s/2)-1) xm = -xm; //if x hits edge, reverse direction
                                                        //subtr. 1 from hres as it was going out of screen
     y = y + (ym*potRead3);  //move y up or down
     if (y<=(s/2) || y>=TV.vres()-(s/2)-1) ym = -ym; //if y hits edge, reverse direction
  
     //draw black triangle in new position on top, moving above its tail
     equi_tri(x,y,s,WHITE,BLACK);

     TV.delay_frame(1);
    } 
  
  TV.clear_screen();   
}



/*
 *    SHAPE_TRAILS description:
 *    x,y is position of shape. 
 *    s is the side length (how large triange is) 
 *    
 */  
void shape_trails(uint8_t x, uint8_t y, uint8_t s, char c, char fc) {
//set shape logic
  int shape = 1; //1=triange, 2=square, 3=circle
//SHAPE BUTTON LOGIC
if (shp_read == HIGH &&  millis() - time > debounce) {
    if (shp == 1)
      shp = 2;
    else if (shp == 2)
         shp = 3;
         else       //if shp = 3
         shp = 1;  

    time = millis();    
  }
  
//DRAW SHAPES BASED ON SHP TOGGLE
if (shp == 1) {equi_tri(x,y,s,c,fc);  
              equi_tri(xr[i],yr[i],sr[i],c,fc);  
              }   
    else if (shp == 2) {TV.draw_rect(x+(s/2),y+(s/2),s,s,c,fc);
                        TV.draw_rect(xr[i]+(s/2),yr[i]+(s/2),sr[i],sr[i],c,fc);
                        }
                  else {TV.draw_circle(x,y,(s/2),c,fc);
                        TV.draw_circle(xr[i],yr[i],(sr[i]/2),c,fc);
                        }  

// Draw  repeating triangles
  for (int j=0; j<potRead1; j++) //j is how long the 'tail' goes for until it starts over 
  {
     //draw light triangles in old position (trails behind black triangle)
     equi_tri(x,y,s,WHITE,BLACK);
     
     x = x + (xm*potRead2);  //move x left or right
     if (x<=(s/2) || x>(TV.hres()-1)-(s/2)-1) xm = -xm; //if x hits edge, reverse direction
                                                        //subtr. 1 from hres as it was going out of screen
     y = y + (ym*potRead3);  //move y up or down
     if (y<=(s/2) || y>=TV.vres()-(s/2)-1) ym = -ym; //if y hits edge, reverse direction
  
     //draw black triangle in new position on top, moving above its tail
     equi_tri(x,y,s,WHITE,BLACK);

     TV.delay_frame(1);
    } 


} //end shape_trails



/*
 *    EQUI_TRI description:
 *    x,y is center point of triange. Halfway x and y directions
 *    s is the side length (how large triange is) 
 *    
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

void draw_tri(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, char c, char fc) {
  unsigned char i = y1;
  float xrow0 = (x1 + (((y1-i)*(x1-x3))/(y1+y3)));
  float xrow1 = (x1 + (((y1-i)*(x1-x2))/(y1+y2)));

  TV.draw_line(x1,y1,x2,y2,c);
  TV.draw_line(x2,y2,x3,y3,c);
  TV.draw_line(x1,y1,x3,y3,c);
} //end of draw_tri

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*


//SHAPE BUTTON LOGIC
if (shp_read == HIGH &&  millis() - time > debounce) {
    if (shp == 1)
      shp = 2;
    else if (shp == 2)
         shp = 3;
         else       //if shp = 3
         shp = 1;  

    time = millis();    
  }
  
//DRAW SHAPES BASED ON SHP TOGGLE
if (shp == 1) {equi_tri(x,y,s,c,fc);  
              equi_tri(xr[i],yr[i],sr[i],c,fc);  
              }   
    else if (shp == 2) {TV.draw_rect(x+(s/2),y+(s/2),s,s,c,fc);
                        TV.draw_rect(xr[i]+(s/2),yr[i]+(s/2),sr[i],sr[i],c,fc);
                        }
                  else {TV.draw_circle(x,y,(s/2),c,fc);
                        TV.draw_circle(xr[i],yr[i],(sr[i]/2),c,fc);
                        }

//ANIMATION MODE BUTTON LOGIC
if (aniModePin_read == HIGH &&  millis() - time > debounce) {
    if (animation_mode == 0)
      animation_mode = 1;
    else if (animation_mode == 1)
         animation_mode = 2;
         else       //if animation_mode = 2
         animation_mode = 0;  

    time = millis();    
  }


//CLEAR SCREEN BUTTON LOGIC
if (clear_scn_toggle_read == HIGH &&  millis() - time > debounce) {
    if (clear_scn_toggle == 1)
      clear_scn_toggle = 0;
    else clear_scn_toggle = 1;
    }  

    time = millis();    
  }
  


*/  
