// Cursor & Local Erasing Test
// - This sketch tests the notion of a cursor and erasing/replacing just what the cursor moves over
// - in order to save a full screen erase and redraw
//
//  TODO:
//  - Figure out what happens if you draw far outside screen boundary: Nothing bad!
//


#include <VGA.h>

int h_res = 800;
int v_res = 600;

// center location of cursor
int cursor_x = 300;
int cursor_y = 200;
int cursor_m = 9; // amount to automatically move the cursor (will replace later with pot values) (note as val can be negative, needs to be an int)

int cursor_size = 9; //should be an odd number
// shape of a 10x10 px cursor
int cursor_1[9][9] = { 
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {1,1,1,1,1,1,1,1,1},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0},
  {0,0,0,0,1,0,0,0,0} };

// store what the cursor covered so that you can redraw it once the cursor moves
int cursor_buffer[9][9];


void draw_cursor (int x, int y) {
  
  // horizontal
  for(int i = 0; i<cursor_size; i++){
    // vertical
    for(int j = 0; j<cursor_size; j++) {
      //draw cursor
      VGA.drawPixel(x-4+i,y-4+j,cursor_1[i][j]);
    }
  }
}

void fill_cursor_buffer (int x, int y) {
  
  for(int i = 0; i<cursor_size; i++){
    for(int j = 0; j<cursor_size; j++) {
      //fill cursor buffer
      cursor_buffer[i][j] = VGA.getPPixelFast(x-4+i,y-4+j);
    }
  }
}

void dump_cursor_buffer (int x, int y) {
 
  for(int i = 0; i<cursor_size; i++){
    for(int j = 0; j<cursor_size; j++) {
      //draw cursor buffer
      VGA.putPPixelFast(x-4+i,y-4+j,cursor_buffer[i][j]);
    }
  }
}

//draw a square at center point x,y with 'radius' (aka 1/2 length)
void draw_square (int x, int y, int r, int col) {
  VGA.drawRect(x-r, y-r, x+r, y+r, col);
}

//draw an equilateral triangle at center point x,y with side length s and direction d
//d: 0 = upright, 1 = upsidedown, 2 = left , 3 = right
//default direction is upright and default color is white
void draw_equi_tri(int x, int y, int s, byte d, int col = 1) {
  //1=top center, 2=bottom left, 3=bottom right
  int x1,y1,x2,y2,x3,y3;

  if(d == 0){
    x1 = x;
    y1 = y-((sqrt(3)/3)*s);   
    x2 = x-(s/2);
    y2 = y+((sqrt(3)/6)*s);
    x3 = x+(s/2);
    y3 = y+((sqrt(3)/6)*s);
  }

  if(d == 1){
    x1 = x; 
    y1 = y+((sqrt(3)/3)*s);
    x2 = x-(s/2);
    y2 = y-((sqrt(3)/6)*s);
    x3 = x+(s/2); 
    y3 = y-((sqrt(3)/6)*s);
  }

  
  if(d == 2){
    x1 = x+((sqrt(3)/3)*s);
    y1 = y;    
    x2 = x-((sqrt(3)/6)*s);
    y2 = y-(s/2);
    x3 = x-((sqrt(3)/6)*s);
    y3 = y+(s/2);
  }

  if(d == 3){
    x1 = x-((sqrt(3)/3)*s);
    y1 = y;    
    x2 = x+((sqrt(3)/6)*s);
    y2 = y-(s/2);
    x3 = x+((sqrt(3)/6)*s);
    y3 = y+(s/2);
  }

  VGA.drawLine(x1,y1,x2,y2,col);
  VGA.drawLine(x2,y2,x3,y3,col);
  VGA.drawLine(x3,y3,x1,y1,col);

  Serial.print("direction d = "); Serial.println(d);

}

//draw a trapazoid point x,y with side length s (due to bytes not being big enough)
void draw_trap(int x, int y, int s, int d = 0,int col = 1) {
  int x1,y1,x2,y2,x3,y3,x4,y4;

  // 'upward' facing
  if(d == 0){
    x1 = x - (s/2);
    y1 = y;
    x2 = x + (s/2);
    y2 = y;
    x3 = x + s;
    y3 = y + (s/2);
    x4 = x - s;
    y4 = y + (s/2);
  }

  // 'downward' facing
  if(d == 1){
    x1 = x - (s/2);
    y1 = y;
    x2 = x + (s/2);
    y2 = y;
    x3 = x + s;
    y3 = y - (s/2);
    x4 = x - s;
    y4 = y - (s/2);
  }

  // 'leftward' facing
  if(d == 2){
    x1 = x;
    y1 = y + (s/2);
    x2 = x ;
    y2 = y - (s/2);
    x3 = x + (s/2);
    y3 = y - s;
    x4 = x + (s/2);
    y4 = y + s;
  }

  // 'rightward' facing
  if(d == 3){
    x1 = x;
    y1 = y + (s/2);
    x2 = x; 
    y2 = y - (s/2);
    x3 = x - (s/2);
    y3 = y - s;
    x4 = x - (s/2);
    y4 = y + s;
  }
 
  VGA.drawLine(x1,y1,x2,y2,col);
  VGA.drawLine(x2,y2,x3,y3,col);
  VGA.drawLine(x3,y3,x4,y4,col);
  VGA.drawLine(x4,y4,x1,y1,col);
  
}

//draw a thing that looks like a startrek badge at center point x,y with side length s (due to bytes not being big enough)
void draw_strange_tri(int x, int y, int s, int col) {
  //1=bottom right
  //2=bottom left
  //3=top center
  
  byte x1 = x+(s/2);
  byte y1 = y+(1.5*(s/(2*sqrt(3))));
  byte x2 = x-(s/2);
  byte y2 = y+(1.5*(s/(2*sqrt(3))));
  byte x3 = x;
  byte y3 = y-(1.5*(s/(2*sqrt(3))));

  VGA.drawLine(x1,y1,x2,y2,col);
  VGA.drawLine(x2,y2,x3,y3,col);
  VGA.drawLine(x3,y3,x1,y1,col);

}


void setup() {
  VGA.begin(800,600,VGA_MONO);
  Serial.begin(9600);
  
  /*
  // draw mega bulls eye 
  // (i<290 is good to fill most of screen / 590 fills more than entire screen and doesn't break anything)
  for(int i = 5; i<290; i = i+5){
    VGA.drawCircle(400,300,i,1);
    delay(10);
  }
  delay(2000);


  // draw mega square eye 
  for(int i = 5; i<200; i = i+10){
    draw_square(400,300,i,-1);
    delay(10);
  }
  delay(2000);
  */

  // draw trap door
  for(int i = 10; i<400; i = i+20){
    draw_trap(400,300,i,0,1);
    delay(10);
    draw_trap(400,300,i,1,1);
    delay(10);
  }
  delay(2000);

  /*
  // draw mega try eye 
  for(int i = 10; i<400; i = i+20){
    //use default color and direaction
    draw_equi_tri(400,300,i,0);
    delay(10);
  }
  delay(2000);

  for(int i = 10; i<400; i = i+20){
    draw_equi_tri(400,300,i,1,1);
    delay(10);
  }
  delay(2000);

 for(int i = 10; i<400; i = i+20){
    draw_equi_tri(400,300,i,2,1);
    delay(10);
  }
  delay(2000);
 for(int i = 10; i<400; i = i+20){
    draw_equi_tri(400,300,i,3,-1);
    delay(10);
  }
  delay(2000);
  */

  
  //fill the cursor buffer before drawing the cursor so that you know what to replace!
  fill_cursor_buffer(cursor_x,cursor_y);

  draw_cursor(cursor_x,cursor_y);
  delay(2000);
 
}


void loop() {
  //before moving the cursor, draw over it! And empty that buffer so that it can be used!
  dump_cursor_buffer(cursor_x,cursor_y);
  
  // logic to move the cursor
  // eventually will do via pots / joystick  
  if (cursor_x <= 4 || cursor_x >= 796) {cursor_m = -cursor_m;}
  cursor_x = cursor_x + cursor_m;

  // before redrawing the cursor, but after knowing where it will be: collect what the cursor is going to be drawn over (so it can be replaced later)
  fill_cursor_buffer(cursor_x,cursor_y);

  // now you can safely draw the cursor
  draw_cursor(cursor_x,cursor_y);

  Serial.print("cursor x = "); Serial.print(cursor_x); Serial.print("  cursor_y = "); Serial.print(cursor_y); Serial.print("  cursor_m = "); Serial.println(cursor_m);


  delay(100);
  
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
