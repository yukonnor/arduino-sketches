/*  SKETCH: explores concepts of:
 *  
 *  TODO:
 *   - Some square's transparent, other's not (black)
 *   - rects movement contained to the sides of the inner x
 */

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

// properties of screen
static byte left_edge = 0;
static byte right_edge = 119;
static byte top_edge = 0;
static byte bottom_edge = 95;
static byte middle_x = 59;
static byte middle_y = 47;

// properties of rects
int rect_1_x = 24;  //starting position of rect 1
byte rect_1_y = 40;
int rect_2_x = 16;
byte rect_2_y = 32;
int rect_3_x = 8;
byte rect_3_y = 24;

byte rect_1_w = 24;
byte rect_1_h = 20;
byte rect_2_w = 38;
byte rect_2_h = 30;
byte rect_3_w = 60;
byte rect_3_h = 50;

short x1m = 2; //x direction movement
short x2m = 2; //x direction movement
short x3m = 2; //x direction movement

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  Serial.begin(9600);

    
  TV.clear_screen(); 
  bckg_img(); //draw background image
}

void loop() {
  byte i = 0;
  

  

  // Draw rect going right for `i` loops
  /*
  while (i < 200) {       

         // rect #1 drawing )
         rect_1_x = rect_1_x + x1m;
         rect_1_x = constrain(rect_1_x, left_edge, (right_edge - rect_1_w));
         if (rect_1_x <= left_edge || rect_1_x >= (right_edge - rect_1_w)) {x1m = -x1m;}   
         TV.draw_rect(rect_1_x, rect_1_y, (2*rect_1_w), (2*rect_1_h), WHITE);
         //Serial.print("rect_1_x= ");Serial.print(rect_1_x);Serial.print(" i= ");Serial.println (i);
 
         
         rect_2_x = rect_2_x + x2m;
         rect_2_x = constrain(rect_2_x, left_edge, (right_edge - rect_1_w));
         if (rect_2_x <= left_edge || rect_2_x >= (right_edge - rect_1_w)) {x2m = -x2m;}   
         TV.draw_rect(rect_2_x, rect_2_y, rect_1_w, rect_1_h, WHITE);
         
         TV.delay_frame(2);
         TV.clear_screen();
         bckg_img();
         
         i++;
         }
     */
   // inner rect movement constrained by outer rect
   while (i < 400) {       

         // rect #1 drawing (small, inner)
         rect_1_x = rect_1_x + x1m;
         rect_1_x = constrain(rect_1_x, rect_2_x+2, ((rect_2_x+rect_2_w-2) - rect_1_w));
         if (rect_1_x <= rect_2_x+2 || rect_1_x >= ((rect_2_x+rect_2_w-2) - rect_1_w)) {x1m = -x1m;}   
         TV.draw_rect(rect_1_x, rect_1_y, rect_1_w, rect_1_h, WHITE);
         //Serial.print("rect_1_x= ");Serial.print(rect_1_x);Serial.print(" i= ");Serial.println (i);
 
         // rect #2 drawing (big)
         rect_2_x = rect_2_x + x2m;
         rect_2_x = constrain(rect_2_x, rect_3_x+2, ((rect_3_x+rect_3_w-2) - rect_2_w));
         if (rect_2_x <= rect_3_x+2 || rect_2_x >= ((rect_3_x+rect_3_w-2) - rect_2_w)) {x2m = -x2m;}   
         TV.draw_rect(rect_2_x, rect_2_y, rect_2_w, rect_2_h, WHITE);
         //Serial.print("rect_2_x= ");Serial.print(rect_2_x);Serial.print(" x2m= ");Serial.println (x2m);

         // rect #3 drawing (bigger)
         rect_3_x = rect_3_x + x3m;
         rect_3_x = constrain(rect_3_x, left_edge, (right_edge - rect_3_w));
         if (rect_3_x <= left_edge || rect_3_x >= (right_edge - rect_3_w)) {x3m = -x3m;}   
         TV.draw_rect(rect_3_x, rect_3_y, rect_3_w, rect_3_h, WHITE);
         
         TV.delay_frame(2);
         TV.clear_screen();
         //bckg_img();
         
         i++;
         }

  
  //TV.clear_screen();
} // END LOOP



// draw a background image that 'doesn't clear' behind foreground animations
void bckg_img() {
  for(int i = 0; i < bottom_edge; i++){ 
    TV.draw_column(middle_x,top_edge, i,2);  // 2 = invert
    }
  TV.draw_row(middle_y,left_edge, right_edge,2); 
  //TV.delay_frame(1);
}
