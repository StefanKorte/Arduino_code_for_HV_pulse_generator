//**********************************************************************//
//  Arduino pulse generator code
//  
//  Outputs A4 and A5 are used to charge a coil.
//  The charge time will determine the pulse voltage.
//
//  An encoder is used to set a voltage.
//  The necessary charge time gets calculated.
//
//  The set voltage is displayed on a 8x2 LCD.
//
//  The output is set to HIGH for the calculated charge time.
//  When the output is switched back to LOW, the pulse is generated.
//
//  The coil charged by A4 will generate positive pulses,
//  the coil charged by A5 will generate negative pulses.
//
//**********************************************************************//

//--- includes ---//

  #include <math.h>
    // needed for logarithm
  #include <Encoder.h>
    // from https://github.com/PaulStoffregen/Encoder
    // -> downloaded version in pulse_generator folder
  #include <LiquidCrystal.h>
    // from https://www.arduinolibraries.info/libraries/liquid-crystal
    // -> downloaded version in pulse_generator folder

//--- initialize hardware ---//

LiquidCrystal lcd(9, 8, 5, 4, 7, 6);
  // LiquidCrystal lcd(RS, E, D4, D5, D6, D7) insert used Arduino pins here

Encoder myEnc1(2, 3);
  // insert used Arduino pins here

const int pos_pin = A4;
  // output to MAX4420 for positive pulse polarity
const int neg_pin = A5;
  // output to MAX4420 for negative pulse polarity

//--- set variables and constants ---//

long  enc_position  = 0;
  // initialize
int   pulses_per_polarity = 25;
  // number of pulses before polarity is reversed
long  pulse_delay = 1000;
  // number of milliseconds between pulses
int   disp_update = 50;
  // number of milliseconds between display updates
int   step_size = 10;
  // Volt per encoder step (variable)
  int   polarity = 1; 
  // starts with positive polarity
int   pulse_number = 0;
int   blink_status = 0;

//--- setup is run once when Arduino starts ---//

void setup() 
{
  lcd.begin(8, 2);
  lcd.setCursor(0, 0);
  lcd.print("Puls-gen");

  pinMode(pos_pin, OUTPUT);
  pinMode(neg_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);  
}

//--- loop will run indefinitely and will create a pulse with each loop---//

void loop()
{
    //--- voltage calibration constants ---//
  double alpha = -0.0005; 
    // - L/R_coil, coil charging constant
  double U_max = 700;
    // max output voltage

  int i = 0;
  int delay_time;
  int U_set;

  lcd.begin(8, 2); // neccessary in case the last pulse deactivated the display

  //--- in between pulses, the encoder is read repeatedly and the display updated ---//
  while (i++ < (pulse_delay / disp_update)) 
  {
    enc_position = myEnc1.read();

    if (enc_position < 0) // reset to 0 if negative encoder value
    {
      enc_position = 0;
      myEnc1.write(0);
    }
    else if ((enc_position + 2) / 4 * step_size > U_max) // reset to U_max if voltage > U_max
    {
      enc_position = 4 * ( U_max - 2) / step_size;
      myEnc1.write( 4 * ( U_max - 2) / step_size);
    }
    //--- removing noise from encoder output ---//
    enc_position = (enc_position + 2) / 4;
    //--- calculating set voltage ---//
    U_set = enc_position * step_size;

    //--- time to charge the coil for U_set ---//
    //--- charging a coil follows exponential law, thus the delay is proportional to the log ---//
    delay_time =  1/alpha * log (1.0 - (double)U_set / (U_max + 1));
    if (delay_time > 10000)
      delay_time = 10000;  // max time before capacitor depletes and current drops.

    //--- U_set and delay_time are written to display ---//
    lcd.setCursor(0, 0);
    lcd.print(polarity * U_set);
    lcd.print("V        ");
    lcd.setCursor(0, 1);
    lcd.print(delay_time);
    lcd.print("us        ");
    delay(disp_update);
  }
  
  if (delay_time) // not run for delay_time == 0
  {
    //--- coil charging for positive voltage ---//
    if (polarity == 1)
    {
      digitalWrite(pos_pin, HIGH);
      delayMicroseconds(delay_time);
      //delay(1000);
      digitalWrite(pos_pin, LOW);
    }
    //--- coil charging for negative voltage ---//
    else
    {
      digitalWrite(neg_pin, HIGH);
      delayMicroseconds(delay_time);
      //delay(1000);
      digitalWrite(neg_pin, LOW);
    }
  }

  //--- Arduino nano led blinks once for 2 pulses ---//
  blink_status = (blink_status + 1) % 2;      
  if (blink_status % 2)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);

  //--- pulses get counted ---//
  pulse_number = (pulse_number + 1) % pulses_per_polarity;
  
  //--- if max pulse count reached, polarity is reversed ---//
  if (!pulse_number)
    polarity *= -1;
}
