#include <TVout.h>
#include <fontALL.h>

/* TODO:
//  - define functions that make it easier to understand keeping strings: "Keep Up For 1000ms"  "GO DARK FOR 500ms"
//  - design a 'texture font' that has three shades (all symetric and tileable): Light, Medium, Dark
//  --build with more femine parts. what does building with electronics teach us about gender dynamics?
*/

// 128,56
// SCREEN CAN HOLD 15 8x8font charaters

TVout TV;
unsigned char x,y;
int length_1;
char* myStrings[] = {"15character max", "This is string 2", "This is string 3",
                     "This is string 4", "This is string 5", "This is string 6"
                    };  // using char* pointer to define array of strings
char message1[] = "Message 1";
char message2[] = "Message 2";
int size1 = 0;
int x_start = 0;
int y_start = 0;


void setup()  {
  x=0;
  y=0;
  TV.begin(NTSC);  //for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  TV.select_font(font8x8);

  size1 = sizeof(message1) - 1;  // Find the length of a string (-1 to remove the trailing null character)
}

void loop() {
  TV.clear_screen();
  x=0;
  y=0;
  for (char i = 32; i < 127; i++) {
    TV.print_char(x*6,y*8,i);
    x++;
    if (x >= TV.char_line()) {
      y++;
      x=0;
    }
  }                            // YOU can break to next line with hidden character '\n' in string
  //length_1 = message.length();                   // GETS the lenght of a string (function of string variable)

  x_start = ((16-size1)/2)*8;  // TRIES TO FIND THE STARTING POINT FOR THE STRING IN ORDER TO BE CENTERED
  
  TV.delay(1000);
  TV.clear_screen();
  TV.println("Fill the Screen\nPixel by Pixel");
  TV.println(length_1);
  TV.delay(1000);
  TV.clear_screen();
  TV.print("Draw some lines");
  TV.delay(1000);
  TV.clear_screen();
  TV.delay(1000);
  TV.print_char(x_start,y_start, 65); // YOU can place specific (ASCII) characters as specific positions (x,y,character).
  TV.delay(1000);
  TV.clear_screen();
  TV.delay(500);
  TV.set_cursor(80,32);                  // YOU can set where the cursor should start typing (for the next print call)
  TV.print("YOU'VE BEEN MOVED,\nSON");   
  TV.delay(1500);   
  TV.clear_screen();
  TV.delay(500);        
  TV.println(myStrings[0]);              // YOU can use 'character array' names as variables to drop in strings from elsewhere in the code
  TV.delay(3000);
  TV.clear_screen();
  TV.println(message1);
  TV.print("Length of message 1 = ");
  TV.println(size1);
  TV.print("X position to center string 1 = ");
  TV.println(x_start);
  TV.set_cursor(x_start,48); //center the  message
  TV.print(message1);        //now centered :)
  TV.delay(3000);


}


// FONTS:  code that generates fonts..."0B00..." 'ob' is a prefix noting its in binary and then then next 8 characters infor the shape

// Blink String Function:
// Arguments: Sting, Blink Period, Times to Blink

// Print String For Function:
// Arguments: String, Show for Time

// 
