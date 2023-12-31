//#include <GD2.h>

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

//Definie button variables
int shapePin    = 13;
int amntPin     = 12;
int movementPin = 11;

//Define states of Shape, Amount of Dots, Type of Movement
int shp   = 1;
int amnt  = 1;
int mvmt = 1;
int spd   = analogRead(A0); //use 10k pot - Values should be 0 - 1023

//Define the environment we're playing in
int TV_width = 120;
int TV_height = 96;

//Define dot arrays
#define AMOUNT_DOTS 20 //20 dots will be generated

int x_dot_array[AMOUNT_DOTS];
int y_dot_array[AMOUNT_DOTS];




void setup() {
  //Initialize buttons
  pinMode(shapePin, INPUT_PULLUP);  //toggle thru dots, lines, angles, dub-angles
  pinMode(amntPin, INPUT_PULLUP);   //amount of dots
  pinMode(movementPin, INPUT_PULLUP);
  
  
  TV.begin(NTSC,120,96); //Screen resolution
  Serial.begin(9600);
  TV.clear_screen(); 

}

void loop() {
int shape_read = digitalRead(shapePin);
int amnt_read = digitalRead(amntPin);
int movement_read = digitalRead(movementPin);
int spd   = analogRead(A0); 
int t = 0;


//SHAPE BUTTON LOGIC - 4 Different 'shapes'
if (shape_read == LOW &&  millis() - time > debounce) {
    if (shp == 1)
         shp = 2;
    else if (shp == 2)
         shp = 3;
    else if (shp == 3)
         shp = 4;
         else       //if shp = 4
         shp = 1;  
    time = millis(); 
    }

//AMOUNT BUTTON LOGIC - 4 Different 'amounts'
if (amnt_read == LOW &&  millis() - time > debounce) {
    if (amnt == 1)
         amnt = 2;
    else if (amnt == 2)
         amnt = 3;
    else if (amnt == 3)
         amnt = 4;
         else       //if shp = 4
         amnt = 1;  
    time = millis(); 
    }



//MOVEMENT BUTTON LOGIC - 4 Different 'movement types'
if (movement_read == LOW &&  millis() - time > debounce) {
    if  (mvmt == 1)
         mvmt = 2;
    else if (mvmt == 2)
         mvmt = 3;
    else if (mvmt == 3)
         mvmt = 4;
    else       //if mvmt = 4
         mvmt = 1;  
    time = millis(); 
    }   
  

//MAKE DOTS  -- Using the Arrays defined above
for (int i = 0; i< AMOUNT_DOTS; i++)
    {
      int x_rand = random(TV_width);
      x_dot_array[i] = x_rand;
      int y_rand = random(TV_height);
      y_dot_array[i] = y_rand;
      
      TV.set_pixel(x_dot_array[i],y_dot_array[i],WHITE); //DRAW THE INITIAL DOTS
      TV.delay_frame(4); 
      //TV.clear_screen();  
    }


//MAKE DOTS MOVE - MOVEMENT TYPE #1
//ALSO: REGENERATE DOTS THAT HAVE LEFT US
while (t < 2000) 
    {   
      for (int i=0; i< AMOUNT_DOTS; i++) //cycle through array for each 't' step
         {
          // x coordinates!
          if  (x_dot_array[i] < (TV_width/2))        //If x is less that halfway, move left
              {x_dot_array[i] = x_dot_array[i]-1;}
          
          if  (x_dot_array[i] > (TV_width/2))        //If x is more tha halfway, move right
              {x_dot_array[i] = x_dot_array[i]+1;}

          if  (x_dot_array[i] <= (0))                 //If x is <= 0, generate a new dot
              {x_dot_array[i] = random((TV_width/4),(3*TV_width/4));
               y_dot_array[i] = random(TV_height);} 

          if  (x_dot_array[i] >= (TV_width))           //If x is >= TV_width, generate a new dot
              {x_dot_array[i] = random((4*(TV_width/7)),(4*(TV_width/7)));
               y_dot_array[i] = random((TV_height/4),(3*(TV_height/4)));} 


          // y coordinates!
          if (y_dot_array[i] < (TV_height/2))          //If y more less than halfway, move down
             {y_dot_array[i] = y_dot_array[i]-1;}     
          
          if  (y_dot_array[i] > (TV_height/2))         //If y is more that halfway, move up
              {y_dot_array[i] = y_dot_array[i]+1;}      //exponential: (y_dot_array[i]/x_dot_array[i]) instead of "1"

          if  (y_dot_array[i] <= (0))                   //If y is <= 0, generate a new dot
              {x_dot_array[i] = random(TV_width);
               y_dot_array[i] = random(TV_height);}  

          if  (y_dot_array[i] >= (TV_height))           //If y is >= TV_height, generate a new dot
              {x_dot_array[i] = random((4*(TV_width/7)),(4*(TV_width/7)));
               y_dot_array[i] = random((TV_height/4),(3*(TV_height/4)));} 

    
         TV.set_pixel(x_dot_array[i],y_dot_array[i],WHITE);  //Now draw the newly moved or newly generated dots
         }
       
       t++;
       TV.delay_frame(4); 
       TV.clear_screen();    
      }



TV.delay_frame(4); 
TV.clear_screen();  
}

/* SERIAL STUFF
delay(1000);

Serial.print(shp);    
Serial.print("\t");    // prints a tab
Serial.print(amnt);    
Serial.print("\t");       
Serial.print(mvmt);    
Serial.print("\t");
Serial.print(spd);    
Serial.print("\t");     
Serial.println(" ");   // return
*/

/* OLD SQUARE DOTS MOVEMENT CODE
 *    while (i < 440) {       
         x1 = x1+(3*xm);
         x2 = x2+(4*xm);
         x3 = x3+(5*xm);
         x4 = x4+(6*xm);
         x5 = x5+(7*xm);
         if (x5<= 14 || x5>=60) {xm = -xm;} 
            
         
         y2 = y2+ym;
         y3 = y3+(2*ym);
         y4 = y4+(3*ym);
         y5 = y5+(4*ym); 
         if (y5<= 14 || y5>=80) {ym = -ym;}   
         
         draw_pixels(x1, x2, x3, x4, x5,y1, y2, y3, y4, y5);
         i++;
         Serial.println(i); 
      
 */



