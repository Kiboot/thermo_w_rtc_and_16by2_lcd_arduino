#include <DS1302.h> //https://github.com/msparks/arduino-ds1302 DS1302 RTC Module
#include <Wire.h> //https://www.arduino.cc/en/reference/wire 
#include <LiquidCrystal_I2C.h> //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library LCD with I2C module
#include "DHT.h" //https://github.com/adafruit/Adafruit_Sensor Master library for sensors, includes DHT module
//Compiled and Programmed by: Kevin Lee D. Cardoza

//5v ->     DS1302 VCC, LCD VCC, DHT VCC
//GND ->    DS1302 GND, LCD GND, DHT GND
//Pin 3 ->  DHT Data
//Pin 8 ->  DS1302 RST
//Pin 9 ->  DS1302 Data
//Pin 10 -> DS1302 CLK
//Pin A4 -> LCD SDA
//Pin A5 -> LCD SCL


#define DHTPIN 3 //Pin used by DHT sensor
#define DHTTYPE DHT11 //Can either be DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE); //DHT object

LiquidCrystal_I2C lcd(0x27, 16, 2);//I2C pin declaration
DS1302 rtc(8, 9, 10);// RTC object

int timing = 0;//used timing variable for timed outputs

String dayAsString(const Time::Day day) {//pulling data from DHT.h referring to day, converting int value(5) to String(Wed)
  switch (day) {
    case Time::kSunday: return "Sun";
    case Time::kMonday: return "Mon";
    case Time::kTuesday: return "Tue";
    case Time::kWednesday: return "Wed";
    case Time::kThursday: return "Thu";
    case Time::kFriday: return "Fri";
    case Time::kSaturday: return "Sat";
  }
  return "(unknown day)";
}
String monAsString(const int mon) {//same function as dayAsString, but for Months. can be used or not
  switch (mon) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
  }}


void printTime() {//Serial display of date. Sample code pulled from DHT.h
  
  Time t = rtc.time();// Get the current time and date from the chip.
  const String day = dayAsString(t.day);// Name the day of the week.
  
  char buf[50];// Format the time and date and insert into the temporary buffer.
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  Serial.println(buf); // Print the formatted string to serial so we can see the time.
}

void setup() 
{
lcd.begin();//Defining 16 columns and 2 rows of lcd display
lcd.backlight();//To Power ON the back light
//lcd.backlight();// To Power OFF the back light

rtc.halt(false);
rtc.writeProtect(false);//disabling write protect to set time

//NOTE: You only need to set values ON THE FIRST RUN. comment it back to avoid overwrite
//rtc.setDAY(TUESDAY);        // Sets the day of the week to Thursday
//rtc.setTime(16, 44, 0);       // Sets the time to 1:32:00 (24hr Format)
//rtc.setDate(9, 4, 2019);    // Sets the date to March 28, 2019
//| You can also use the code below this text
//| to make a new time object to set the date and time.
//| Thursday, January 28, 2019 at 01:39:50 (24hr Format)
//Time t(2019, 4, 11, 20, 41, 00, Time::kThursday);
//rtc.time(t); // The actualy function that sets the time and date on the chip.

  Serial.begin(9600); //initialize serial output
  Serial.println(F("DHTxx test!")); //Test text. can be removed
  dht.begin(); //initializes the DHT module
}


void loop() 
{
  Time tm = rtc.time(); //variable to store time from RTC module (DS1302.h)
  
  String day = dayAsString(tm.day); //variable to store string from dayAsString Function. Ex: Thu
  
  if(timing>30){timing=0;} //Resets the timing counter from 30 to zero
  
  String mon = monAsString(tm.mon);
  float h = dht.readHumidity();
  float t = dht.readTemperature();// Read temperature as Celsius (the default)
  float f = dht.readTemperature(true);// Read temperature as Fahrenheit (isFahrenheit = true)
  
  if (isnan(h) || isnan(t) || isnan(f)) {// Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
  float hif = dht.computeHeatIndex(f, h);// Compute heat index in Fahrenheit (the default)
  float hic = dht.computeHeatIndex(t, h, false);// Compute heat index in Celsius (isFahreheit = false)

  //Serial outputs to check if data matches with LCD outputs
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.print(F("°F "));
  Serial.print(F("Timing: "));
  Serial.println(timing);
  Serial.print(F("Month: "));
  Serial.println(mon.c_str());
  printTime();//Serial outputs end here


//These timings are used for the first column (0,0)
if(timing>15){//from 0-15 seconds it displays humidity
  lcd.setCursor(0,0); //Defining positon to write from first row,first column
  lcd.clear(); //Clean the screen
  lcd.print("Humidity: ");//print is limited to LCD characters, in this case 16.
  lcd.print(h); //prints humidity
  lcd.print("%");
}else{//from 16-30 it displays Temperature
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print(F("Temp: "));
  lcd.print(t); //prints temperature in celsius
  lcd.print(F("C"));
}//Timings for first column end

//These timings are used for the first column (0,1). Note that 8 characters are reserved for the clock (HH:MM:SS)
if(timing>=0 && timing<=10){ //first 10 seconds it prints
  lcd.setCursor(0,1); //points the output to the second row, starting with the first character
  lcd.print(mon.c_str()); //prints the month in text. ex: Mar
  lcd.print(F(" "));
  lcd.print(tm.date); //prints the current day
  lcd.print(F(" "));
  lcd.print(F(" "));
  lcd.print(tm.hr);  //current hour
  lcd.print(F(":"));
  lcd.print(tm.min); //current minute
  lcd.print(F(":"));
  lcd.print(tm.sec); //current second
}
else if(timing>=11 && timing<=20){ //from 11-20 seconds it displays day of the week
  lcd.setCursor(0,1);
  lcd.print(F("Day:"));
  lcd.print(day.c_str()); //prints the current day of the week
  lcd.print(F(" "));
  lcd.print(tm.hr);
  lcd.print(F(":"));
  lcd.print(tm.min);
  lcd.print(F(":"));
  lcd.print(tm.sec);
}else{ //for the remaining 10 seconds it displays the current year
  lcd.setCursor(0,1);
  lcd.print(F("Yr:"));
  lcd.print(tm.yr); //current year
  lcd.print(F(" "));
  lcd.print(tm.hr);
  lcd.print(F(":"));
  lcd.print(tm.min);
  lcd.print(F(":"));
  lcd.print(tm.sec);
}//Timings for second column end


timing++; //increment function to make the timer work
delay(1000); //make sure to keep the value at 1 second (1000ms) to have an accurate display of values
}
