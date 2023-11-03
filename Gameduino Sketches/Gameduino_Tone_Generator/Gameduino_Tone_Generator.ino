//Arduino for Musicians
//Listing 7.13: Gameduino Additive Synthesizer

#include <SPI.h>
#include <GD.h>

const int SINE = 0;
const int NUM_POTS = 5;
const int STEP = 2;

typedef unsigned char prog_uchar;

//Define a struct to hold sinewave paramters
struct SineParameters{
  long last_time;  //Updated at the end of each wait_time cycle
  long wait_time;  //Amount of time to wait before altering amplitude 
  int amplitude;   //The current amplitude or level of the sine wave
  boolean add;     //True if amplitude is currently being incremented. 
  float frequency; //The frequency of the sine wave
};


//Create an array of structs
SineParameters sine_wave[NUM_POTS];


void setup()
{
    //Initialize a sawtooth wave
    initSawtooth(400, 100);
      
    //Initialize the Gameduino
    GD.begin();
    
    //TEST: Uncomment and change values to experiment sans potentiometers
    //(also comment out the analogRead() line in the main loop())
    //sine_wave[0].wait_time = 1000;
    //sine_wave[1].wait_time = 800;
    //sine_wave[2].wait_time = 600;
    //sine_wave[3].wait_time = 300;
    //sine_wave[4].wait_time = 200;
    //sine_wave[5].wait_time = 100;
}

void loop()
{
    //Get the current time
    long current_time = millis();
    
    //Update the wait time of each sine wave based on the value of pots 1-6
    for(int i = 0; i< NUM_POTS; i++)
    {
        //Comment the following line to experiment sans potentiometers
        sine_wave[i].wait_time = analogRead(i);
        if(current_time - sine_wave[i].last_time >= sine_wave[i].wait_time)
        {
            updateSineParams(i);
            sine_wave[i].last_time = current_time; 
        }
        
         GD.voice(i, SINE, sine_wave[i].frequency, sine_wave[i].amplitude, 
                   sine_wave[i].amplitude);
    }
    
    //Call updateFrequency(freq) to update the frequency in response to 
    //user input, algorithm, etc.
    //TESING: MAKING POT 6 control Frequency

    int pot_frequency = analogRead(5);
    
    updateFrequency(pot_frequency); // D
    //delay(2000);
    //updateFrequency(440); // A
    //delay(2000);
    //updateFrequency(392); // G
    //delay(2000);
    
}


void updateSineParams(int i)
{
    if(sine_wave[i].add == true)
    {
       sine_wave[i].amplitude += STEP;
       if(sine_wave[i].amplitude > 100)
       {
           sine_wave[i].amplitude = 100;
           sine_wave[i].add = false;
       } 
    }else{
       sine_wave[i].amplitude -= STEP;
       if(sine_wave[i].amplitude < 0)
       {
           sine_wave[i].amplitude = 0;
           sine_wave[i].add = true;
       } 
    }
    
    
}

//Use this function to initialize the sine array as a sawtooth
void initSawtooth(int f0, int amplitude)
{
    //initialize overtones
    for(int i = 0; i < NUM_POTS; i++)
    {
        //Handle the fundamental differently than the overtones
        if(i == 0)
        {
             sine_wave[i].frequency = f0;
             sine_wave[i].amplitude = amplitude;
        }else{
            //Initialize overtones
            sine_wave[i].frequency = f0 * (i+1);
            sine_wave[i].amplitude = amplitude/ (i + 1); 
        }
        
        //These defaults are the same for the fundamental and overtones:
        sine_wave[i].last_time = 0;
        sine_wave[i].add = true;
    }  
}

//Use this function to update the frequency in real-time
void updateFrequency(int f0)
{     
    for(int i = 0; i < NUM_POTS; i++)
    {
        if(i == 0)  //Handle the fundamental differently than overtones
        {
           sine_wave[0].frequency = f0; 
        }else{
          sine_wave[i].frequency = f0 * (i+1);
        }
    }
}



