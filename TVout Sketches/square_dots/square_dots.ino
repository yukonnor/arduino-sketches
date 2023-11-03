/*
 * TO DO:
 *   - What does this look like at higher res?
 *   - What would it look like if each point was tracing (sewing) a color?
 */


#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

TVout TV;

int x1=10;
int x2=11;
int x3=12;
int x4=13;
int x5=14; 

int y1=10;
int y2=11;
int y3=12;
int y4=13;
int y5=14;

unsigned long time = 0;


void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  Serial.begin(9600);
  TV.clear_screen(); 
  Serial.println("TEST"); 
    
}



void loop() {

int xm = 1;
int ym = 1;

int i = 0;

      while (i < 20) {
         
         
         x2 = x2+xm;
         //x2 = constrain(x2, 14, 110);
         x3 = x3+(2*xm);
         x3 = constrain(x3, 14, 110);
         x4 = x4+(3*xm);
         //x4 = constrain(x4, 14, 110);
         x5 = x5+(4*xm);
         x5 = constrain(x5, 14, 110);
         if (x5<= 14 || x5>=110) {xm = -xm; ym = -ym;}
             Serial.print("x5= ");Serial.print(x2);Serial.print(" i= ");Serial.println (i);
         
         y2 = y2+ym;
         y3 = y3+(2*ym);
         y4 = y4+(3*ym);
         y5 = y5+(4*ym);   
         
         draw_pixels(x1, x2, x3, x4, x5,y1, y2, y3, y4, y5);
         i++;
      } 
       
         
       while (i < 50) {       
         x1 = x1+(3*xm);
         //x1 = constrain(x1, 14, 110);
         x2 = x2+(4*xm);
         x2 = constrain(x2, 14, 110);
         x3 = x3+(5*xm);
         x3 = constrain(x3, 14, 110);
         x4 = x4+(6*xm);
         //x4 = constrain(x4, 14, 110);
         x5 = x5+(7*xm);
         x5 = constrain(x5, 14, 60);
         if (x5<= 14 || x5>=60) {xm = -xm;} 
             Serial.print("x5= ");Serial.print(x2);Serial.print(" i= ");Serial.println (i);   
         
         y2 = y2+ym;
         y3 = y3+(2*ym);
         y4 = y4+(3*ym);
         y5 = y5+(4*ym);
         y5 = constrain(y5, 14, 80); 
         if (y5<= 14 || y5>=80) {ym = -ym;} //y5=79;}   
         
         draw_pixels(x1, x2, x3, x4, x5, y1, y2, y3, y4, y5);
         i++;
      }

       while (i < 80) {       
         x1 = x1+(3*xm);
         //x1 = constrain(x1, 14, 110);
         x2 = x2+(4*xm);
         //x2 = constrain(x2, 14, 110);
         x3 = x3+(5*xm);
         x3 = constrain(x3, 14, 110);
         x4 = x4+(6*xm);
        // x4 = constrain(x4, 14, 110);
         x5 = x5+(7*xm);
         x5 = constrain(x5, 14, 110);
         if (x5<= 14 || x5>=110) {xm = -xm;} 
             Serial.print("x5= ");Serial.print(x2);Serial.print(" i= ");Serial.println (i);   
         
         y2 = y2+ym;
         y3 = y3+(2*ym);
         y4 = y4+(3*ym);
         y5 = y5+(4*ym);
         y5 = constrain(y5, 14, 80);  
         if (y5<= 14 || y5>=80) {ym = -ym;}   
         
         draw_pixels(x1, x2, x3, x4, x5,y1, y2, y3, y4, y5);
         i++; 
      }
      
//if (y1<= 14 || y1>=80){Serial.print("y1= ");Serial.print(y1);Serial.print(" i= ");Serial.println (i);}


i = 0;

} //END LOOP



void draw_pixels(int x1, int x2, int x3, int x4, int x5, int y1, int y2, int y3, int y4, int y5) {

int i, j = 0;

drawing:

int x_array[5]={x1,x2,x3,x4,x5};
int y_array[5]={y1,y2,y3,y4,y5};

//draw square #1:
for (i = 0; i < 5; i++) {      // I think i can do this with on for statement: 
    for (j = 0; j < 5; j++) {
      TV.set_pixel(x_array[i],y_array[j],WHITE);
    } //close j loop
  } //close i loop
i, j = 0;


//draw square #2:
for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      TV.set_pixel((120-x_array[i]),(90-y_array[j]),WHITE);
    }                  //close j loop
  }                    //close i loop
i, j = 0;              //resets values


//draw square #3:
for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      TV.set_pixel((120-x_array[i]),y_array[j],WHITE);
    } //close j loop
  } //close i loop
i, j = 0;


//draw square #4:
for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      TV.set_pixel(x_array[i],(90-y_array[j]),WHITE);
    } //close j loop
  } //close i loop

  TV.delay_frame(4); 
  TV.clear_screen();  
} // close draw_pixels
