#pragma once
#include "Font.h"
#include "TriangleTree.h"

class CompositeGraphics
{ 
  public:
  int xres;
  int yres;
  char **frame;
  char **backbuffer;
  //char **zbuffer;
  int cursorX, cursorY, cursorBaseX;
  int frontColor, backColor;
  Font<CompositeGraphics> *font;
  
  TriangleTree<CompositeGraphics> *triangleBuffer;
  TriangleTree<CompositeGraphics> *triangleRoot;
  int trinagleBufferSize;
  int triangleCount;

  CompositeGraphics(int w, int h, int initialTrinagleBufferSize = 0)
    :xres(w), 
    yres(h)
  {
    font = 0;
    cursorX = cursorY = cursorBaseX = 0;
    trinagleBufferSize = initialTrinagleBufferSize;
    triangleCount = 0;
    frontColor = 50;
    backColor = -1;
  }
  
  void init()
  {
    frame = (char**)malloc(yres * sizeof(int*));
    backbuffer = (char**)malloc(yres * sizeof(int*));
    //not enough memory for z-buffer implementation
    //zbuffer = (char**)malloc(yres * sizeof(char*));
    for(int y = 0; y < yres; y++)
    {
      frame[y] = (char*)malloc(xres);
      backbuffer[y] = (char*)malloc(xres);
      //zbuffer[y] = (char*)malloc(xres);
    }
    triangleBuffer = (TriangleTree<CompositeGraphics>*)malloc(sizeof(TriangleTree<CompositeGraphics>) * trinagleBufferSize);
  }

  //From DueVGA
  template <typename T> int _v_sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }
  #define _V_P(x,y) if(y>=0 && y<yres){if(x<xmin[y])xmin[y]=x;if(x>xmax[y])xmax[y]=x;}

  void setTextColor(int front, int back = -1)
  {
    //-1 = transparent back;
    frontColor = front;
    backColor = back;
  }

  void setFont(Font<CompositeGraphics> &font)
  {
    this->font = &font;
  }
  
  void setCursor(int x, int y)
  {
    cursorX = cursorBaseX = x;  
    cursorY = y;  
  }
  
  void print(char *str)
  {
    if(!font) return;
    while(*str)
    {
      if(*str >= 32 && *str < 128)
        font->drawChar(*this, cursorX, cursorY, *str, frontColor, backColor);
      cursorX += font->xres;
      if(cursorX + font->xres > xres || *str == '\n')
      {
        cursorX = cursorBaseX;
        cursorY += font->yres;        
      }
      str++;
    }
  }
  
  void print(int number, int base = 10, int minCharacters = 1)
  {
    bool sign = number < 0;
    if(sign) number = -number;
    const char baseChars[] = "0123456789ABCDEF";
    char temp[33];
    temp[32] = 0;
    int i = 31;
    do
    {
      temp[i--] = baseChars[number % base];
      number /= base;
    }while(number > 0);
    if(sign)
      temp[i--] = '-';
    for(;i > 31 - minCharacters; i--)
      temp[i] = ' ';
    print(&temp[i + 1]);
  }

  inline void begin(int clear = -1, bool clearZ = true)
  {
    if(clear > -1)
      for(int y = 0; y < yres; y++)
        for(int x = 0; x < xres; x++)
          backbuffer[y][x] = clear;
    triangleCount = 0;
    triangleRoot = 0;
  }

  inline void putPixelFast(int x, int y, char color)
  {
    backbuffer[y][x] = color;
  }
  
  inline void drawPixel(int x, int y, char color)
  {
    if((unsigned int)x < xres && (unsigned int)y < yres)
      backbuffer[y][x] = color;
  }

  inline void pixelAdd(int x, int y, char color)
  {
    if((unsigned int)x < xres && (unsigned int)y < yres)
      backbuffer[y][x] = min(54, color + backbuffer[y][x]);
  }

  inline void pixelSubtract(int x, int y, char color)
  {
    if((unsigned int)x < xres && (unsigned int)y < yres)
      backbuffer[y][x] = max(0,(backbuffer[y][x] - color));
  }

  /*
  inline void pixelAdd(int x, int y, char color)
  {
    if((unsigned int)x < xres && (unsigned int)y < yres)
      backbuffer[y][x] = min(54, color + backbuffer[y][x]);
  }
  */

  // original name: get
  inline int getPixel(int x, int y)
  {
    if((unsigned int)x < xres && (unsigned int)y < yres)
      return backbuffer[y][x];
    return 0;
  }

  void enqueueTriangle(short *v0, short *v1, short *v2, char color)
  {
    if(triangleCount >= trinagleBufferSize) return;
    TriangleTree<CompositeGraphics> &t = triangleBuffer[triangleCount++];
    t.set(v0, v1, v2, color);
    if(triangleRoot)
      triangleRoot->add(&triangleRoot, t);
    else
      triangleRoot = &t;
  }

  void triangle(short *v0, short *v1, short *v2, char color)
  {
    short *v[3] = {v0, v1, v2};
    if(v[1][1] < v[0][1])
    {
      short *vb = v[0]; v[0] = v[1]; v[1] = vb;
    }
    if(v[2][1] < v[1][1])
    {
      short *vb = v[1]; v[1] = v[2]; v[2] = vb;
    }
    if(v[1][1] < v[0][1])
    {
      short *vb = v[0]; v[0] = v[1]; v[1] = vb;
    }
    int y = v[0][1];
    int xac = v[0][0] << 16;
    int xab = v[0][0] << 16;
    int xbc = v[1][0] << 16;
    int xaci = 0;
    int xabi = 0;
    int xbci = 0;
    if(v[1][1] != v[0][1])
      xabi = ((v[1][0] - v[0][0]) << 16) / (v[1][1] - v[0][1]);
    if(v[2][1] != v[0][1])
      xaci = ((v[2][0] - v[0][0]) << 16) / (v[2][1] - v[0][1]);
    if(v[2][1] != v[1][1])
      xbci = ((v[2][0] - v[1][0]) << 16) / (v[2][1] - v[1][1]);

    for(; y < v[1][1] && y < yres; y++)
    {
      if(y >= 0)
        drawHLine(xab >> 16, xac >> 16, y, color);
      xab += xabi;
      xac += xaci;
    }
    for(; y < v[2][1] && y < yres; y++)
    {
      if(y >= 0)
        drawHLine(xbc >> 16, xac >> 16, y, color);
      xbc += xbci;
      xac += xaci;
    }
  }
  
  void line(int x1, int y1, int x2, int y2, char color)
  {
    int x, y, xe, ye;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx1 = labs(dx);
    int dy1 = labs(dy);
    int px = 2 * dy1 - dx1;
    int py = 2 * dx1 - dy1;
    if(dy1 <= dx1)
    {
      if(dx >= 0)
      {
        x = x1;
        y = y1;
        xe = x2;
      }
      else
      {
        x = x2;
        y = y2;
        xe = x1;
      }
      drawPixel(x, y, color);
      for(int i = 0; x < xe; i++)
      {
        x = x + 1;
        if(px < 0)
        {
          px = px + 2 * dy1;
        }
        else
        {
          if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          {
            y = y + 1;
          }
          else
          {
            y = y - 1;
          }
          px = px + 2 *(dy1 - dx1);
        }
        drawPixel(x, y, color);
      }
    }
    else
    {
      if(dy >= 0)
      {
        x = x1;
        y = y1;
        ye = y2;
      }
      else
      {
        x = x2;
        y = y2;
        ye = y1;
      }
      drawPixel(x, y, color);
      for(int i = 0; y < ye; i++)
      {
        y = y + 1;
        if(py <= 0)
        {
          py = py + 2 * dx1;
        }
        else
        {
          if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          {
            x = x + 1;
          }
          else
          {
            x = x - 1;
          }
          py = py + 2 * (dx1 - dy1);
        }
        drawPixel(x, y, color);
      }
    }
  }

  //from DueVGA...see if it works
  void drawLine(int x0, int y0, int x1, int y1,char c)
  {
   int dx=abs(x1-x0), dy=abs(y1-y0),sx=_v_sgn(x1-x0),sy=_v_sgn(y1-y0);
   int err=dx-dy; 
   if((x0!=x1)||(y0!=y1))drawPixel(x1,y1,c);
   do{ drawPixel(x0,y0,c);
       int e2=2*err;
       if (e2 > -dy){err-=dy;x0+=sx;}
       if (e2 <  dx){err+=dx;y0+=sy;}
   }   while ((x0!=x1)||(y0!=y1));
   
  }

  void drawHLine(int x0, int x1, int y, char col)
  {
    for(int i=x0;i<=x1;i++)drawPixel(i,y,col);
  }

  void drawVLine(int x, int y0, int y1, char col)
  {
    for(int i=y0;i<=y1;i++)drawPixel(x,i,col);
  }
  
  inline void flush()
  {
    if(triangleRoot)
      triangleRoot->draw(*this);
  }

  inline void end()
  {
    char **b = backbuffer;
    backbuffer = frame;
    frame = b;    
  }

  /*
  void fillRect(int x, int y, int w, int h, int color)
  {
    if(x < 0){w += x; x = 0;}
    if(y < 0){h += y;y = 0;}
    
    if(x + w > xres)
      w = xres - x;
    if(y + h > yres)
      h = yres - y;
    for(int j = y; j < y + h; j++)
      for(int i = x; i < x + w; i++)
        putPixelFast(i, j, color);
  }*/

  void rect(int x, int y, int w, int h, char color)
  {
    fillRect(x, y, w, 1, color);
    fillRect(x, y, 1, h, color);
    fillRect(x, y + h - 1, w, 1, color);
    fillRect(x + w - 1, y, 1, h, color);
  }


  ////////////////////////////////////////////////
  //From DueVGA//////////////////////////////////
  //////////////////////////////////////////////

  void drawRect(int x0, int y0, int x1, int y1, char col){
    drawLine(x0,y0,x0,y1,col); //stimmer originally used drawLinex() here
    drawLine(x0,y1,x1,y1,col);
    drawLine(x1,y1,x1,y0,col);
    drawLine(x1,y0,x0,y0,col);
  }

  void fillRect(int x0, int y0, int x1, int y1, char col){
    int xa=min(min(xres-1,x0),x1),xb=max(max(0,x0),x1);
    for(int y=min(min(yres-1,y0),y1);y<=max(max(0,y0),y1);y++)
      drawHLine(xa,xb,y,col);
  }

  //d is direction of gradient. 0 up/down, 1 left/right
  //c1 and c2 are the gradient extremes
  void fillRectGrad(int x0, int y0, int x1, int y1, byte d = 0, float c1 = 0.0, float c2 = 54.0){
    int xa=min(min(xres-1,x0),x1),xb=max(max(0,x0),x1);
    int ya=min(min(yres-1,y0),y1),yb=max(max(0,y0),y1);
    int a = abs(x1-x0), b = abs(y1-y0);
    char col = c1;
    int y_orig = y0;
    int x_orig = x0;
    float dgy = (c2-c1)/b; // how much to shift the color by row 
    float dgx = (c2-c1)/a; // how much to shift the color by column

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) { y0 = y1; y1 += b; }                     /* .. exchange them */

    if(d == 0){   
      for(int y=ya;y<=yb;y++){
        col = c1 + (dgy*(y-ya)); 
        drawHLine(xa,xb,y,col);
      }
    } 
    else if (d == 1){
      for(int x=xa;x<=xb;x++){
        col = c1 + (dgx*(x-xa)); 
        drawVLine(x,ya,yb,col);
      }
    }
  }

  void drawCircle(int xm, int ym, int r, char col){
    int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
    do {                                          
       drawPixel(xm-x, ym+y,col);                        /*   I. Quadrant +x +y */
       drawPixel(xm-y, ym-x,col);                        /*  II. Quadrant -x +y */
       drawPixel(xm+x, ym-y,col);                        /* III. Quadrant -x -y */
       drawPixel(xm+y, ym+x,col);                        /*  IV. Quadrant +x -y */
       r = err;                                   
       if (r <= y) err += ++y*2+1;                             /* e_xy+e_y < 0 */
       if (r > x || err > y)                  /* e_xy+e_x > 0 or no 2nd y-step */
          err += ++x*2+1;                                     /* -> x-step now */
    } while (x < 0); 
  }

  void fillCircle(int xm, int ym, int r, char col){
    short xmin[yres],xmax[yres];
    for(int i=0;i<yres;i++){xmin[i]=xres;xmax[i]= -1;}
    int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
    do {                                          
       _V_P(xm-x, ym+y);                        /*   I. Quadrant +x +y */
       _V_P(xm-y, ym-x);                        /*  II. Quadrant -x +y */
       _V_P(xm+x, ym-y);                        /* III. Quadrant -x -y */
       _V_P(xm+y, ym+x);                        /*  IV. Quadrant +x -y */
       r = err;                                   
       if (r <= y) err += ++y*2+1;                             /* e_xy+e_y < 0 */
       if (r > x || err > y)                  /* e_xy+e_x > 0 or no 2nd y-step */
          err += ++x*2+1;                                     /* -> x-step now */
    } while (x < 0); 
    
    for(int i=0;i<yres;i++)
      if(xmin[i]<=xmax[i])
        drawHLine(xmin[i],xmax[i],i,col);
  }

  //d is direction of gradient. 0 up/down, 1 left/right
  //c1 and c2 are the gradient extremes
  void fillCircleGrad(int xm, int ym, int r, byte d = 0, float c1 = 0.0, float c2 = 54.0){ 
    //find difference of c2 - c1 / gradient distance (either x1-x0 or y1-y0) and that's how much the color needs to change by per 'row'
    int r_orig = r;
    int c = c1;
    float dg = (c2-c1)/r_orig; // how much to shift the color by row or column
    short xmin[yres],xmax[yres];
    
    for(int i=0;i<yres;i++){xmin[i]=xres;xmax[i]= -1;}
    int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
    do {                                          
       _V_P(xm-x, ym+y);                        /*   I. Quadrant +x +y */
       _V_P(xm-y, ym-x);                        /*  II. Quadrant -x +y */
       _V_P(xm+x, ym-y);                        /* III. Quadrant -x -y */
       _V_P(xm+y, ym+x);                        /*  IV. Quadrant +x -y */
       r = err;                                   
       if (r <= y) err += ++y*2+1;                             /* e_xy+e_y < 0 */
       if (r > x || err > y)                  /* e_xy+e_x > 0 or no 2nd y-step */
          err += ++x*2+1;                                     /* -> x-step now */
    } while (x < 0); 

    //horizontal gradient
    for(int i=0;i<yres;i++)
      if(xmin[i]<=xmax[i]){
        c = c1 + (dg*(i-(ym-r_orig)));     // i is y, c is first hit at top edge of circle i = (ym-r_orig)
        drawHLine(xmin[i],xmax[i],i,c);
      }
  }


  //Draws an ellipse bounded by rectangle (x0,y0),(x1,y1) in colour col
  void drawEllipse(int x0, int y0, int x1, int y1, int col){
                               /* rectangular parameter enclosing the ellipse */
    long a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;                 /* diameter */
    double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
    double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) y0 = y1;                                  /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
    a = 8*a*a; b1 = 8*b*b;                   
                                            
    do {                                     
       drawPixel(x1, y0,col);                                  /*   I. Quadrant */
       drawPixel(x0, y0,col);                                  /*  II. Quadrant */
       drawPixel(x0, y1,col);                                  /* III. Quadrant */
       drawPixel(x1, y1,col);                                  /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    } while (x0 <= x1);

    while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
       drawPixel(x0-1, y0,col);                    /* -> finish tip of ellipse */
       drawPixel(x1+1, y0++,col);
       drawPixel(x0-1, y1,col);
       drawPixel(x1+1, y1--,col);
    }
  }

  void fillEllipse(int x0, int y0, int x1, int y1, int col){                              
                               /* rectangular parameter enclosing the ellipse */
    long a = abs(x1-x0), b = abs(y1-y0), b1 = b&1;                 /* diameter */
    double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
    double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */
    short xmin[yres],xmax[yres];
    for(int i=0;i<yres;i++){xmin[i]=xres;xmax[i]= -1;}

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) y0 = y1;                                  /* .. exchange them */
    y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
    a = 8*a*a; b1 = 8*b*b;                   
                                            
    do {                                     
       _V_P(x1, y0);                                  /*   I. Quadrant */
       _V_P(x0, y0);                                  /*  II. Quadrant */
       _V_P(x0, y1);                                  /* III. Quadrant */
       _V_P(x1, y1);                                  /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    } while (x0 <= x1);

    while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
       _V_P(x0-1, y0);                    /* -> finish tip of ellipse */
       _V_P(x1+1, y0++);
       _V_P(x0-1, y1);
       _V_P(x1+1, y1--);
    }
   
    for(int i=0;i<yres;i++)
      if(xmin[i]<=xmax[i])
        drawHLine(xmin[i],xmax[i],i,col);
  }

  //d is direction of gradient. 0 up/down, 1 left/right
  void fillEllipseGrad(int x0, int y0, int x1, int y1, byte d = 0, float c1 = 0.0, float c2 = 54.0){ 
    //find difference of c2 - c1 / gradient distance (either x1-x0 or y1-y0) and that's how much the color needs to change by per 'row'
    int c = c1;
    long  a= abs(x1-x0), b = abs(y1-y0), b1 = b&1;                 /* diameter */
    double dx = 4*(1.0-a)*b*b, dy = 4*(b1+1)*a*a;           /* error increment */
    double err = dx+dy+b1*a*a, e2;                          /* error of 1.step */
    short xmin[yres],xmax[yres];
    
    for(int i=0;i<yres;i++){xmin[i]=xres;xmax[i]= -1;}

    if (x0 > x1) { x0 = x1; x1 += a; }        /* if called with swapped points */
    if (y0 > y1) y0 = y1;                                  /* .. exchange them */
    int y_orig = y0;
    int x_orig = x0;
    float dgy = (c2-c1)/b; // how much to shift the color by row 
    float dgx = (c2-c1)/a; // how much to shift the color by column
    
    y0 += (b+1)/2; y1 = y0-b1;                               /* starting pixel */
    a = 8*a*a; b1 = 8*b*b;                   
                                            
    do {                                     
       _V_P(x1, y0);                                  /*   I. Quadrant */
       _V_P(x0, y0);                                  /*  II. Quadrant */
       _V_P(x0, y1);                                  /* III. Quadrant */
       _V_P(x1, y1);                                  /*  IV. Quadrant */
       e2 = 2*err;
       if (e2 <= dy) { y0++; y1--; err += dy += a; }                 /* y step */
       if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; }  /* x step */
    } while (x0 <= x1);

    while (y0-y1 <= b) {                /* too early stop of flat ellipses a=1 */
       _V_P(x0-1, y0);                    /* -> finish tip of ellipse */
       _V_P(x1+1, y0++);
       _V_P(x0-1, y1);
       _V_P(x1+1, y1--);
    }

    ////horizontal gradient (d == 0)
    for(int i=0;i<yres;i++)
      if(xmin[i]<=xmax[i]){
        c = c1 + (dgy*(i-y0));     // i is y, c is first hit at top edge of ellipse i = yo
        drawHLine(xmin[i],xmax[i],i,c);
      }
  }

  // d is direction of circular arc
  void drawArc(int xm, int ym, int r, byte d, int col = 54){
    int x = -r, y = 0, err = 2-2*r;                /* bottom left to top right */
    do {
      if(d==0)drawPixel(xm-x, ym+y,col);                 /*   I. Quadrant +x +y */
      else if(d==1)drawPixel(xm-y, ym-x,col);            /*  II. Quadrant -x +y */
      else if(d==2)drawPixel(xm+x, ym-y,col);            /* III. Quadrant -x -y */
      else if(d==3)drawPixel(xm+y, ym+x,col);            /*  IV. Quadrant +x -y */
       r = err;                                   
       if (r <= y) err += ++y*2+1;                             /* e_xy+e_y < 0 */
       if (r > x || err > y)                  /* e_xy+e_x > 0 or no 2nd y-step */
          err += ++x*2+1;                                     /* -> x-step now */
    } while (x < 0); 
  }

  // d is direction of circular arc
  // t is how thic the arc is, r measures to the outer edge of arc
  void drawThicArc(int xm, int ym, int r, byte d, byte t, int col = 54){
    for(int i = 0; i<=t; i++)drawArc(xm,ym,r-i,d,col);
  }

  
  //Scrolls an area of the screen, top left corner (x,y), width w, height h
  //by (dx,dy) pixels. If dx>0 scrolling is right, dx<0 is left. dy>0 is down,
  //dy<0 is up.
  void scroll(int x, int y, int w, int h, int dx, int dy,int col){
    if(dy<=0){
      if(dx<=0){      
        for(int i=x;i<x+w+dx;i++)for(int j=y;j<y+h+dy;j++)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      } 
      else{
        for(int i=x+w-1;i>=x+dx;i--)for(int j=y;j<y+h+dy;j++)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
    }
    else{
      if(dx<=0){      
        for(int i=x;i<x+w+dx;i++)for(int j=y+h-1;j>=y+dy;j--)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
      else{
        for(int i=x+w-1;i>=x+dx;i--)for(int j=y+h-1;j>=y+dy;j--)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
    }
    
    if(dy>0)fillRect(x,y,x+w-1,y+dy-1,col); 
    else if(dy<0)fillRect(x,y+h+dy,x+w-1,y+h-1,col); 
    if(dx>0)fillRect(x,y,x+dx-1,y+h-1,col); 
    else if(dx<0)fillRect(x+w+dx,y,x+w-1,y+h-1,col);
  }

  // Scrolls entire screen by (dx,dy) pixels. 
  // dx>0 scrolling is right, dx<0 is left. dy>0 is down, dy<0 is up.
  void scrollSreen(int dx, int dy,int col = 0){
    if(dy<=0){
      if(dx<=0){      
        for(int i=0;i<xres+dx;i++)for(int j=0;j<yres+dy;j++)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      } 
      else{
        for(int i=xres-1;i>=dx;i--)for(int j=0;j<yres+dy;j++)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
    }
    else{
      if(dx<=0){      
        for(int i=0;i<xres+dx;i++)for(int j=yres-1;j>=dy;j--)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
      else{
        for(int i=xres-1;i>=dx;i--)for(int j=yres-1;j>=dy;j--)
          putPixelFast(i,j,getPixel(i-dx,j-dy));
      }
    }
    
    if(dy>0)fillRect(0,0,xres-1,dy-1,col); 
    else if(dy<0)fillRect(0,yres+dy,xres-1,yres-1,col); 
    if(dx>0)fillRect(0,0,dx-1,yres-1,col); 
    else if(dx<0)fillRect(xres+dx,0,xres-1,yres-1,col);
    
  }

  
};
