#include <HX711.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// HX711.DOUT	- pin #A1
// HX711.PD_SCK	- pin #A0

HX711 scale(A1, A0);		   // parameter "gain" is ommited; the default value 128 is used by the library
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
const int buttonPin = 9;           // the number of the pushbutton pin

void setup() {
  Serial.begin(38400);
  pinMode(buttonPin, INPUT);  
  
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Hello, I am HX");
  lcd.setCursor(0, 1);
  lcd.print("Arduino scales!");
  
  Serial.println("HX711 Demo");

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided 
						// by the SCALE parameter (not set yet)  

  scale.set_scale(434.f);    //2280.f                  // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
						// by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  if (Serial.available()) {
    String tempCommd = "";
    // wait a bit for the entire message to arrive
    delay(100);
    // read all the available characters
    while (Serial.available() > 0) {
      // save each character from Serial
      tempCommd += char(Serial.read());
      // show on LCD
      lcd.clear();
      lcd.println("User write:" + tempCommd);
    }
    if(tempCommd.startsWith("tare")){
      // reset
      Serial.println("Call Tare to reset");
      lcd.setCursor(0, 1);
      lcd.print("Call Tare.");
      scale.tare();
    }
  }else{
    int buttonState = 0;
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
      Serial.print("button is push");
      lcd.setCursor(0, 1);
      lcd.println("Call Tare.");
      scale.tare();
    }
  }
  
  //Serial.println(scale.read_average(10), 1);
  // use this calculate the SCALE
  Serial.println(scale.read(), 1);
  Serial.println(scale.get_value(10), 1);
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  float gram = scale.get_units(10);
  Serial.println(gram, 1);
  gram = gram * -1;
  lcd.clear();
  lcd.print(gram, 1);
  lcd.print(" g");
  if(gram > 0.5){
    lcd.setCursor(0, 1);
    lcd.print("Dont move...");  
    lcd.backlight();
    scale.power_down();			        // put the ADC in sleep mode
    delay(1000);
    scale.power_up();
  }else{
    lcd.setCursor(0, 1);
    lcd.print("Sleep...");
    scale.power_down();
    lcd.noBacklight();
    delay(500);
    scale.power_up();
  }
}
