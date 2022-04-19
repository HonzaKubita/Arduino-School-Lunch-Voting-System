#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
String MasterTag = "370B8C";
String tagID = "";
String data = "";

int polevkaANO = 0;
int polevkaNE = 0;

int hlavniJidloANO = 0;
int hlavniJidloNE = 0;

int button1Pin = A5;
int button2Pin = A4;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); //Parameters: (rs, enable, d4, d5, d6, d7)


void setup() 
{
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  // Initiating
  SPI.begin(); // SPI bus
  mfrc522.PCD_Init(); // MFRC522
  lcd.begin(16, 2); // LCD screen
  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hodnoceni obedu");
  lcd.setCursor(0, 1);
  lcd.print("Nacitani...");
  delay(5000);
}

void loop() {
  if(getID()){
    Serial.println(tagID);
    if(tagID == MasterTag) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Aktualni data:");
      lcd.setCursor(0, 1);
      lcd.print("Jidlo: ANO/NE");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Obed: ");
      lcd.print(hlavniJidloANO);
      lcd.print("/");
      lcd.print(hlavniJidloNE);
      lcd.setCursor(0, 1);
      lcd.print("Polevka: ");
      lcd.print(polevkaANO);
      lcd.print("/");
      lcd.print(polevkaNE);
      while(!getID()){}
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zaviram...");
      delay(1000);
      
    } else if(data.indexOf(tagID) != -1) { // cip uz byl priplozen
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Uz si hlasoval!");
      Serial.print(data);
      delay(2000);
    } else { // cip nebyl prilozen+
      Serial.println("Hlasovani");
      hlasovani();
      Serial.println("hlasovani konec");
      data = data + " " + tagID;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Hodnoceni obedu");
  lcd.setCursor(0, 1);
  lcd.print("Priloz svuj cip");
}

int hlasovani() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ohodnot:");
  lcd.setCursor(0, 1);
  lcd.print("Polevka");
      
  if(buttons() == "ANO"){
    polevkaANO++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Polevka: ANO");
    delay(1000);
  } else {
    polevkaNE++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Polevka: NE");
    delay(1000);
  }
      
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ohodnot:");
  lcd.setCursor(0, 1);
  lcd.print("Hlavni jidlo");

  if(buttons() == "ANO"){
    hlavniJidloANO++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hlavni jidlo:ANO");
    delay(1000);
  } else {
    hlavniJidloNE++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hlavni jidlo: NE");
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dekujeme za tve");
  lcd.setCursor(0, 1);
  lcd.print("hodnoceni :D");
  delay(2000);
  lcd.clear();
}

String buttons() {
  Serial.print("Vyber...");
  while (analogRead(button1Pin) < 100 && analogRead(button2Pin) < 100) {}
  if (analogRead(button1Pin) > 300) {
    Serial.println("ANO");
    Serial.println(analogRead(button1Pin));
    return "ANO";
  } else if (analogRead(button2Pin) > 300) {
    Serial.println("NE");
    Serial.println(analogRead(button2Pin));
    return "NE";
  } else {
    Serial.println("ERROR unreadable");
    return "ANO";
  }
}

boolean getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return true;
}
