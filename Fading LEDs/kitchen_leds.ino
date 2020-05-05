//define pins for the LEDs (using different ones for differnet strips to prevent MOSFET overloading and allow for more flexibility in the fade up sequence)
#define LIGHT_LED 3   // under cupboard downlights
#define ONBOARD_LED 13  // so we can see what is supposed to be happening, just with the onboard LED.

#define SENSOR 2 // PIR module
#define SWITCH 7 // push to make switch

//overall brightness value
int stripbright1 = 0;
int lightbright = 0;
int ledmaxbrightness = 100; // this is the maximum PWM brightness for the LED strips (the under cabinet LED lights can be full 255 brightness, so we don't worry about those).
                           // note that I set this at 80 for a good lighting for my use - remember the lights don't follow a linear brightness so have a play to see what works                            

int fadeSpeed1 = 20;
int fadeSpeed2 = 10; //slightly faster fade out time

int offset = 10; //offset allows for one light to begin the glow after the other one
int totaltime = 255 + (offset * 4); // 255 is the maximum brightness value

bool stillactive = false;
bool switchactive = false;
bool switchpressed = false;

unsigned long previousMillis = 0;         // will store last time LED was updated
unsigned long interval = 60000;           // interval at which to wait in an ON state (milliseconds) - basically 1 minutes since last movement - change this temporarily for testing to 10000
unsigned long currentMillis = millis();   // defining the variable - it will get set to the current time when things need to start/stop
unsigned long switchpressedMillis = 0;    //used to track debounce 
unsigned long debouncetime = 250;

void setup() {

  Serial.begin(9600);
  //setup sensor and switch pins to input
  pinMode (SENSOR, INPUT_PULLUP);
  pinMode (SWITCH, INPUT);

  //set up LED pins to output.
  pinMode(LIGHT_LED, OUTPUT);

  //setup the onboard LED to show the same state
  pinMode(ONBOARD_LED, OUTPUT);

  //and set the lights to off
  analogWrite(LIGHT_LED, 0);
  analogWrite(ONBOARD_LED, 0);
}

void loop() {

//flash the light to show it in a ready state.
  analogWrite(ONBOARD_LED, 255);
  delay(250);
  analogWrite(ONBOARD_LED, 0);
  delay(250);
  analogWrite(ONBOARD_LED, 255);
  delay(250);
  analogWrite(ONBOARD_LED, 0);
  delay(250);

  switchactive = false;
  //wait for the sensor
  do {
        currentMillis = millis(); //twiddle thmbs, waiting for the sensor to trigger
  } while (digitalRead(SENSOR) == LOW);  // you could also wait for the button here but make sure to DEBOUNCE the input 

  //Once triggered, turn on

  TurnOn(); 
  previousMillis = millis();
  stillactive = true;
  Serial.println("Motion detected");

  // and keep on for a few seconds unless triggered again
  
  do {      
   currentMillis = millis();

   if ((currentMillis - previousMillis >= interval) or (currentMillis < 5)) { // if the 'on' time has passeed OR the internal timer has reset to 0 (after c.50 days)
      stillactive = false ;//QUIT the loop
    }
    if (switchactive == true) {  //if sensor triggered
      previousMillis = currentMillis; //reset the counter to allow constan movement to keep the lights on 
            Serial.println("switch mode enabled");
    }
    if (digitalRead(SENSOR) == HIGH)  {  //if sensor triggered
      previousMillis = currentMillis; //reset the counter to allow constan movement to keep the lights on 
            Serial.println("sensor triggered");
    }
    
    if (digitalRead(SWITCH) == LOW) {  // if switch pushed
 
    //debounce the switch
      switchpressedMillis = currentMillis; 
      switchpressed = true;
      do {
        if (digitalRead(SWITCH) == HIGH) {
          switchpressed = false;
          }
        switchpressedMillis = millis(); 
        } while (switchpressedMillis - currentMillis < debouncetime) ;

      if (switchpressed == true) {
        Serial.println("switch pressed");
                
        if (switchactive == true) {
          Serial.println("switch pressed to STOP the lights");
          analogWrite(LIGHT_LED, 0);
          delay(250); // remind you it has been set and allow you time to let go of the button
          analogWrite(LIGHT_LED, 255);
          delay(1000); 
          switchactive = false; // reset the variable
          stillactive = false; // quit the loop
        }
        else {
          switchactive = true; //set the variable to show the switch was activated
          Serial.println("switch pressed to extend the lights");
          analogWrite(LIGHT_LED, 0);
          delay(250); // remind you it has been set and allow you time to let go of the button
          analogWrite(LIGHT_LED, 255);
          delay(250); 
          analogWrite(LIGHT_LED, 0);
          delay(250);  
          analogWrite(LIGHT_LED, 255);
          delay(500); // allow you time to let go of the button

        }
      }  

    }
  } while (stillactive == true);
    Serial.println("Time finished - turning off");
    TurnOff();
}


void TurnOn() {
  analogWrite(ONBOARD_LED, 255); // show the lights are supposed to be on
  for (int i = 0; i < totaltime; i++) {
    lightbright = constrain (i, 0, ledmaxbrightness); //maximum brightness set to a lower brightness - could be 255 but I prefer them a little dimmer - this also lengthens the life of the MOSFETs and LEDs
    analogWrite(LIGHT_LED, lightbright);
    delay(fadeSpeed1);
  }
}

void TurnOff() {
  analogWrite(ONBOARD_LED, 0); // show the lights are supposed to be off
  for (int i = 255; i >= 0; --i) {
    analogWrite(LIGHT_LED, constrain (i, 0, ledmaxbrightness));
    delay(fadeSpeed2);
  }
}
