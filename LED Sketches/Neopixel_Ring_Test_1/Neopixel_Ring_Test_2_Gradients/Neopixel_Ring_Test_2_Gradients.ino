//TODO:
// For shifting, try to shift the colors in an array rather than shifting as part of the gradient function
// have separate function to light pixels based on the colors of the array

#include <Adafruit_NeoPixel.h>
#include <math.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120
#define S_PIXEL_COUNT 16   // pixels 0 - 15, 22.5
#define M_PIXEL_COUNT 24   // pixels 16 - 39, 15 degs
#define L_PIXEL_COUNT 35   // pixels 40 - 74, 10.3 degs
#define XL_PIXEL_COUNT 45  // pixels 75 - 119, 8 degs
const int M_PIXEL_START = S_PIXEL_COUNT; // 16
const int M_PIXEL_END = M_PIXEL_START + M_PIXEL_COUNT; // 40
const int L_PIXEL_START = M_PIXEL_START + M_PIXEL_COUNT; // 40
const int L_PIXEL_END = L_PIXEL_START + L_PIXEL_COUNT;   // 75
const int XL_PIXEL_START = L_PIXEL_START + L_PIXEL_COUNT; // 75 
const int XL_PIXEL_END = XL_PIXEL_START + XL_PIXEL_COUNT; // 120


short pixel_colors[LED_COUNT][3];                // array to store colors in their original, primary position 
short shifted_pixel_colors[LED_COUNT][3];        // array to start colors after being shifted
short gradient_colors[2][3] = {{0,0,0},{0,0,0}}; // array of 2 rbg colors for the gradient

bool adjust_colors = true;                       // set whether you want to adjust colors or not (so you don't pointless adjust to same color every loop)

const float micro_step_amt = 0.02;
bool  free_shift = false;            // mode for shifting colors around each ring individually
bool  sync_shift = true;             // mode for shifting colors around all rings in sync
float xl_shift_counter = 0.00;
float l_shift_counter = 0.00;
float m_shift_counter = 0.00;
float s_shift_counter = 0.00;s

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // (Total number of pixels, Arduino pin number, Pixel type flags)

void light_leds(){
  // This function sets the pixel colors based on the final color setting (currently the color after they have been shifted)
  for(int pixel = 0; pixel < LED_COUNT; pixel++){
    strip.setPixelColor(pixel, shifted_pixel_colors[pixel][0], shifted_pixel_colors[pixel][1], shifted_pixel_colors[pixel][2]);
  }
  strip.show(); // update the colors of the pixels
}


void set_gradient_color(short color_num, short r, short g, short b) {
  // set the two colors for the gradient in the gradient color array
  gradient_colors[color_num][0] = r;
  gradient_colors[color_num][1] = g;
  gradient_colors[color_num][2] = b;
}

void set_gradient(char circle, short color_1_array[], short color_2_array[]){     // TODO: define array size?
    // Set a gradient between two colors on a given pixel ring. Stores the colors in the base-level 'color' array, which is later shifted for the final pixel display
    short pixels_per_color = 0; // find how many pixels each color should have based on the ring
    short circle_offset = 0;    // determines where the color should start based on which circle we're working on
 
    //Serial.print(color_1_array[0]); Serial.print(" "); Serial.print(color_1_array[1]); Serial.print(" ");  Serial.println(color_1_array[2]);
    //Serial.println(colors_array[1]);

    // define pixel count and offset for each circle type
    if (circle == 's') {
      circle_offset = 0;
      pixels_per_color = S_PIXEL_COUNT / 2;
      }
    else if (circle == 'm') {
      circle_offset =  M_PIXEL_START;
      pixels_per_color = M_PIXEL_COUNT / 2;
      }
    else if (circle == 'l') {
      circle_offset =  L_PIXEL_START;
      pixels_per_color = 18;
      }
    else if (circle == 'x') {
      circle_offset =  XL_PIXEL_START;
      pixels_per_color = 23;
      }
   
    for(int color = 0; color < 2; color++) {           
        // figure out which range of pixels we're working on based on the circle
        short current_range[2] = {(color * pixels_per_color) + circle_offset, ((color + 1) * pixels_per_color) + circle_offset}; 
        short color_1_start; 
        short color_2_start;
        short final_color[3];

        // set the first color for the range (if color = 0 it's red. if color = 1 it's blue)
        if (color == 0) {
          final_color[0] = color_1_array[0]; 
          final_color[1] = color_1_array[1]; 
          final_color[2] = color_1_array[2]; 
        }
        if (color == 1) {
          final_color[0] = color_2_array[0]; 
          final_color[1] = color_2_array[1]; 
          final_color[2] = color_2_array[2]; 
        }

        for(int pixel = current_range[0]; pixel < current_range[1]; pixel++) {
            // define pixel color: 
            pixel_colors[pixel][0] = final_color[0];
            pixel_colors[pixel][1] = final_color[1];
            pixel_colors[pixel][2] = final_color[2];
            
            // define next color in the gradient: 
            if (color == 0) {
              final_color[0] += int((color_2_array[0]-color_1_array[0]) / pixels_per_color);
              final_color[1] += int((color_2_array[1]-color_1_array[1]) / pixels_per_color);
              final_color[2] += int((color_2_array[2]-color_1_array[2]) / pixels_per_color);
            }
             if (color == 1) {
              final_color[0] += int((color_1_array[0]-color_2_array[0]) / pixels_per_color);
              final_color[1] += int((color_1_array[1]-color_2_array[1]) / pixels_per_color);
              final_color[2] += int((color_1_array[2]-color_2_array[2]) / pixels_per_color);
            }
        }
    }
}

// shift the pixels around the individual circles
void shift_pixels(short s_shift_amt, short m_shift_amt, short l_shift_amt, short xl_shift_amt){
    for(int i = 0; i < S_PIXEL_COUNT; i++) {
        short index_to_get = 0 + ((i + s_shift_amt) % S_PIXEL_COUNT);
        shifted_pixel_colors[i + 0] = pixel_colors[index_to_get];
    }
    for(int i = 0; i < M_PIXEL_COUNT; i++) {
        short index_to_get = M_PIXEL_START + ((i + m_shift_amt) % M_PIXEL_COUNT);
        shifted_pixel_colors[i + M_PIXEL_START] = pixel_colors[index_to_get];
    }
    for(int i = 0; i < L_PIXEL_COUNT; i++) {
        short index_to_get = L_PIXEL_START + ((i + l_shift_amt) % L_PIXEL_COUNT);
        shifted_pixel_colors[i + L_PIXEL_START] = pixel_colors[index_to_get];
    }
    for(int i = 0; i < M_PIXEL_COUNT; i++) {
        short index_to_get = XL_PIXEL_START + ((i + xl_shift_amt) % XL_PIXEL_COUNT);
        shifted_pixel_colors[i + XL_PIXEL_START] = pixel_colors[index_to_get];
    }

}


//////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);      
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels (turns off as no pixel colors have been defined yet)
  strip.setBrightness(5);  // Set BRIGHTNESS (max = 255)
}

void loop() {

  if (adjust_colors == true) {
      // set the gradient colors
      set_gradient_color(0, 255,0,0);
      set_gradient_color(1, 0,0,255);

      // apply the gradient using the gradient colors
      set_gradient('s', gradient_colors[0], gradient_colors[1]);
      set_gradient('m', gradient_colors[0], gradient_colors[1]); 
      set_gradient('l', gradient_colors[0], gradient_colors[1]);
      set_gradient('x', gradient_colors[0], gradient_colors[1]);

      adjust_colors = false;
  }
  
  // animate / shift the colors
  if (free_sync = true){
      shift_pixels(1, -1, 1, -1);  // (s, m, l, xl) shift steps
  }


  // full_sync_shift (shift all ring colors in unison)
  if (sync_shift = true) {
      short shift_dir = 1; 
      // TODO: determine if micro step amount can be less granular
      xl_shift_counter += micro_step_amt * shift_dir;
      l_shift_counter += micro_step_amt * shift_dir;
      m_shift_counter += micro_step_amt * shift_dir;
      s_shift_counter += micro_step_amt * shift_dir;

      if (xl_shift_counter == 1.00 * shift_dir){
        short xl_shift = 1 * shift_dir;
        xl_shift_counter = 0.00;
      }
      else {xl_shift = 0;}

      if (l_shift_counter == 1.28 * shift_dir){
        short l_shift = 1 * shift_dir;
        l_shift_counter = 0.00;
      }
      else {l_shift = 0;}

      if (m_shift_counter == 1.88 * shift_dir){
        short m_shift = 1 * shift_dir;
        m_shift_counter = 0.00;
      }
      else {l_shift = 0;}

      if (s_shift_counter == 2.80 * shift_dir){
        short s_shift = 1;
        s_shift_counter = 0.00;
      }
      else {l_shift = 0;}

      shift_pixels(s_shift, m_shift, l_shift, xl_shift);
  }

  
  light_leds();
  delay(1000);

}


