// Librarie Umiditate
#include <DHT.h>
#define TEMP_TYPE DHT11


// Pini
int dhtPin = 7;
int increaseButtonPin = 5;
int decreaseButtonPin = 4;
int manualButtonPin = 6;
int ledPowerFanPin = 10;
int ledManualPin = 11;
int fanPin = 9;

// Variabile
bool manual = false;
int powerFanMin = 30;
int powerFanMax = 255;
int stepIncrement = 51;

int powerManualFan = 0;
int powerTempFan = 0;
int powerFan = 0;
int ledPower = 0;

int okManualButton = 0;
int okIncreaseButton = 0;
int okDecreaseButton = 0;

float temp = 0.0;
float tempValue = 25.0;

int stateManualButton = 0;
int stateIncreaseButton = 0;
int stateDecreaseButton = 0;

// Pentru counter
int nr = 0;

DHT dht(dhtPin, TEMP_TYPE);

void setup()
{
  // Debug console
  Serial.begin(9600);
  delay(10);

  dht.begin();
  delay(500);

  pinMode(ledPowerFanPin, OUTPUT);
  pinMode(ledManualPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  pinMode(increaseButtonPin, INPUT);
  pinMode(decreaseButtonPin, INPUT);
  pinMode(manualButtonPin, INPUT);

  digitalWrite(fanPin, LOW);

  // Setari pentru counter intern - Nu modific
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 20;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
}

void loop()
{
  readTemp();
  stateManualButton = digitalRead(manualButtonPin);
  stateIncreaseButton = digitalRead(increaseButtonPin);
  stateDecreaseButton = digitalRead(decreaseButtonPin);

  if (stateManualButton == HIGH) {
    if (okManualButton == 0)
    {
      manual = !manual;
    }
    okManualButton = 1;
  } else {
    okManualButton = 0;
  }

  if (stateIncreaseButton == HIGH) {
    if (okIncreaseButton == 0)
    {
      if (powerManualFan + stepIncrement <= powerFanMax)
      {
        powerManualFan += stepIncrement;
      }
    }
    okIncreaseButton = 1;
  } else {
    okIncreaseButton = 0;
  }

  if (stateDecreaseButton == HIGH) {
    if (okDecreaseButton == 0)
    {
      if (powerManualFan - stepIncrement >= 0)
      {
        powerManualFan -= stepIncrement;
      }
    }
    okDecreaseButton = 1;
  } else {
    okDecreaseButton = 0;
  }


  if (manual == true) {
    digitalWrite(ledManualPin, HIGH);
    powerFan = powerManualFan;
  } else {
    powerFan = powerTempFan;
    powerManualFan = 0;
    digitalWrite(ledManualPin, LOW);
  }

  ledPower = map(powerFan, 0, powerFanMax, 0, 255);
  analogWrite(ledPowerFanPin, ledPower);

  Serial.print("Putere Ventilator : ");
  Serial.println(powerFan);
  delay(200);
}

void readTemp()
{
  temp = dht.readTemperature();

  if (temp > tempValue)
  {
    powerTempFan = map(temp, 15, 35, powerFanMin, powerFanMax);
  } else {
    powerTempFan = 0;
  }

  Serial.print("Temperatura : ");
  Serial.println(temp);
}

// Simulare scriere analog
ISR(TIMER1_COMPA_vect)
{
  if(nr<powerFan)
  {
    digitalWrite(fanPin, HIGH);
  }
  else{
    digitalWrite(fanPin, LOW);
  }
  
  nr++;
  if (nr == 255)
  {
    nr = 0;
  }
}
