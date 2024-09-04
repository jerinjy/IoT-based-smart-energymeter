#include <LiquidCrystal_I2C.h>


//Libraries
#include <TrueRMS.h>
#include <SoftwareSerial.h>
#include <Wire.h>


//Variables
unsigned long crrnTym;
int idc,adc;
int cnt=0,ct=0,cnnt =0;
float Energy=0,Unit,Bill,Pow,Irms,Vrms,Pavg,realPow;


//Objects
Rms readRms; 
Rms rms;
SoftwareSerial mySerial(6,5);
LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {

  //Arduino
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(12,INPUT);
  pinMode(11,OUTPUT);

  //Lcd
  lcd.init();
  lcd.backlight();
  
  //Rms calculation
  readRms.begin(900.73, 40, ADC_10BIT, BLR_ON, CNT_SCAN);
  readRms.start();
  rms.begin(5.00, 40, ADC_10BIT, BLR_ON, CNT_SCAN);
  rms.start();


  crrnTym = micros()+1000;
}


void loop()  {
  
  //Rms loop
  adc = analogRead(A0);
  readRms.update(adc);
  idc = analogRead(A1);
  rms.update(idc);

  cnt++;
  cnnt++;
  ct++;
  if(ct >= 100)
   {
     readRms.publish();
     Vrms = readRms.rmsVal;
     rms.publish();
     Irms = rms.rmsVal;

     //power calc
     Pow = Vrms*Irms;
     Pavg += Pow; 
     ct=0;
   }

     //energy
     if(cnnt >= 1000)
     {
       realPow = Pavg/10.00;
       Energy += realPow;
       Unit = Energy / 3600000; 
       Pavg = 0;
       cnnt=0;
     }
     
    //Bill
     if(cnt >= 5000)   //check UL
    {
      if(digitalRead(12)==HIGH){
        lcd.clear();
        lcd.noBacklight();
        digitalWrite(11,HIGH);
      }
      if( Unit >= 50 )
      { Bill = Unit*3.15;
       if( Unit>50 && Unit<=100 )
        { Bill = Unit*3.95;
        if( Unit>100 && Unit<=150 )
         { Bill = Unit*5;
         if( Unit>150 && Unit<=200)
          {   
            Bill = Unit*6.80; 
          }
         }
        } 
       }
      else 
      { Bill = Unit*7.5; }

      //printing
       Serial.print(">>> V:");
       Serial.println(Vrms);
       Serial.print(">>> I:");
       Serial.println(Irms);
       Serial.print(">>> Pow:");
       Serial.println(realPow);
       Serial.print(">>> Energy :");
       Serial.println(Energy);
       Serial.print(">>> Bill");
       Serial.println(Bill);
       Serial.println("");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("V:");
    lcd.print(Vrms);
    lcd.setCursor(8,0);
    lcd.print("I:");
    lcd.print(Irms);
    lcd.setCursor(0,1);
    lcd.print("Energy:");
    lcd.print(Energy);

       //esp comm
       mySerial.println(Energy);
       
      cnt = 0;
      
    }
    
    while (crrnTym > micros());
    crrnTym += 1000;
    
} 
