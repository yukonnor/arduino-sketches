// This sketch is usig the "Custom VideoShield" for pots and buttons.
#include <Adafruit_NeoPixel.h>

// Define shield pins
#define pot1Pin A0
#define pot2Pin A2 //note here that the stripboard wires cross, hence out of order
#define pot3Pin A1
#define pot4Pin A3
#define button1Pin 13
#define button2Pin 12
#define button3Pin 11
#define button4Pin 10

// Define shield variables
byte current_color = 0; 
int pot_value[4] = {0,0,0,0};
int last_pot_value[4][2] = {{0,0}, {0,0}, {0,0}, {0,0}} ;  // 1-3 currently using JUST for r,g,b colors
int pot_value_diff[3] = {0,0,0};
bool actively_measure_pot[4] = {true,true,true,true};

// Define delay variables
unsigned long previousMillis = 0;
int delayInterval = 50;

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120
#define S_PIXEL_COUNT 16  // pixels 0 - 15, 22.5
#define M_PIXEL_COUNT 24  // pixels 16 - 39, 15 degs
#define L_PIXEL_COUNT 35  // pixels 40 - 74, 10.3 degs
#define XL_PIXEL_COUNT 45 // pixels 75 - 119, 8 degs
const byte S_PIXEL_START = 0;
const byte S_PIXEL_END = 15;
const byte M_PIXEL_START = S_PIXEL_COUNT;                  // 16
const byte M_PIXEL_END = M_PIXEL_START + M_PIXEL_COUNT;    // 40
const byte L_PIXEL_START = M_PIXEL_START + M_PIXEL_COUNT;  // 40
const byte L_PIXEL_END = L_PIXEL_START + L_PIXEL_COUNT;    // 75
const byte XL_PIXEL_START = L_PIXEL_START + L_PIXEL_COUNT; // 75
const byte XL_PIXEL_END = XL_PIXEL_START + XL_PIXEL_COUNT; // 120

byte pixel_colors[LED_COUNT][3];                     // array to store colors in their original, primary position
byte shifted_pixel_colors[LED_COUNT][3];             // array to start colors after being shifted
byte gradient_colors[2][3] = {{0, 0, 0}, {0, 0, 0}}; // array of 2 rbg colors for the gradient

bool adjust_colors = true; // set whether you want to adjust colors or not (to avoid adjusting to same color every loop)

bool free_shift = true;  // mode for shifting colors around each ring individually
bool sync_shift = false; // mode for shifting colors around all rings in sync

int xl_shift_counter = 0;
int l_shift_counter = 0;
int m_shift_counter = 0;
int s_shift_counter = 0;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // (Total number of pixels, Arduino pin number, Pixel type flags)

void light_leds()
{
  // This function sets the pixel colors based on the final color setting (currently the color after they have been shifted)
  for (int pixel = 0; pixel < 120; pixel++)
  {
    strip.setPixelColor(pixel, shifted_pixel_colors[pixel][0], shifted_pixel_colors[pixel][1], shifted_pixel_colors[pixel][2]);
    // Serial.print("Pixel ");Serial.print(pixel);Serial.print(": Red: ");Serial.println(shifted_pixel_colors[pixel][0]);
  }

  strip.show(); // update the colors of the pixels
}

void set_gradient_color(byte color_num, byte r, byte g, byte b)
{
  // set the two colors for the gradient in the gradient color array
  gradient_colors[color_num][0] = r;
  gradient_colors[color_num][1] = g;
  gradient_colors[color_num][2] = b;
}

byte get_ring_pixel_count(char circle)
{
  // define pixel count and offset for each circle type
  if (circle == 's')
  {
    return S_PIXEL_COUNT;
  }
  else if (circle == 'm')
  {
    return M_PIXEL_COUNT;
  }
  else if (circle == 'l')
  {
    return L_PIXEL_COUNT;
  }
  else if (circle == 'x')
  {
    return XL_PIXEL_COUNT;
  }
}

byte get_ring_first_pixel(char circle)
{
  if (circle == 's')
  {
    return S_PIXEL_START;
  }
  else if (circle == 'm')
  {
    return M_PIXEL_START;
  }
  else if (circle == 'l')
  {
    return L_PIXEL_START;
  }
  else if (circle == 'x')
  {
    return XL_PIXEL_START;
  }
}

void apply_gradient(char circle, byte color_1[], byte color_2[])
{ // Applies the color gradient between two colors on a given pixel ring to the main pixel_colors array.
  // TODO: define array size?

  byte pixels_per_color = get_ring_pixel_count(circle) / 2; // find how many pixels each color should have based on the ring
  byte ring_first_pixel = get_ring_first_pixel(circle);     // was circle_offset    // determines where the first color should start based on which circle we're working on

  // add one to pixels_per_color for the rings with odd # leds
  if (pixels_per_color % 2 == 1)
    pixels_per_color++;

  // for each color of the 2 color array
  for (int color = 0; color < 2; color++)
  {
    // figure out which range of pixels we're working on based on the circle: [start pixel, end pixel]
    byte current_range[2];

    // to fix bug of xl 2nd color going out of array range
    if (circle == 'x' && color == 1)
    {
      current_range[0] = (color * pixels_per_color) + ring_first_pixel; // 22 + 75 = 97
      current_range[1] = ((color + 1) * pixels_per_color) + ring_first_pixel + 1; // 44 + 75 + 1 = 120
    }
    else
    {
      current_range[0] = (color * pixels_per_color) + ring_first_pixel;  // 75
      current_range[1] = ((color + 1) * pixels_per_color) + ring_first_pixel; // 22 + 75 = 97
    }

    byte final_color[3];

    // set the starting color for the range (if color = 0 it's red. if color = 1 it's blue)
    if (color == 0)
    {
      final_color[0] = color_1[0]; // r
      final_color[1] = color_1[1]; // g
      final_color[2] = color_1[2]; // b
      // Serial.print("final_color_1 red: "); Serial.println(final_color[0]);
    }
    if (color == 1)
    {
      final_color[0] = color_2[0];
      final_color[1] = color_2[1];
      final_color[2] = color_2[2];
      // Serial.print("final_color_2 red: "); Serial.println(final_color[0]);
    }

    // Serial.print("Seeing if we can get into gradient color loop: start "); Serial.print(current_range[0]); Serial.print(" end: "); Serial.println(current_range[1]);
    for (int pixel = current_range[0]; pixel < current_range[1]; pixel++)
    {
      // apply the gradient color to the main pixel color array:
      pixel_colors[pixel][0] = final_color[0];
      pixel_colors[pixel][1] = final_color[1];
      pixel_colors[pixel][2] = final_color[2];
      // Serial.print("Apply final gradient color to pixel_colors pixel: "); Serial.print(pixel); Serial.print(" red: "); Serial.println(pixel_colors[pixel][0]);

      // define next color in the gradient:
      if (color == 0)
      {
        final_color[0] += int((color_2[0] - color_1[0]) / pixels_per_color);
        final_color[1] += int((color_2[1] - color_1[1]) / pixels_per_color);
        final_color[2] += int((color_2[2] - color_1[2]) / pixels_per_color);
      }
      if (color == 1)
      {
        final_color[0] += int((color_1[0] - color_2[0]) / pixels_per_color);
        final_color[1] += int((color_1[1] - color_2[1]) / pixels_per_color);
        final_color[2] += int((color_1[2] - color_2[2]) / pixels_per_color);
      }
    }
    // Serial.println("End of gradient colors being applied to pixel_colors "); 
  }
  // Serial.println("End of apply gradient function");
}

// shift the pixels around the individual circles
int shift_pixels(char circle, byte shift_amt, int shift_counter)
{
  byte pixel_count = get_ring_pixel_count(circle);
  byte first_pixel = get_ring_first_pixel(circle);

  shift_counter += shift_amt;

  // reset the counter once it does a full loop.
  if (shift_counter == pixel_count)
    shift_counter = 0;
  if (shift_counter == -1 * pixel_count)
    shift_counter = 0;

  for (int i = 0; i < pixel_count; i++)
  {
    byte index_to_get = first_pixel + ((i + shift_counter) % pixel_count);

    // fix c's handling of negative modulos
    if (index_to_get < 0)
    {
      index_to_get = pixel_count + index_to_get;
    }

    shifted_pixel_colors[i + first_pixel][0] = pixel_colors[index_to_get][0];
    shifted_pixel_colors[i + first_pixel][1] = pixel_colors[index_to_get][1];
    shifted_pixel_colors[i + first_pixel][2] = pixel_colors[index_to_get][2];
  }

  // for (int i = 0; i < S_PIXEL_COUNT; i++) {
  //   Serial.print("Shifted_pixel_color for pixel "); Serial.print(i); Serial.print(" ");
  //   Serial.print(shifted_pixel_colors[i][0]); Serial.print(" ");  Serial.print(shifted_pixel_colors[i][1]); Serial.print(" ");  Serial.println(shifted_pixel_colors[i][2]);
  // }

  return shift_counter;
}

void update_color(byte color_num) {

  pot_value_diff[color_num] = abs(last_pot_value[color_num][current_color] - pot_value[color_num]);

  if (pot_value_diff[color_num] < 5 || actively_measure_pot[color_num] == true)
  {
    actively_measure_pot[color_num] = true;              // keep measuring pot once the current value comes close to old value
    int color = map(pot_value[color_num], 0, 1023, 0, 255); 

    // now that we're measuring, if the pot is twisted, adjust color
    if (pot_value_diff[color_num] > 10)
    {
      adjust_colors = true;
    } else 
    {
      adjust_colors = false;
    }

    if (adjust_colors) 
    {
      gradient_colors[current_color][color_num] = color;

      apply_gradient('s', gradient_colors[0], gradient_colors[1]);
      apply_gradient('m', gradient_colors[0], gradient_colors[1]);
      apply_gradient('l', gradient_colors[0], gradient_colors[1]);
      apply_gradient('x', gradient_colors[0], gradient_colors[1]);
    }

    last_pot_value[color_num][current_color] = pot_value[color_num];
  }
}

void switch_current_color()
{
  int buttonRead1 = digitalRead(button1Pin);

  //if button 1 pressed, switch current color
  if (buttonRead1 == LOW) {

     Serial.println("Button 1 pressed."); 

     // stop measuring pot values
     actively_measure_pot[0] = false;
     actively_measure_pot[1] = false;
     actively_measure_pot[2] = false;

     // switch color
     if (current_color == 0) 
     {
       current_color = 1;
     }
     else current_color = 0;
  }
}

void readPots() {
  pot_value[0] = analogRead(pot1Pin); // Read the potentiometer value
  pot_value[1] = analogRead(pot2Pin); // Read the potentiometer value
  pot_value[2] = analogRead(pot3Pin); // Read the potentiometer value
  pot_value[3] = analogRead(pot4Pin); // Read the potentiometer value
}

void adjust_delay_interval() {
  delayInterval = map(pot_value[3], 0, 1023, 20, 500); 
}

void setup()
{
  Serial.begin(9600); // open the serial port at 9600 bps:

  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);

  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.begin();      
  strip.clear();
  strip.show();                            // Turn OFF all pixels (turns off as no pixel colors have been defined yet)
  strip.setBrightness(5);                  // Set BRIGHTNESS (max = 255)
  Serial.println("Hello. Setup running."); // trying to debug why setup is looping...

  // setup initial colors
  // set the gradient colors
  set_gradient_color(0, 255, 0, 0);
  set_gradient_color(1, 0, 0, 255);

  // apply the gradient using the gradient colors
  apply_gradient('s', gradient_colors[0], gradient_colors[1]);
  apply_gradient('m', gradient_colors[0], gradient_colors[1]);
  apply_gradient('l', gradient_colors[0], gradient_colors[1]);
  apply_gradient('x', gradient_colors[0], gradient_colors[1]);

  // for (int i = 0; i < S_PIXEL_COUNT; i++) {
  //     Serial.print("Initial RED color for pixel "); Serial.print(i); Serial.print(" "); Serial.println(pixel_colors[i][0]);
  // }


  // set the initial shifted pixel array values:
  s_shift_counter = shift_pixels('s', 0, s_shift_counter);
  m_shift_counter = shift_pixels('m', 0, m_shift_counter);
  l_shift_counter = shift_pixels('l', 0, l_shift_counter);
  xl_shift_counter = shift_pixels('x', 0, xl_shift_counter);

  delay(500);
  Serial.print("....end of SETUP.....\n");
}

void loop()
{
  unsigned long currentMillis = millis();

  readPots();

  // if we've waited long enough, run the code to update and set the LED colors
  if (currentMillis - previousMillis >= delayInterval) 
  {
    previousMillis = currentMillis;

    // shift the small circle's led colors
    s_shift_counter = shift_pixels('s', 1, s_shift_counter);
    m_shift_counter = shift_pixels('m', 1, m_shift_counter);
    l_shift_counter = shift_pixels('l', 1, l_shift_counter);
    xl_shift_counter = shift_pixels('x', 1, xl_shift_counter);

    // get new color values
    update_color(0);
    update_color(1);
    update_color(2); 
    switch_current_color();

    // update the led colors and show them
    light_leds();
  }
  
  adjust_delay_interval();
  // Serial.print(".....end of LOOP.....\n");
}
