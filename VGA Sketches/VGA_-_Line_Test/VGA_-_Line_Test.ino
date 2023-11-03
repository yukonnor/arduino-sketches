/*  Animated Lines & Circles
 *  - Animates lines passing through the center points that bounce left and right
 *  - Animates a circle bouncing left and right, playing with speed (slowing) as it gets further from center
 *   
 *  TODO:
 *  - Currently flickers
 */


#include <VGA.h>

// Set resolution here. Max supported for mono is 800x600
// Other options include: (640x480),(400,300),(320,240) 
int h_res = 640;
int v_res = 480;

int h_mid = h_res/2;
int v_mid = v_res/2;

byte num_lines = 6;
byte num_h_lines = 6;
byte num_circs = 1;

// X,Y Coordinate Arrays of different shapes
int x[] = {h_mid-60,h_mid-40,h_mid-20,h_mid,h_mid+20,h_mid+40}; 
int y[] = {1,1,1,1,1,1}; 

int x_h[] = {1,1,1,1,1,1}; 
int y_h[] = {v_mid-30, v_mid-20, v_mid-10, v_mid, v_mid+10, v_mid+20}; 

int x_c = h_mid;
int y_c = v_mid;

int x_mirror = h_mid;
int y_mirror = v_mid;

// Array of 'velocities' for the lines and circle
int xm[]= {4,2,4,2,4,2};
int y_hm[]= {4,2,4,2,4,2};

int xm_c = 1; // circle's default velocity
int xm_c_new = xm_c; //adjusted velocity


int f = 1;


void drawLine3 (int x0, int y0, int x1, int y1, int f) {
  // needs to logic here to determine where points 0,1 are relative to eachother
  // if positive slope: x' = x - 1
  // if negative slope: x' = x + 1
  // if vertical (x0 = x1): x' = x  -- check first
  // if horizontal: draw line with same x values, but y values adjusted up and down

  // draw original line
  VGA.drawLine(x0,y0,x1,y1,f); 

  // draw thickening lines
  // if horizontal
  if(y0 == y1){ 
    VGA.drawLine(x0, y0 - 1, x1, y1 - 1 ,f); // below
    VGA.drawLine(x0, y0 + 1, x1, y1 + 1, f); // above
  }
  // if has a slope or is vertical
  else {
    VGA.drawLine(x0 - 1,y0,x1 - 1,y1,f); // preceding
    VGA.drawLine(x0 + 1,y0,x1 + 1,y1,f); // following
  }
}

void drawLine5 (int x0, int y0, int x1, int y1, int f) {
  // draw original line
  VGA.drawLine(x0,y0,x1,y1,f); 

  // draw thickening lines
  if(y0 == y1){ 
    VGA.drawLine(x0, y0 - 2, x1, y1 - 2 ,f); 
    VGA.drawLine(x0, y0 - 1, x1, y1 - 1 ,f); 
    VGA.drawLine(x0, y0 + 1, x1, y1 + 1, f); 
    VGA.drawLine(x0, y0 + 2, x1, y1 + 2, f); 
  }
  // if has a slope or is vertical
  else {
    VGA.drawLine(x0 - 2,y0,x1 - 2,y1,f); 
    VGA.drawLine(x0 - 1,y0,x1 - 1,y1,f); 
    VGA.drawLine(x0 + 1,y0,x1 + 1,y1,f); 
    VGA.drawLine(x0 + 2,y0,x1 + 2,y1,f);
  }
}

void setup() {
  VGA.setMonitorFreqRange(27000, 82000, 60, 65);
  VGA.begin(h_res,v_res,VGA_MONO);
  //Serial.begin(9600);

}


void loop() {
  VGA.clear();
  VGA.waitSync();
  
  VGA.fillCircle(x_mirror,y_mirror,8,1); // draw center circle

  // move lines forward or backward
  for(int i = 0; i < num_lines; i++){
    x[i] = x[i] + xm[i]; 
  }

  // move horizontal lines up or down
  for(int i = 0; i < num_lines; i++){
    y_h[i] = y_h[i] + y_hm[i]; 
  }

  // move circle forward or backward
  x_c = x_c + xm_c_new;

  // Draw vertical lines, mirrored around center
  for(int i = 0; i < num_lines; i++){
    drawLine3(x[i],y[i],x_mirror + (x_mirror - x[i]),v_res,f);
  }

  // vertical line switch
  for(int i = 0; i < num_lines; i++){
    if (x[i] <= 100 || x[i] >= (h_res-100)) {xm[i]  = -xm[i];} // switch xm movement direction
  }

  /*
  // Draw horizontal lines, mirrored around center
  for(int i = 0; i < num_h_lines; i++){
    VGA.drawLine(x_h[i],y_h[i],h_res,y_mirror + (y_mirror - y_h[i]),f);
  }

  // horizontal line switch
  for(int i = 0; i < num_h_lines; i++){
    if (y_h[i] <= 100 || y_h[i] >= (v_res-100)) {y_hm[i]  = -y_hm[i];} // switch xm movement direction
  }
  */
  
  VGA.fillCircle(x_c,y_c,30,-1);
  
  // circle switch
  if (x_c <= 200 || x_c >= (h_res-200)) {xm_c  = -xm_c;} // switch xm movement direction

  // circle - adjust speed based on distance from center (further = slower)
  if(abs(x_c - h_mid) <= 60) {
    xm_c_new = 4 * xm_c;
  }
  else if(abs(x_c - h_mid) <= 90) {
    xm_c_new = 3 * xm_c;
  }
  else if(abs(x_c - h_mid) <= 105) {
    xm_c_new = 2 * xm_c;
  }
  else {
    xm_c_new = xm_c;
  } 

  //Serial.print("abs = "); Serial.print(abs(x_c - h_mid)); Serial.print("  xm_c_new = "); Serial.println(xm_c_new);
  VGA.waitSync();
  delay(500); //33 flickers
  
}
