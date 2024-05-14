/*
 * -> Feedback Control Systems - Final Project
 * -> Authors:
 *    Luiz Pedro Bittencourt Souza - 12111EAU002 | github.com/luizpedrobt
 *    Fernanda de Carvalho Pinto - 12111EBI020 | github.com/fcarvalhop
 *
*/

//----------------------------------------------------------------
// Libs
//----------------------------------------------------------------

#include <TimerOne.h>

//----------------------------------------------------------------
// Physical pins
//----------------------------------------------------------------

#define sensorPin A0 
#define pwmCooler 11
#define pwmCarga 10

//----------------------------------------------------------------
// Sample Time (us)
//----------------------------------------------------------------

#define sampleTime 25000

//----------------------------------------------------------------
// PID parameters
//----------------------------------------------------------------

#define b0 -15
#define b1 20.325
#define b2 -5.4482
#define a1 -1
#define a2 0

//----------------------------------------------------------------
// System Variables
//----------------------------------------------------------------

int adcValue = 0;
float voltage = 0.0;
float temp = 0.0;

// Setpoint
float referencia = 50.0;

float erro = 0.0, erro_1 = 0.0, erro_2 = 0.0;
float controle = 0.0, saida_1 = 0.0, saida_2 = 0.0;
unsigned long previousMillis = 0;
const long interval = 1000;

//----------------------------------------------------------------
// Digital Filter 
//----------------------------------------------------------------

#define ORDEM 10
const double TAPS[ORDEM] = {-0.005498926503157899, 
  0.015086909771554665, 
  -0.018460113510057274, 
  -0.06953871743390073, 
  0.5784108476755612, 
  0.5784108476755612, 
  -0.06953871743390075, 
  -0.018460113510057278, 
  0.015086909771554665, 
  -0.005498926503157899};

typedef struct {
  double *history;
  const double *taps;
  unsigned int last_index;
  unsigned int filterOrder;
} DigitalFilter;

DigitalFilter digitalFilter_1; //Digital filter structure

//----------------------------------------------------------------
// Functions structure
//----------------------------------------------------------------

void getTemp(); // Prints the temperature and applies the PID

void DigitalFilter_init(DigitalFilter* f, const int filterOrder, const double *filterTaps); // Initializes the digital filter declared
void DigitalFilter_put(DigitalFilter* f, double input); // Associates a value read by the ADC to a Digital Filter
double DigitalFilter_get(DigitalFilter* f); // Returns the value of the signal after the Digital Filter

//----------------------------------------------------------------
// Functions
//----------------------------------------------------------------

void getTemp() {

  adcValue = analogRead(sensorPin);
  DigitalFilter_put(&digitalFilter_1, adcValue);
  
  voltage = (5.0/1023.0) * DigitalFilter_get(&digitalFilter_1);
  temp = -101.099 * voltage + 347.473; // A equation relating the voltage across a 1N4007 diode to ambient temperature

  erro = referencia - temp; // The difference between the current temperature and the setpoint

  controle = -a1*saida_1 - a2*saida_2 + b0*erro + b1*erro_1 + b2*erro_2; // Difference equation

  controle = min(controle, 255); // Saturation control
  controle = max(controle, 0); // Saturation control

  erro_2 = erro_1;
  erro_1 = erro;
  saida_2 = saida_1;
  saida_1 = controle;
  
  analogWrite(pwmCooler, (int)controle); // Uses the result of the PID to generate a PWM waveform based on the current temperature

  Serial.print("Actual temp: ");
  Serial.print(temp); 
  Serial.print(" °C");
  Serial.print("Setpoint: ");
  Serial.print(referencia);
  Serial.print(" °C");
  Serial.println("");
}

void DigitalFilter_init(DigitalFilter* f, const int filterOrder, const double *filterTaps) {
  f->history = (double *) malloc(filterOrder*sizeof(double));
  for(int i = 0; i < filterOrder; ++i) f->history[i] = 0;  
  f->taps = filterTaps;
  f->filterOrder = filterOrder;
  f->last_index = 0;
}
  
void DigitalFilter_put(DigitalFilter* f, double input) {
  f->history[f->last_index++] = input;
  if(f->last_index == f->filterOrder) f->last_index = 0;
}
  
double DigitalFilter_get(DigitalFilter* f) {
  double acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < f->filterOrder; ++i) {
    index = index != 0 ? index-1 : f->filterOrder-1;
    acc += f->history[index] * f->taps[i];
  };
  return acc;
}

//----------------------------------------------------------------
// Code
//----------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  DigitalFilter_init(&digitalFilter_1, 10, TAPS);
  
  pinMode(sensorPin, INPUT);
  pinMode(pwmCarga, OUTPUT);
  pinMode(pwmCooler, OUTPUT);
  
  digitalWrite(pwmCarga, HIGH);

  Timer1.initialize(sampleTime);
  Timer1.attachInterrupt(getTemp);
}

void loop () {

}

void serialEvent() {
  while (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    float valor = input.toFloat();
    referencia = valor; // Allows the user to change the setpoint via Arduino Serial Console
  }
}
