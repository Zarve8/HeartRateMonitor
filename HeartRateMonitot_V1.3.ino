
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define BUZZER_PIN    13 
#define V_PIN A3
#include <Wire.h>
#include "MAX30105.h"           //MAX3010x library
#include "heartRate.h"          //Heart rate calculating algorithm

MAX30105 particleSensor;

const byte RATE_SIZE = 6; //  Усреднение значений
byte rates[RATE_SIZE]; 
byte rateSpot = 0;
long lastBeat = 0; 
float beatsPerMinute;
int beatAvg;



void setup() {  
   pinMode(BUZZER_PIN, OUTPUT);
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(" PULSOMETER V.1 ");
    lcd.setCursor(0, 1); 
    lcd.print("I am ready");

  // Инициализация сенсора
  particleSensor.begin(Wire, I2C_SPEED_FAST); 
  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); // Включает красный светодиод

// показываем заряд батарейки
   battery_icon();
   tone(BUZZER_PIN, 2000, 500);
   delay(3000);
}

void loop() {
 long irValue = particleSensor.getIR();    
                                           
if(irValue > 7000){                                           
    lcd.clear();                                  
    lcd.setCursor(0, 1);                
    lcd.print("BPM = ");             
    lcd.setCursor(8,1);                
   lcd.print(beatAvg);  
 
  if (checkForBeat(irValue) == true)                       
  {
     tone(BUZZER_PIN, 1000, 100);   // пропищим удар
   
    //We sensed a beat!
    long delta = millis() - lastBeat;                   
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);           

    if (beatsPerMinute < 255 && beatsPerMinute > 20)               
    {for(byte x= 1; x< RATE_SIZE;x++){
      rates[x] = rates[x-1];
    }
      rates[0]= beatsPerMinute;
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++){
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
    }
  }

}
  if (irValue < 7000){       //если палец убран - сообщим и  сбросим средний  BPM в 0 
     beatAvg=0;
     lcd.clear();             
     lcd.setCursor(0,0);                
     lcd.print("Please place "); 
     lcd.setCursor(0,1);
     lcd.print("     your finger ");  
     }
}

void battery_icon()
{
float V = analogRead( V_PIN)/1023.0*5.0/0.82;
lcd.setCursor(15, 1);

if (V >= 5.5)   lcd.print("\x9B"); 
else if (V >=5.0 && V<5.5) lcd.print("\x9C"); 
else if (V >=4.0 && V< 5.0) lcd.print("\x9D"); 
else if (V >=3.5 && V< 4.0) lcd.print("\x9E");
else lcd.print("\x9F");
}
