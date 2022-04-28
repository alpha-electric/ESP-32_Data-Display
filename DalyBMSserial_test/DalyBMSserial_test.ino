#define RXD2 16
#define TXD2 17

const int size = 100;
byte send[] = {0xA5, 0x40, 0x95, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2};
byte receive[size];

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial1.write(send, sizeof(send));
  
  int count = 0;
  while (Serial1.available()){
   receive[count++] = Serial1.read();
}
  
  for(int i = 0; i < 10; i++){
      Serial.print(receive[i], HEX);
      Serial.print("\n");
  }
  delay(500);
}
