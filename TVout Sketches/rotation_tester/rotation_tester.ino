/*  SKETCH: explores concepts of:
 *   - draw grid, with help of static screen size variables
 *   - draw a background image behind animations
 *   - 
 *  
 *  TODO:
 *  - create concept of a 'backround image' function. After clearning screen for animated objects, call the 'background img' function to redraw static elements
 */

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

static byte left_edge = 0;
static byte right_edge = 119;
static byte top_edge = 0;
static byte bottom_edge = 95;
static byte middle_x = 59;
static byte middle_y = 47;

byte rect_h = 20;
byte rect_w = 40;
byte rect_w_new = 20;
byte rect_h_new = 40;

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  //Serial.begin(9600);
    
  TV.clear_screen(); 
}

void loop() {
  // set some variables
  rect_h = 20;
  rect_w = 40;
  rect_w_new = 20;
  rect_h_new = 40;
  
  // show the key points
  TV.set_pixel(left_edge,top_edge,WHITE);
  TV.set_pixel(middle_x,top_edge,WHITE);
  TV.set_pixel(right_edge,top_edge,WHITE);
  TV.set_pixel(right_edge,middle_y,WHITE);
  TV.set_pixel(right_edge,bottom_edge,WHITE);
  TV.set_pixel(middle_x,bottom_edge,WHITE);
  TV.set_pixel(left_edge,bottom_edge,WHITE);
  TV.set_pixel(left_edge,middle_y,WHITE);
  TV.set_pixel(middle_x,middle_y,WHITE);

  TV.delay(500);
  

  /*
  for(int i = 0; i < bottom_edge; i++){ 
    TV.draw_column(middle_x,top_edge, i,2);  // 2 = invert
    //TV.delay_frame(1);
    } 
  
  TV.delay(500);
  
  //TV.draw_column(middle_x,top_edge, bottom_edge,2);  // 2 = invert
  TV.draw_row(middle_y,left_edge, right_edge,2);  

  TV.delay(500);

  for(int i = 0; i < TV.vres(); i++){ 
    TV.draw_line(left_edge, top_edge,(i*1.25),i,2);  // 2 = invert  1.25 = x to y ratio
    TV.draw_line(right_edge, top_edge,(right_edge - (i*1.25)),i,2);  // 2 = invert
   
    TV.delay_frame(1);
  } 

  TV.delay(2000);
  */
  
  TV.clear_screen();
  bckg_img();

  // Draw rect going right
  for(int i = 0; i < (TV.hres()-rect_w); i++){
    TV.draw_rect(left_edge+i, middle_y - (rect_h/2), rect_w, rect_h, WHITE);
    TV.delay_frame(1);
    TV.clear_screen();
    bckg_img();
  }
  
  // Draw rect going left, back to center
  for(int i = (TV.hres()-rect_w); i > (middle_x - (rect_w/2)); i--){
    TV.draw_rect(left_edge+i, middle_y - (rect_h/2), rect_w, rect_h, WHITE);
    TV.delay_frame(1);
    TV.clear_screen();
    bckg_img();
  }

  // Square up rect; hone in
  for(int i = 0; i < 21; i++){
    rect_w_new = rect_w - i;
    TV.draw_rect(middle_x - (rect_w_new/2), middle_y - (rect_h/2), rect_w_new, rect_h, WHITE);
    TV.delay_frame(1);
    TV.clear_screen();
    bckg_img();
  }

  TV.draw_rect(middle_x - (rect_w_new/2), middle_y - (rect_h/2), rect_w_new, rect_h, WHITE); // where we ended up after the last for loop
  TV.delay(1000);

  // draw 7 expanding rects around center; locked onto target
  for(int i = 0; i < 4; i++){
    rect_w_new = rect_w_new + 4;
    TV.draw_rect(middle_x - (rect_w_new/2), middle_y - (rect_w_new/2), rect_w_new, rect_w_new, WHITE);
    TV.delay_frame(1);
  }

  TV.delay(5000);
  
  TV.clear_screen();
} // END LOOP



// draw a background image that 'doesn't clear' behind foreground animations
void bckg_img() {
  for(int i = 0; i < bottom_edge; i++){ 
    TV.draw_column(middle_x,top_edge, i,2);  // 2 = invert
    //TV.delay_frame(1);
    }
  TV.draw_row(middle_y,left_edge, right_edge,2); 
}
