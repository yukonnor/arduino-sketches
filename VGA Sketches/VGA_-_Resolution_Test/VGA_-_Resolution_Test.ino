// Resolution Test
// - This sketch tests to see if we get that full on 800x600 resolution


#include <VGA.h>



byte h_res = 800;
byte v_res = 600;

void setup() {
  VGA.begin(800,600,VGA_MONO);
}


void loop() {
  VGA.clear();
  //VGA.fillCircle(400,300,100,1);
  for(int i = 5; i<290; i = i+5){
    VGA.drawCircle(400,300,i,1);
    delay(10);
  }
  for(int i = 5; i<290; i = i+5){
    VGA.drawCircle(500,300,i,1);
    delay(10);
  }
  for(int i = 5; i<290; i = i+5){
    VGA.drawCircle(300,300,i,1);
    delay(10);
  }
  
  delay(2000);
}

/*
 *VGA.drawLine(2,2,h_res,v_res,1); //diagonal
 *VGA.drawLine(2,2,2,v_res,1); //vertical
 *VGA.drawLine(2,2,h_res,2,1); //horizontal
 */

/*
 *for(int j = 0; j < v_res; j+2){
    for(int i = 0; i < h_res; i+2){
      VGA.drawPixel(i,j,1); //diagonal
    }
  }
 * 
 */
