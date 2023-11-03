#include <VGA.h>

// DrawingTest
// test of all drawing functions

int h_res = 800;
int v_res = 600;

int h_mid = h_res / 2;
int v_mid = v_res / 2;

int y = 3;
int ym = 5;

int r_1 = v_res / 10;
int r_2 = v_res / 6;
int r_3 = v_res / 4;
int r_4 = v_res / 3;
int r_5 = v_res / 2;

// Draw a 3px wide line
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

// Draw a 5px wide line
void drawLine5 (int x0, int y0, int x1, int y1, int f) {
  // draw original line
  VGA.drawLine(x0,y0,x1,y1,f); 

  // draw thickening lines on either side of original line
  // if horizonatl
  if(y0 == y1){ 
    VGA.drawLine(x0, y0 - 2, x1, y1 - 2 ,f); 
    VGA.drawLine(x0, y0 - 1, x1, y1 - 1 ,f); 
    VGA.drawLine(x0, y0 + 1, x1, y1 + 1, f); 
    VGA.drawLine(x0, y0 + 2, x1, y1 + 2, f); 
  }
  // if vertical or has slope
  else {
    VGA.drawLine(x0 - 2,y0,x1 - 2,y1,f); 
    VGA.drawLine(x0 - 1,y0,x1 - 1,y1,f); 
    VGA.drawLine(x0 + 1,y0,x1 + 1,y1,f); 
    VGA.drawLine(x0 + 2,y0,x1 + 2,y1,f); 
  }
}

// Draw a circle with a 2px wide border
void drawCircle2 (int x, int y, int r, int f) {
  VGA.drawCircle(x,y,r,f);
  VGA.drawCircle(x,y,r-1,f);
}

// Draw a circle with a 4px wide border
void drawCircle4 (int x, int y, int r, int f) {
  VGA.drawCircle(x,y,r,f);
  VGA.drawCircle(x,y,r-1,f);
  VGA.drawCircle(x,y,r-2,f);
  VGA.drawCircle(x,y,r-3,f);
}

// Draw a circle with a 8px wide border
void drawCircle8 (int x, int y, int r, int f) {
  VGA.drawCircle(x,y,r,f);
  VGA.drawCircle(x,y,r-1,f);
  VGA.drawCircle(x,y,r-2,f);
  VGA.drawCircle(x,y,r-3,f);
  VGA.drawCircle(x,y,r-4,f);
  VGA.drawCircle(x,y,r-5,f);
  VGA.drawCircle(x,y,r-6,f);
  VGA.drawCircle(x,y,r-7,f);
}

// Draw a triange with a 4px wide border
void drawTri4 (int x0, int y0, int x1, int y1, int x2, int y2, int f) {
  // needs to logic here to determine where points 0,1,2 are relative to eachother
  // for now, will just assume that 0 = bottom left, 1 top middle, 2 bottom right

  // original triangle (outer)
  VGA.drawTri(x0,y0,x1,y1,x2,y2,f);
  
  VGA.drawTri(x0+1,y0+1,x1,y1-1,x2-1,y2+1,f);
  VGA.drawTri(x0+2,y0+2,x1,y1-2,x2-2,y2+2,f);
  VGA.drawTri(x0+3,y0+3,x1,y1-3,x2-3,y2+3,f);
}

// Draw a triange with a 8px wide border
void drawTri8 (int x0, int y0, int x1, int y1, int x2, int y2, int f) {
  // needs to logic here to determine where points 0,1,2 are relative to eachother
  // for now, will just assume that 0 = bottom left, 1 top middle, 2 bottom right
  // also needs to adjust inner triangle's coords based on slope of line
  VGA.drawTri(x0,y0,x1,y1,x2,y2,f);
  VGA.drawTri(x0+1,y0+1,x1,y1-1,x2-1,y2+1,f);
  VGA.drawTri(x0+2,y0+2,x1,y1-2,x2-2,y2+2,f);
  VGA.drawTri(x0+3,y0+3,x1,y1-3,x2-3,y2+3,f);
  VGA.drawTri(x0+3,y0+4,x1,y1-4,x2-4,y2+4,f);
  VGA.drawTri(x0+3,y0+5,x1,y1-5,x2-5,y2+5,f);
  VGA.drawTri(x0+3,y0+6,x1,y1-6,x2-6,y2+6,f);
  VGA.drawTri(x0+3,y0+7,x1,y1-7,x2-7,y2+7,f);
}


void setup() {
  VGA.begin(h_res,v_res,VGA_MONO);
  VGA.clear();
  drawCircle8(h_mid,v_mid,r_1,1);  
  drawCircle2(h_mid,v_mid,r_2,1); 
  drawCircle4(h_mid,v_mid,r_3,1); 
  drawCircle4(h_mid,v_mid,r_4,1); 
  drawCircle8(h_mid,v_mid,r_5,1); 

  drawLine3((h_mid-100),0,(h_mid-100),v_res,-1);
  drawLine5(h_mid,0,h_mid,v_res,-1);
  drawLine3((h_mid+100),0,(h_mid+100),v_res,-1);

  //drawTri4(100,200,400,500,700,200,-1);
  //drawTri8(200,300,400,400,600,300,-1);
}


void loop() {
  
  y = y + ym;
  drawLine5(0, y, h_res, y, -1); // draw a moving horizontal line
  if (y <= 3 || y >= (v_res - 3)) {ym  = -ym;} // switch ym movement direction at top and bottom of screen
  
  VGA.waitSync();
}
