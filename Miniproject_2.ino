// Simple smart parking (RFID Base)
// miniproject of microcontroller
//นายนนทวัฒน์ สุวรรณไชยรพ รหัส 61332110007-8 ECP4N
//นางสาวอภิญา อ้อยบำรุง รหัส 61332110057-7 ECP4N 

//servo var
#include <Servo.h>
byte gate = 1; //1 on 0 off
//oLED var
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
//ir var
#define IR1_Pin 3
int IRl_val = 0;
//sonic1 var
byte i=0,i2=0;
const int pingPin = 4; //trig
int inPin = 5;
long duration, cm;
//sonic2 var
const int pingPin2 = 6; //trig
int inPin2 = 7;
long duration2, cm2;
//buzzer
//parkcount
// global var
int park1 = 0;
int park2 = 0;
// RFID
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above
#define PIEZO           8

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

#define NOTE_G4  392
#define NOTE_C5  523
#define NOTE_G5  784
#define NOTE_C6  1047

#include <Servo.h>
Servo myservo; //ประกาศตัวแปรแทน Servo
int TrueMelody[] = {NOTE_G5, NOTE_C6};
int TrueNoteDurations[] = {12, 8};

int FalseMelody[] = {NOTE_C6, NOTE_G5};
int FalseNoteDurations[] = {12, 8};

#define playTrueMelody() playMelody(TrueMelody, TrueNoteDurations, 2)
#define playFalseMelody() playMelody(FalseMelody, FalseNoteDurations, 2)

void setup() {
  //servo
  myservo.attach(2);
  //ir
  pinMode(IR1_Pin, INPUT); // sets the pin as input
  
  Serial.begin(9600);   // Initialize serial communications with the PC
  SPI.begin();          // Init SPI bus
  Serial.println("Please put your card on the RFID reader..");
  pinMode(PIEZO, OUTPUT);
  myservo.write(70); // สั่งให้ Servo หมุนไปองศาที่ 0
    //oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c); //initialize I2C addr 0x3c

}

void loop() {
  sonic(); // call park status
  sonic2(); // call park status
//  Serial.print("park1 :");
//  Serial.println(park);
  oledshow(); // Show screen output // refresh screen
  delay(100);
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  mfrc522.PCD_Init();   // Init MFRC522
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String RFIDtag = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    RFIDtag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    RFIDtag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  RFIDtag.toUpperCase();


  if ( RFIDtag.substring(1) == "86 4B 0B A4" ) //Change the 00 00 00 00 to your UID card number that you want to give access

  {
    Serial.println("Access is granted");
    oledshow_AG();
    playTrueMelody();
    myservo.write(0); // สั่งให้ Servo หมุนไปองศาที่ 0
    delay(3000); // หน่วงเวลา 3000ms
    oledshow();
    //gate = 0;
    IR1();
    while (IRl_val == 0)
    {
      Serial.println("not going");
      IR1();
      }
      delay(250);
    myservo.write(70);
   //gate = 1;
  }

  else   {
    Serial.println("Access is denied");
    oledshow_DG();
    playFalseMelody();
    delay(500);
    Serial.println("Please put your card on the RFID reader..");
  }
}
void sonic(){
  
  pinMode(pingPin, OUTPUT);


  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);

  cm = microsecondsToCentimeters(duration);

  //Serial.println(cm);
  if (cm <= 6)
   {
      park1 = 1;
   }
   else if (cm >= 6)
   {
     park1 = 0;
    }
  }

void sonic2(){
  
  pinMode(pingPin2, OUTPUT);


  digitalWrite(pingPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin2, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin2, LOW);
  pinMode(inPin2, INPUT);
  duration2 = pulseIn(inPin2, HIGH);

  cm2 = microsecondsToCentimeters(duration2);

  if (cm2 <= 6)
   {
      park2 = 1;
   }
   else if (cm2 >= 6)
   {
     park2 = 0;
    }
  }
  long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}
void IR1()
{   
  IRl_val = digitalRead(IR1_Pin);
  Serial.print("val = ");
  Serial.println(IRl_val);
  return;
}

void oledshow(){
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Please put RFID card");
  display.print("available park : ");
  display.print(park1+park2);
  display.print("/2");
  display.display();
  }
void oledshow_AG(){
   display.clearDisplay();
     display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Access is granted");
  display.display();
  }
void oledshow_DG(){
   display.clearDisplay();
     display.setTextColor(WHITE, BLACK);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Access is denied");
  display.display();
  }
void playMelody(int *melody, int *noteDurations, int notesLength)
{
  pinMode(PIEZO, OUTPUT);

  for (int thisNote = 0; thisNote < notesLength; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIEZO, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIEZO);
  }
}
