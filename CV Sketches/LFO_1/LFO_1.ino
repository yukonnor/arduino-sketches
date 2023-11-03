/*
 * LFO based on Seymour Clufley's code: 
 * http://discuss.littlebits.cc/t/arduino-lfo-code-included/18567
 */

float max_pc = 100;
float max_in_voltage = 1024;
float last_freq = -1;
unsigned long endcyclems = 0;
float rem_pc = 100;
float cycleperiod = 0;

void setup() {
  pinMode(A1, INPUT); //controls speed of LFO
  pinMode(5, OUTPUT); //current stage of LFO out
  pinMode(9, OUTPUT); //LFO freq output
  Serial.begin(9600);
}

void loop() {
  unsigned long nowms = millis();
  float freq = max_pc / max_in_voltage * 10; //analogRead(A1);
  freq = max(freq,0.1);
  
  if (FloatDif(freq, last_freq) > 1 || nowms > endcyclems) {
    last_freq = freq;
    cycleperiod = freq * 500;
    endcyclems = nowms + (cycleperiod / max_pc * rem_pc);
  }
  
  float pc = max_pc - (max_pc / cycleperiod * (endcyclems - nowms) );
  
  WritePercentage(5, pc/10);
  
  float lfofreqpc = max_pc;
  
  if (pc < 50) {
    lfofreqpc = pc * 2;
  } else { 
    if (pc > 50) {
      lfofreqpc = (100 - pc) * 2;
    }
  }
  
  WritePercentage(9, lfofreqpc);
  
  rem_pc = max_pc - pc;
  if (pc > 99) {
    rem_pc = 100;
  }

  Serial.print("LFO Frequency = ");
  Serial.print(lfofreqpc);
  Serial.print("    LFO Value = ");
  Serial.println(pc);
}

float FloatDif(float f1, float f2) {
  if (f1 < f2) {
    return f2 - f1;
  }
  return f1 - f2;
}

void WritePercentage(int outpin, float pctg) {
  int adjpctg = constrain(pctg * 2.55, 0, 255);
  analogWrite(outpin, adjpctg);
}
