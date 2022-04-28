//daly bms setup
#include <daly-bms-uart.h>
#include <HardwareSerial.h>
HardwareSerial DalySerial(2);
Daly_BMS_UART bms(DalySerial);
//Pin 16 17 defined in DalySerial(2) library
//#define RXD2 16
//#define TXD2 17

//Display Setup
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Thermistors setup
#define ThermistorPin 26


void setup() {
  // This is needed to print stuff to the serial monitor
  Serial.begin(115200);
  
  // Constructing the bms driver and passing in the Serial interface (which pins to use)
  // This call sets up the driver
  bms.Init();

  //setup display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
}

void loop() {

  // Set up some variables to hold the values
  float volts = 0;
  float amps = 0;
  float percentage = 0;
  

  // Grab those values from the BMS
  bms.getPackMeasurements(volts, amps, percentage);

  // And print them out!
  Serial.printf("V: %4.1f, I: %4.1f, \%:%4.1f\n",volts, amps, percentage);

  // Now the same thing, but for temperature
  //daly bms temp probe
  int8_t temp = 0;
  bms.getPackTemp(temp);

  //esp32 temp probe 1
  int Vo;
  float R1 = 10000; // value of R1 on board
  float logR2, R2;
  int8_t temp2 = 0;
  //steinhart-hart coeficients for thermistor
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; 
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (4095 / (float)Vo - 1.0); //calculate resistance on thermistor
  logR2 = log(R2);
  temp2 = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2)); // temperature in Kelvin
  temp2 = temp2 - 273.15; //convert Kelvin to Celcius
  
  Serial.printf("Temp: %d , Temp2: %d\n",temp, temp2); 
  
  // And again, for min/max cell voltages
  float maxCellVoltage = 0;
  float minCellVoltage = 0;
  uint8_t maxCellNumber = 0;
  uint8_t minCellNumber = 0;
  bms.getMinMaxCellVoltage(minCellVoltage, minCellNumber, maxCellVoltage, maxCellNumber);
  Serial.printf("Highest Cell Voltage: Cell #%d with voltage %4.2f\n Lowest Cell Voltage: Cell #%d with voltage %4.2f\n"
  ,maxCellNumber,maxCellVoltage,minCellNumber,minCellVoltage);

  // Print on display
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(WHITE);        
  display.setCursor(0,0);             
  display.printf("Volt:%4.1f Current:%4.1f Percent:%4.1f\n T1:%d\n T2:%d\n Vhigh:Cell #%d %4.2f v\n Vlow:Cell #%d %4.2f v\n" 
  ,volts, amps, percentage, temp, temp2, maxCellNumber, maxCellVoltage, minCellNumber, minCellVoltage);
  display.display();
  
  delay(500);
  
}
