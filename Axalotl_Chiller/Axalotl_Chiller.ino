//Goal is to keep water between 16 and 18 *C
//CONFIGURATION  - TWEAK THESE IF NEEDED
float temp_target = 16.00; //Degrees Celsius
float temp_delta = 1.00; //one degree C variation
float temp_alarm_threshold = 4;
//CODE, DONT CHANGE UNLESS YOU KNOW WHY AND HAVE TESTED RESULT BEFORE USING FOR LIVE ANIMALS
#define CHILLER D0
#define PUMP D1
#define BUZZER D2


// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 100000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 100000   
 
 
uint16_t samples[NUMSAMPLES];
 
void setup(void) {
  Serial.begin(9600);
  pinMode(CHILLER, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(BUZZER,OUTPUT);
  //analogReference(EXTERNAL);
}
 
void loop(void) {
  float temp = getTemp();
  if(temp > temp_target){
    Serial.println("Too hot! Turning on Chiller and pump");
    digitalWrite(CHILLER, LOW);
    digitalWrite(PUMP, LOW);
    if(temp >= temp_alarm_threshold + temp_target){
      Serial.print("Red Alert! Temps way too high! Sound the alarm! Current temp: ");
      Serial.print(temp);
      Serial.println(" *C");
      Serial.println("  ");
      alarmHot();
    }
    
  }else{
    if(temp <= temp_alarm_threshold - temp_target){
      Serial.print("Blue Alert! Temps way too Low! Sound the alarm! Current temp: ");
      Serial.print(temp);
      Serial.println(" *C");
      Serial.println("  ");
      alarmCold();
    }    
    Serial.println("Not too hot. Turning off Chiller and pump");
    digitalWrite(CHILLER, HIGH);
    digitalWrite(PUMP, HIGH);
  }
 delay(1000);
}

void alarmHot(){
  for(int i = 500;i <= 800;i++) //frequence loop from 200 to 800
  {
    tone(BUZZER,i); //turn the buzzer on
    delay(5); //wait for 5 milliseconds 
  }
  delay(5000); //wait for 4 seconds on highest frequence
  for(int i = 800;i >= 500;i--)//frequence loop from 800 downto 200
  {
    tone(BUZZER,i);
    delay(5);
  }
}


void alarmCold(){
  for(int i = 200;i <= 800;i++) //frequence loop from 200 to 800
  {
    tone(BUZZER,i); //turn the buzzer on
    delay(2); //wait for 5 milliseconds 
  }
  delay(500); //wait for 4 seconds on highest frequence
  for(int i = 800;i >= 200;i--)//frequence loop from 800 downto 200
  {
    tone(BUZZER,i);
    delay(2);
  }
  delay(5000); //wait for 4 seconds on highest frequence
}
bool isChillerOn(){
  return !digitalRead(CHILLER);
}
float getTemp(){
    uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  Serial.print("Average analog reading "); 
  Serial.println(average);
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
 
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");

  return steinhart;
 }
