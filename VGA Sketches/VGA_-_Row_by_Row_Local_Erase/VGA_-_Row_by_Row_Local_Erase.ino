// Row by Row Local Erasing Test
// - This sketch \
//
//  TODO:
//  - Figure out what happens if you draw far outside screen boundary
//

#include <VGA.h>

int h_res = 800;
int v_res = 600;

int h_mid = h_res / 2;

// each 'row' is 20px tall
int row_height = 20;
int num_rows = v_res / row_height;

// draw a simple dippp (centered)
void draw_dip(int x, int y, int dl, int dh, int col) {
  int x1 = h_mid-(dl/2)-10;
  int x2 = h_mid-(dl/2);
  int x3 = h_mid+(dl/2);
  int x4 = h_mid+(dl/2)+10;
  int x5 = h_res - x; //center that
 
  //x,y is the starting point
  VGA.drawLine(x,y,x1,y,col);
  VGA.drawLine(x1,y,x2,y+dh,col);
  VGA.drawLine(x2,y+dh,x3,y+dh,1);
  VGA.drawLine(x3,y+dh,x4,y,1);
  VGA.drawLine(x4,y,x5,y,1);

}


void setup() {
  VGA.begin(800,600,VGA_MONO);
  Serial.begin(9600);

  VGA.drawLine(5,5,300,5,1);
  
  delay(2000);

  
 
}


void loop() {
  // to locally erase a row, find the row's starting point: row 3 = row_height * 3
  // then draw horizontal black lines
  
}
