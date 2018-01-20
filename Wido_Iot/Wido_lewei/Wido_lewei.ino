/*************************************************** 
 * This is an example for the DFRobot Wido - Wifi Integrated IoT lite sensor and control node
 * Product Page & More info: http://www.dfrobot.com.cn/goods-997.html
 * Designed specifically to work with the DFRobot Wido products:
 * 
 * The library is forked from Adafruit
 * Modified by Cain
 * Base on the yeelink version written by Lauren 
 * BSD license, all text above must be included in any redistribution
 * 
 ****************************************************/
 
/*
This example code is used to connect the Lewei cloud service (Official homepage: www.lewei50.com).

 The device required is just:
 
 1. any device you used to upload data
 2. And Wido

Note: Please don't forget to change the setting below before using!
 1. WLAN_SSID & WlAN_PASS
 2. userkey
 3. gateway ID & sensor ID

 */


#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#define Wido_IRQ   7
#define Wido_VBAT  5
#define Wido_CS    10

Adafruit_CC3000 Wido = Adafruit_CC3000(Wido_CS, Wido_IRQ, Wido_VBAT,
SPI_CLOCK_DIVIDER); // you can change this clock speed
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2


#define WLAN_SSID       "wifi"           // cannot be longer than 32 characters!
#define WLAN_PASS       "pass"          // For connecting router or AP, don't forget to set the SSID and password here!!


#define TCP_TIMEOUT      10000
#define HTTP_TIMEOUT     2000
#define SENSOR_TIMEOUT   500
#define UPLOAD_TIMEOUT   4000
//#define CC3000_TINY_DRIVER

#define WEBSITE  "www.lewei50.com"
#define USERKEY  "00000000000000000000000000000000"  // Update Your userkey. To get your userkey, please check the link below
                                                    // https://www.lewei50.com/user/clientindex


/*
 * DFRobot 27 sensors in one product
 */


//not use analog&digital
//JoyStick摇杆

//analog sensor
#define GAS 0            //模拟一氧化碳气体传感器
#define STEAM 1          //水分传感器
#define SOUND 2          //模拟声音传感器
#define GRAYSCALE 3      //模拟灰度传感器
#define TEMPERATURE 4    //LM35模拟线性温度传感器
#define AMBIENT_LIGHT 5  //模拟环境光线传感器

//not use analog
//模拟电压检测模块
//模拟压电陶瓷震动传感器
//模拟角度传感器Rotation Sensor V2
//Flame sensor火焰传感器
//MMA7361三轴加速度传感器
//GP2Y0A21 距离传感器
//土壤湿度传感器

//digital led
#define GREEN_LED 0  //数字绿色LED模块
#define WHITE_LED 1  //数字白色LED模块
#define BLUE_LED 2   //数字蓝色LED模块
#define RED_LED 3    //数字红色LED模块

//digital sensor
#define VIBRATION 8  //数字震动传感器
#define TILT 9       //数字钢球倾角传感器

#define MAGNETIC 11  //数字贴片磁感应传感器
#define TOUCH 12     //数字触摸开关Touch
#define INFRARED 13  //人体热释电红外传感器

//not use digital
//红色按键模块
//白色按键模块
//继电器模块

//wido pin count
#define ANALOG_COUNT 6
#define DIGITAL_COUNT 17

//HTTP body buffer lenght
#define BODY_BUF_LEN 450

/* 
 *  External configuration file, 
 *  containing only personal information, 
 *  can be deleted directly
 *  
 *  外部配置文件，仅仅包含个人私有信息，直接删除即可
 */
#include "config.h"

char* nameAnalogPins[] = {
  "GAS"             //0
  ,"STEAM"          //1
  ,"SOUND"          //2
  ,"GRAYSCALE"      //3
  ,"TEMPERATURE"    //4
  ,"AMBIENT_LIGHT"  //5
};
char* nameDigitalPins[] = {

  ""   //0GREEN_LED
  ,""  //1WHITE_LED
  ,""  //2BLUE_LED
  ,""  //3RED_LED
  ,""  //4
  ,""  //5Wido_VBAT
  ,""  //6
  ,""  //7Wido_IRQ
  ,"VIBRATION" //8
  ,"TILT"      //9
  ,""          //10Wido_CS
  ,"MAGNETIC"  //11
  ,"TOUCH"     //12
  ,"INFRARED"  //13
  ,""  //14Wido_SPI_MISO
  ,""  //15Wido_SPI_SCK
  ,""  //16Wido_SPI_MOSI
};
long valueAnalogPins[ANALOG_COUNT] = {};
long valueDigitalPins[DIGITAL_COUNT] = {};
int valueCount;
int indexOutDigitalPins[] = {
  GREEN_LED, WHITE_LED, BLUE_LED, RED_LED
};
int indexInDigitalPins[] = {
  VIBRATION, TILT, MAGNETIC, TOUCH, INFRARED
};
int indexInAnalogPins[] = {
  GAS, STEAM, SOUND, GRAYSCALE, TEMPERATURE, AMBIENT_LIGHT
};
int countOutDigitalPins = sizeof(indexOutDigitalPins) / sizeof(int);
int countInDigitalPins = sizeof(indexInDigitalPins) / sizeof(int);
int countInAnalogPins = sizeof(indexInAnalogPins) / sizeof(int);

void modelDigital(int* digitalPins, int pinCount, int mode){
  Serial.print(F("\tPin Index"));
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    Serial.print(F(" "));
    Serial.print(*digitalPins);
    pinMode(*digitalPins, mode);
    digitalPins++;
  }
  Serial.println(F(""));
}
void readAllDigital(int* digitalPins, int pinCount, long *result){
  Serial.print(F("\tIndex"));
  Serial.print(F("\tnew || "));
  Serial.print(F("\told"));
  Serial.println(F("\tvalue"));
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    int index = *digitalPins;
    int value = digitalRead(index);
    
    Serial.print(F("\t"));
    Serial.print(index);
    Serial.print(F("\t"));
    Serial.print(value);
    Serial.print(F("\t"));
    Serial.print(*result);

    *result = value || *result;
    Serial.print(F("\t"));
    Serial.println(*result);
    
    digitalPins++;
    result++;
  }
}
void readAllAnalog(int* analogPins, int pinCount, long *result){
  Serial.print(F("\tIndex"));
  Serial.print(F("\tvalue"));
  Serial.print(F("\tsum / "));
  Serial.print(F("\tcount"));
  Serial.println(F("\tvalue"));
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    int index = *analogPins;
    int value = analogRead(index);
    int sum = (*result * valueCount);
    sum = sum + value;
    
    Serial.print(F("\t"));
    Serial.print(index);
    Serial.print(F("\t"));
    Serial.print(value);
    Serial.print(F("\t"));
    Serial.print(sum);
    Serial.print(F("\t"));
    Serial.print((valueCount + 1));

    *result = sum / (valueCount + 1);
    Serial.print(F("\t"));
    Serial.println(*result);

    analogPins++;
    result++;
  }
}
void resetAll(int pinCount, long *result){
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    *result = 0;
    result++;
  }
}
void readAllValue(){
  Serial.println(F("Reading all pins value!"));
  Serial.print(F("Digital pins need to read count "));
  Serial.println(countInDigitalPins);
  readAllDigital(indexInDigitalPins,countInDigitalPins,valueDigitalPins);
  Serial.print(F("Analog pins need to read count "));
  Serial.println(countInAnalogPins);
  readAllAnalog(indexInAnalogPins,countInAnalogPins,valueAnalogPins);
  valueCount++;
  Serial.println(F("Read pins value done."));
}
void createJsonMultiple(char* namePins[], int pinCount, long* valuePins, char *httpPackage){
  Serial.print(F("Name Count "));
  Serial.println(pinCount);
  int index = 0;
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    int length = strlen(namePins[thisPin]);
    if(length == 0){
      continue;
    }
    if(index != 0){
      strcat(httpPackage,"\n,");
    }
    strcat(httpPackage,"{\"Name\":\"");
    strcat(httpPackage,namePins[thisPin]);
    strcat(httpPackage,"\",\"Value\":\"");
    itoa(*valuePins,httpPackage+strlen(httpPackage),10);
    strcat(httpPackage,"\"}");

    Serial.print(F("\t"));
    Serial.print(namePins[thisPin]);
    Serial.print(F(" - "));
    Serial.println(*valuePins);
    valuePins++;
    index++;
  }
}
void createJsonComplex(char* namePins[], int pinCount, long* valuePins, char *httpPackage){
  Serial.print(F("Flag Count "));
  Serial.println(pinCount);
  int index = 0;
  int values;
  char names[pinCount] = {};
  
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    int length = strlen(namePins[thisPin]);
    if(length == 0){
      continue;
    }
    char c = namePins[thisPin][0];
    names[index] = c;
    values = values + (*valuePins << index);

    Serial.print(F("\tFlag "));
    Serial.print(index);
    Serial.print(F(" pin "));
    Serial.print(thisPin);
    Serial.print(F(" : "));
    Serial.print(c);
    Serial.print(F(" - "));
    Serial.println(*valuePins);
    
    valuePins++;
    index++;
  }
  
  strcat(httpPackage,"{\"Name\":\"");
  strcat(httpPackage,names);
  strcat(httpPackage,"\",\"Value\":\"");
  itoa(values,httpPackage+strlen(httpPackage),10);
  strcat(httpPackage,"\"}");

}
void createJsonCount(char *httpPackage){
  Serial.print(F("Value Count "));
  Serial.println(valueCount);
  strcat(httpPackage,"{\"Name\":\"COUNT\",\"Value\":\"");
  itoa(valueCount,httpPackage+strlen(httpPackage),10);
  strcat(httpPackage,"\"}");
}
void createBody(char *httpPackage){
  Serial.println(F("Create HTTP Body!"));
  strcat(httpPackage,"[\n");
  //analog
  createJsonMultiple(nameAnalogPins,ANALOG_COUNT,valueAnalogPins,httpPackage);
  resetAll(ANALOG_COUNT,valueAnalogPins);
  
  //lewei免费用户有传感器数量限制，暂时不上传数字量
  //digital
  strcat(httpPackage,"\n,");
  createJsonComplex(nameDigitalPins,DIGITAL_COUNT,valueDigitalPins,httpPackage);
  resetAll(DIGITAL_COUNT,valueDigitalPins);

  //count
  strcat(httpPackage,"\n,");
  createJsonCount(httpPackage);
  valueCount = 0;
  
  strcat(httpPackage,"\n]");
  Serial.println(F("Create HTTP Body done."));
}


void setup(){
  /* Wait for Serial ready */
  delay(5000);
  
  /* Initialise the Serial */
  Serial.begin(115200);
  Serial.println(F("Hello, Wido!\n"));

  /* Initialise the pins */
  Serial.println(F("Initialising the digital Pins Model ..."));
  Serial.print(F("\tOUTPUT pins count "));
  Serial.println(countOutDigitalPins);
  modelDigital(indexOutDigitalPins,countOutDigitalPins,OUTPUT);
  Serial.print(F("\tINPUT pins count "));
  Serial.println(countInDigitalPins);
  modelDigital(indexInDigitalPins,countInDigitalPins,INPUT);

  digitalWrite(WHITE_LED, HIGH);
  /* Initialise the module */
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!Wido.begin())
  {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    digitalWrite(RED_LED, HIGH);
    while(1);
  }
  digitalWrite(RED_LED, LOW);
  digitalWrite(WHITE_LED, LOW);

  digitalWrite(BLUE_LED, HIGH);
  /* Attempt to connect to an access point */
  char *ssid = WLAN_SSID;             /* Max 32 chars */
  Serial.print(F("\nAttempting to connect to ")); 
  Serial.println(ssid);

  /* NOTE: Secure connections are not available in 'Tiny' mode!
   By default connectToAP will retry indefinitely, however you can pass an
   optional maximum number of retries (greater than zero) as the fourth parameter.
   */
  if (!Wido.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    digitalWrite(RED_LED, HIGH);
    while(1);
  }
  Serial.println(F("Connected!"));
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  digitalWrite(WHITE_LED, HIGH);
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!Wido.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
    digitalWrite(RED_LED, HIGH);
  }
  digitalWrite(RED_LED, LOW);
  digitalWrite(WHITE_LED, LOW);

  /* first read value */
  digitalWrite(BLUE_LED, HIGH);
  Serial.println(F(""));
  /* read all pin */
  readAllValue();
  digitalWrite(BLUE_LED, LOW);
  
}

uint32_t ip = 0;
//float humidity = 0;                     //Please change your gateway IDsensor ID
char lengthstr[10] = {};

void loop(){
  
  static Adafruit_CC3000_Client WidoClient;
  static unsigned long RetryMillis = 0;
  static unsigned long uploadtStamp = 0;
  static unsigned long sensortStamp = 0;
//  static 


//  Serial.println(F("LOOP..."));

  /* check last tcp time is expired */
  if(!WidoClient.connected() && millis() - RetryMillis > TCP_TIMEOUT){
    // Update the time stamp
    RetryMillis = millis();

    Serial.println(F("\n\nTry to connect the cloud server"));
    digitalWrite(WHITE_LED, HIGH);
    
    WidoClient.close();

    // Get Lewei IP address
    Serial.print(F("www.lewei50.com -> "));
    while  (ip  ==  0)  {
      if  (!Wido.getHostByName(WEBSITE, &ip))  {    //  Get the server IP address based on the domain name
        Serial.println(F("Couldn't resolve!"));
      }
      delay(500);
      digitalWrite(RED_LED, HIGH);
    }  
    Wido.printIPdotsRev(ip);
    Serial.println(F(""));
    
    // Connect to the Lewei Server
    WidoClient = Wido.connectTCP(ip, 80);          // Try to connect cloud server
    digitalWrite(RED_LED, LOW);
    digitalWrite(WHITE_LED, LOW);
  }

  
  /* check last upload time is expired */
  if(WidoClient.connected() && millis() - uploadtStamp > UPLOAD_TIMEOUT){
    // Update the time stamp
    uploadtStamp = millis();
    // If the device is connected to the cloud server, upload the data every 10s.
    Serial.println(F("\n\nTry to upload the cloud server"));
    digitalWrite(GREEN_LED, HIGH);
    
    // Create Http data package
    char httpPackage[BODY_BUF_LEN] = "";
    int p = &httpPackage;
    Serial.print(F("Offset of Http Body "));
    Serial.println(p);
    createBody(httpPackage);
//    char httpPackage[60] = "";
//    strcat(httpPackage,"[{\"Name\":\"hum\",");
//    strcat(httpPackage,"\"Value\":\"");
//    itoa(humidity,httpPackage+strlen(httpPackage),10);       // push the data(humidity) to the http data package
//    strcat(httpPackage,"\"}]");
//    Serial.print(F("Offset "));
//    Serial.print(&httpPackage);
    Serial.println(F("Body = "));
    Serial.println(httpPackage);
    
    // Prepare Http Package for Lewei & get length
    int length = strlen(httpPackage);                           // get the length of data package
    int p2 = &lengthstr;
    Serial.print(F("Offset of Content-Length "));
    Serial.println(p2);
    itoa(length,lengthstr,10);                             // convert int to char array for posting
    Serial.print(F("Length = "));
    Serial.println(length);
    
    Serial.println(F("Connected to Lewei server."));
    
    // Send headers
    Serial.print(F("Sending headers"));
    
    WidoClient.fastrprint(F("POST /api/V1/Gateway/UpdateSensors/"));
    WidoClient.fastrprint(F("01"));                        //Please change your gateway ID
                                                           //The example URL:http://www.lewei50.com/api/V1/gateway/UpdateSensors/01
    WidoClient.fastrprintln(F(" HTTP/1.1"));
    Serial.print(F("."));
    
    WidoClient.fastrprintln(F("Host: open.lewei50.com"));
    Serial.print(F("."));
    
    WidoClient.fastrprint(F("userkey: "));
    WidoClient.fastrprintln(USERKEY);
    Serial.print(F("."));
    
    WidoClient.fastrprint("Content-Length: "); 
    WidoClient.fastrprintln(lengthstr);
    WidoClient.fastrprintln("");
    Serial.print(F("."));
    
    Serial.println(F(" done."));
    
    // Send data
    Serial.println(F("Sending data"));
    for (int thisPin = 0; thisPin < length; thisPin++) {
      Serial.print(httpPackage[thisPin]);
      WidoClient.write(httpPackage[thisPin]);
    }
    Serial.println(F(""));
//    WidoClient.fastrprintln(httpPackage);

    Serial.println(F(" done."));
    
    /********** Get the http page feedback ***********/
    
    unsigned long rTimer = millis();
    char c = 0;
    Serial.println(F("Reading Cloud Response!!!"));
    while (millis() - rTimer < HTTP_TIMEOUT) {
      digitalWrite(RED_LED, LOW);
      if (WidoClient.connected() && WidoClient.available()) {
        Serial.println(F(""));
        while (WidoClient.connected() && WidoClient.available()) {
          c = WidoClient.read();
          Serial.print(c);
        }
        break;
      }else{
        digitalWrite(RED_LED, HIGH);
//      Serial.print(F("Wait HTTP Time "));
//      Serial.println(millis() - rTimer);
        Serial.print(F("."));
        delay(100);
      }
    }
    if(c == 0){
      Serial.println(F("HTTP Time out!"));
    }else{
      Serial.println(F(""));
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
    }
    
//    delay(1000);             // Wait for 1s to finish posting the data stream
    WidoClient.close();      // Close the service connection
  
    RetryMillis = millis();  // Reset the timer stamp for applying the connection with the service
  
    Serial.println(F("HTTP close."));
  }


//  Serial.print(F("sensortStamp ? "));
//  Serial.print(millis());
//  Serial.print(F(" "));
//  Serial.print(sensortStamp);
//  Serial.print(F(" "));
//  Serial.println(millis() - sensortStamp);
  /* check last sensor time is expired */
  if(millis() - sensortStamp > SENSOR_TIMEOUT){
    // Update the time stamp
    sensortStamp = millis();
    
    Serial.println(F("\n\nTry to update sensor value"));
    digitalWrite(BLUE_LED, HIGH);

    /* read all pin */
    readAllValue();
    
    digitalWrite(BLUE_LED, LOW);
    Serial.println(F(""));
    
//    int reading = analogRead(0);
//    humidity = reading *0.009765625*100;             //You can use formula in here or change it on the web in modify sensor
//    Serial.print(F("Real Time Humidity: ")); 
//    Serial.println(humidity); 
  }
 
  
  Serial.print(F("."));
  
}


