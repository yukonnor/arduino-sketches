/*
 * TO DO:
 *   - Fix video 'flashes' (just shoddy components?)
 */

#include <TVout.h>
#include <fontALL.h>

// 128,56
// SCREEN CAN HOLD 15 8x8font charaters

TVout TV;
unsigned char x,y;

char message[] = "1234567890abcde";
int  array_size = 15;
char message2[] = "234567890abcde1";
int  array_size2 = 15;


void setup()  {
  x=0;
  y=0;
  TV.begin(NTSC);  //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  TV.select_font(font8x8);
}


void loop() {
  TV.clear_screen();
  //delay(200);

  for (int j=0; j<array_size; j++) {      // controlls the greater cycle

    // Print the string fron left side of screen until last character reached
    for(int i=j; i<array_size; i++) {
      TV.print(message[i]); //with j==0, string starts at beging, 1, string starts at 2nd character, etc.
    }
    //same thing for second string (place it one line down)
    TV.set_cursor(0,8); // x,y
    for(int i=j; i<array_size2; i++) {
      TV.print(message2[i]); 
    }

    // Print the first character of string (and so on) after the last character
    if(j>0) {
      TV.set_cursor((array_size*8)-(j*8), 0);  // sets where first character should start based on string length and current cycle (j)
      for(int i=0; i<j; i++){
        TV.print(message[i]);
      }
      //same thing for second string
      TV.set_cursor((array_size2*8)-(j*8), 8);  
      for(int i=0; i<j; i++){
        TV.print(message2[i]);
      }
    }
    
  
    delay(200);             // Show current string for 200 ms
    TV.clear_screen();
  }




}


// NOTES:

/*
 * for (int i = 15, j = 39; (i > 0) && (j > 22); i++, j--)
 {
            ....
  }
 */
