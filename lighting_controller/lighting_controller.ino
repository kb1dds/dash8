/* Dash 8 lighting controller
 *  Controls 
 *  1. Two anti-collision strobe pairs (two red strobes on fuselage, two red strobes on wingtips, timed separately)
 *  2. Landing lights controlled by receiver channel.  When channel duty cycle exceeds 8%, landing lights are activated
 */

// Wiring: pin definitions
#define LANDING_IN 2 // PWM signal input
#define LANDING_OUT 3   // White landing lights
#define STROBE_RD_OUT 4 // Red strobes on fuselage
#define STROBE_WT_OUT 5 // White strobes on wingtip

// Strobe timing data (milliseconds)
#define STROBE_ON_TIME 100
#define STROBE_PERIOD 1000
#define STROBE_OFFSET 400 // Offset between red and white strobes

// Landing light state data
int landing;
int landing_debounce;
#define LANDING_PERIOD 1000 // milliseconds, threshold between on/off control
#define LANDING_DEBOUNCE 50 // PWM intervals for debounce

// PWM timer data
volatile unsigned long pwm_rising;
volatile unsigned long pwm_falling;

int pwm_int[LANDING_DEBOUNCE];

// Handler for PWM input rising edge, which sets landing light state
void pwm_handler()
{
  if(digitalRead(LANDING_IN)){
    pwm_rising = micros();
  }
  else {
    pwm_falling = micros();
  }
}

void setup() {
  // Setup pins
  pinMode(STROBE_RD_OUT,OUTPUT);
  pinMode(STROBE_WT_OUT,OUTPUT);
  pinMode(LANDING_OUT,OUTPUT);
  pinMode(LANDING_IN,INPUT);

  // Setup PWM input timer
  pwm_rising=0;
  pwm_falling=0;

  // Configure PWM input to catch first rising edge
  attachInterrupt(digitalPinToInterrupt(LANDING_IN),pwm_handler,CHANGE);

  // Landing light state
  landing = 0;
  landing_debounce = 0;

//  Serial.begin(9600);
}

void loop() {
  // Red strobe control
  if( (millis() % STROBE_PERIOD) < STROBE_ON_TIME ) {
    digitalWrite(STROBE_RD_OUT,1);
  }
  else {
    digitalWrite(STROBE_RD_OUT,0);
  }

  // White strobe control
  if( ((millis()-STROBE_OFFSET) % STROBE_PERIOD) < STROBE_ON_TIME ) {
    digitalWrite(STROBE_WT_OUT,1);
  }
  else {
    digitalWrite(STROBE_WT_OUT,0);
  }

  // Landing lights debounce logic
  for( int i = LANDING_DEBOUNCE-1; i > 0; i -- ){
    pwm_int[i] = pwm_int[i-1];
  }
  pwm_int[0]=pwm_falling-pwm_rising;
  if( pwm_int[0] < LANDING_PERIOD/2 ){
    pwm_int[0] = LANDING_PERIOD;
  }

  double pwm=0;
  for( int i = 0; i < LANDING_DEBOUNCE; i ++ ){
    pwm += pwm_int[i]/LANDING_DEBOUNCE;
  }
  //Serial.println(pwm);

  /*
  if( pwm > LANDING_PERIOD ){
    landing = 1;
  }
  else{
    landing = 0;
  }*/

  // Force landing lights on
  landing = 1;

  // Light the landing lights
  if( landing ){
    digitalWrite(LANDING_OUT,1);
  }
  else{
    digitalWrite(LANDING_OUT,0);
  }
}
