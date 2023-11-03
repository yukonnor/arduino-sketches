/*
 * TOP SKETCH: Boucing equilateral triangle wiping. Creates mountains and valleys.
 * ----------
 *             todo: clear old wipes.
 *                   - clear after certain time before current time (ex: 10s)
 *                   - clear after time based on pot value (map range 1s - 20s)
 *             
 *             
 * 
 * BOTTOM SKETCH: Seems to just be logic around buttons. No animations. 
 * -------------
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


//define 'fill' variable values
const int NO_FILL = -1;
const int FUZZ    = -2;
const int LINES   = -3;

/*  BLACK  =0
 *  WHITE =1
 *  INVERT  =2
 *  No Fill =-1
 *  Fuzz Fill =-2
 *  LINES = -3
 */

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  //Serial.begin(9600);
  
  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  
  TV.clear_screen();     
}

//Retry draw black triangle to reduce flicker
//Add: animation_state: 0 is normal, 1 is x-direction snake (width of x pot read)
//                      2 is y-direction (width of y pot read)


void loop() {

  //record values of pots 
  //use 10k pots
  int potRead1 = analogRead(pot1Pin); 
  int potRead2 = analogRead(pot2Pin);
  int potRead3 = analogRead(pot3Pin);

  //record values of buttons
  int buttonRead1 = digitalRead(button1Pin);

  uint8_t s = 70;
  uint8_t x = (s/2)+(potRead1*0.048); //Should be between 35 and 85   (120-s)/1023
  uint8_t y = 30+(potRead2*.035);//y should be between 30 --- (1.5*(s/(2*sqrt(3)))) and 66 --96-(1.5*(s/(2*sqrt(3))))

  float p = -10+(potRead3*0.0195);   //p should be between -10 and 10

  //equi_tri(x,y,70,WHITE,WHITE);    
  hallway_effect(x,y,s,p,WHITE,BLACK);

  //CLEAR SCREEN BUTTON LOGIC
  //if button 1 is pressed, clear screen
  if (buttonRead1 == LOW){// &&  millis() - time > debounce) {
      button1 = button1 + 1;
      delay(20);
      TV.clear_screen(); 
      time = millis(); //set 'time' to be current time   
  }

  Serial.print("Button1 = "); 
  Serial.println(button1);  
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* hallway_effect_triangles: draw a series of triangles to add a 'hallway' perspective
 * 
 * Arguments:
 *    x,y:      The x,y coordinates of top point of the main triangle/square/circle
 *    
 *      s:      The length of sides of the main equilateral triangle
 *              All other triangles will use a fraction of this for their side length
 *
 *      p:      Perspective ratio. How far each triangle is from eachother in x direction
 *      
 *      shp:    Shape toggle. Toggles through triangle (1), square (2), circle (3)
 *
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

  uint8_t shp = 1; // ADD TOGGLE BUTTON 1=tri 2=squar 3=circle
  
  uint8_t sr[REPEATS];
  uint8_t xr[REPEATS];
  int     xc = x;
  uint8_t yr[REPEATS];
  int     yc = y;
  uint8_t  i;       // counter

  int8_t   xm[REPEATS], ym[REPEATS]; 
  
  int alternate = 1; //0 is off, 1 is alternate
  int fc_r = fc;
  int fc_alt = WHITE;
  int fc_alt_last = fc;
  
  int animation_mode = 1;  

  int clear_scn_toggle = 1; //Add button toggle
  
  int dtime = 1;
  

  //NORMAL ANIMATION - TRIANGLES FOLLOW POT READING
  if (animation_mode == 0) {
       
     for (i=0; i<REPEATS; i++)
     {       
       TV.delay_frame(1);
       if (clear_scn_toggle == 1) {TV.clear_screen();} //clear screen if clear_scn_toggle == 1     
       
       // XY center point positions of repeating shapes
       sr[i] = s * .75;
       xr[i] = x + ((60-xc)*(pow(.75,i)));
       yr[i] = y + ((48-yc)*(pow(.75,i)));

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

       s = sr[i];
       x = xr[i];
       y = yr[i];

    }
  }

  if (animation_mode == 1) {
       // Initialize triangles
        for (i=0; i<REPEATS; i++)
     {       
       // XY positions based on moving x and potRead y
        sr[i] = s * .75;
        xr[i] = x + ((60-x)*(pow(.75,i)));
        yr[i] = y + ((48-y)*(pow(.75,i)));

        s = sr[i];
        x = xr[i];
        y = yr[i];
   
        // Start moving to the right, and down.
        xm[i] = 1;
        ym[i] = 1;
    }

      TV.delay_frame(1);

      // Draw  repeating triangles
      for (i=0; i<REPEATS; i++) 
      while (animation_mode == 1) {
      // Draw main triangle
        //equi_tri(x,y,s,c,fc);
        equi_tri(xr[i],yr[i],sr[i],c,fc);

      xr[i] = xr[i] + xm[i];
      if (xr[i]<=(sr[i]/2) || xr[i]>TV.hres()-(sr[i]/2)-1) xm[i] = -xm[i];
       
      yr[i] = yr[i] + ym[i];
      if (yr[i]<=(sr[i]/2) || yr[i]>=TV.vres()-(sr[i]/2)-1) ym[i] = -ym[i];
      
      //Erase trianges
      //equi_tri(x,y,s,BLACK, BLACK);
      equi_tri(xr[i],yr[i],sr[i],BLACK,BLACK);
    }
  }



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


/*

long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int shapePin = 2;
int pPin = 3;
int fcPin = 4;
int aniModePin = 5;
int clrScrnPin = 6; 
int repeatsPin = 7;

int shp = 1;
int clear_scn_toggle = 1;
int REPEATS = 10;

void setup()
{
  pinMode(shapePin, INPUT);
  pinMode(pPin, INPUT);
  pinMode(fcPin, INPUT);
  pinMode(aniModePin, INPUT);
  pinMode(clrScrnPin, INPUT);
  pinMode(repeatsPin, INPUT);

}


void loop()
{

int shp_read = digitalRead(shapePin);
int clear_scn_toggle_read = digitalRead(clrScrnPin);
int aniModePin_read = digitalRead(aniModePin);


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
  
}

*/  
