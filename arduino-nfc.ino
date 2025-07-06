#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9
#define SS_PIN          10
#define BUZZER_PIN      8

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println(F("=== NFC Diagnostic Tool ==="));
  Serial.println(F("Initializing..."));
  
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println(F("MFRC522 Pins:"));
  Serial.println(F("VCC->3.3V, RST->9, GND->GND"));
  Serial.println(F("MISO->12, MOSI->11, SCK->13, SS->10"));
  Serial.println(F("Buzzer->Pin 8"));
  Serial.println(F("WARNING: Use 3.3V power only"));
  Serial.println();
  
  // Check if MFRC522 is connected
  Serial.println(F("Testing MFRC522 connection..."));
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("Version: 0x"));
  Serial.println(v, HEX);
  
  if (v == 0x91 || v == 0x92) {
    Serial.println(F("MFRC522 detected successfully"));
  } else if (v == 0x00 || v == 0xFF) {
    Serial.println(F("ERROR: MFRC522 not detected"));
    Serial.println(F("Check wiring and power"));
    while(1);
  } else {
    Serial.println(F("WARNING: Unknown version"));
    Serial.println(F("Attempting to continue..."));
  }
  
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println();
  Serial.println(F("Place NFC card near reader..."));
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Card detected beep
  beep(1);
  
  Serial.println(F("=== NFC Card Detected ==="));
  Serial.println();
  
  Serial.print(F("UID: "));
  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  
  detectCardType();
  showCardDetails();
  clearCardData();
  
  // Operation complete beep
  beep(2);
  
  Serial.println(F("=== Operation Complete ==="));
  Serial.println(F("Remove card, then place new card"));
  Serial.println();
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  delay(2000);
}

void detectCardType() {
  Serial.println(F("--- Card Type Detection ---"));
  
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  String typeName = mfrc522.PICC_GetTypeName(piccType);
  
  Serial.print(F("Card Type: "));
  Serial.println(typeName);
  Serial.print(F("SAK: 0x"));
  Serial.println(mfrc522.uid.sak, HEX);
  
  switch (piccType) {
    case MFRC522::PICC_TYPE_MIFARE_MINI:
      Serial.println(F("Capacity: 320 bytes"));
      Serial.println(F("Sectors: 5"));
      Serial.println(F("Blocks: 20"));
      Serial.println(F("NDEF: No"));
      break;
      
    case MFRC522::PICC_TYPE_MIFARE_1K:
      Serial.println(F("Capacity: 1KB"));
      Serial.println(F("Sectors: 16"));
      Serial.println(F("Blocks: 64"));
      Serial.println(F("NDEF: Yes (needs format)"));
      break;
      
    case MFRC522::PICC_TYPE_MIFARE_4K:
      Serial.println(F("Capacity: 4KB"));
      Serial.println(F("Sectors: 40"));
      Serial.println(F("Blocks: 256"));
      Serial.println(F("NDEF: Yes (needs format)"));
      break;
      
    case MFRC522::PICC_TYPE_MIFARE_UL:
      Serial.println(F("Capacity: 64 bytes"));
      Serial.println(F("Pages: 16"));
      Serial.println(F("NDEF: Yes"));
      Serial.println(F("Type: NTAG213/Ultralight"));
      break;
      
    case MFRC522::PICC_TYPE_TNP3XXX:
      Serial.println(F("Type: TNP3xxx compatible"));
      Serial.println(F("NDEF: Possible"));
      break;
      
    default:
      // Check SAK for additional identification
      if (mfrc522.uid.sak == 0x28) {
        Serial.println(F("Likely: MIFARE Classic 4KB"));
        Serial.println(F("Capacity: 4KB"));
        Serial.println(F("Sectors: 40"));
        Serial.println(F("Blocks: 256"));
        Serial.println(F("NDEF: Yes (needs format)"));
      } else {
        Serial.println(F("Unknown"));
        Serial.println(F("NDEF: Unknown"));
      }
      break;
  }
  
  Serial.println();
}

void showCardDetails() {
  Serial.println(F("--- Card Details ---"));
  
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
  Serial.println();
  Serial.println(F("--- Technical Specs ---"));
  Serial.print(F("UID Length: "));
  Serial.print(mfrc522.uid.size);
  Serial.println(F(" bytes"));
  
  Serial.print(F("SAK: 0x"));
  Serial.println(mfrc522.uid.sak, HEX);
  
  if (mfrc522.uid.size == 7) {
    Serial.println(F("Possible NTAG series:"));
    Serial.println(F("- NTAG213: 180 bytes user data"));
    Serial.println(F("- NTAG215: 540 bytes user data"));  
    Serial.println(F("- NTAG216: 928 bytes user data"));
    Serial.println(F("Full NDEF support: Yes"));
  }
  
  Serial.println();
}

void clearCardData() {
  Serial.println(F("--- Clearing Card Data ---"));
  
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  
  if (piccType == MFRC522::PICC_TYPE_MIFARE_1K || 
      piccType == MFRC522::PICC_TYPE_MIFARE_4K ||
      mfrc522.uid.sak == 0x28) {
    clearMifareClassic();
  } else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
    clearMifareUltralight();
  } else {
    Serial.println(F("Warning: Clear function not implemented"));
    Serial.println(F("Please check card specs manually"));
  }
}

void clearMifareClassic() {
  Serial.println(F("Clearing MIFARE Classic..."));
  
  byte defaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = defaultKey[i];
  
  byte sectors = (mfrc522.PICC_GetType(mfrc522.uid.sak) == MFRC522::PICC_TYPE_MIFARE_1K) ? 16 : 40;
  
  for (byte sector = 1; sector < sectors; sector++) {
    for (byte block = 0; block < 3; block++) {
      byte blockAddr = sector * 4 + block;
      
      MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
      
      if (status == MFRC522::STATUS_OK) {
        byte zeroData[16] = {0};
        status = mfrc522.MIFARE_Write(blockAddr, zeroData, 16);
        
        if (status == MFRC522::STATUS_OK) {
          Serial.print(F("Block "));
          Serial.print(blockAddr);
          Serial.println(F(" cleared"));
        } else {
          Serial.print(F("Block "));
          Serial.print(blockAddr);
          Serial.print(F(" failed: "));
          Serial.println(mfrc522.GetStatusCodeName(status));
        }
      }
    }
  }
  
  Serial.println(F("MIFARE Classic clear complete"));
}

void clearMifareUltralight() {
  Serial.println(F("Clearing MIFARE Ultralight/NTAG..."));
  
  byte zeroPage[4] = {0x00, 0x00, 0x00, 0x00};
  
  for (byte page = 4; page <= 39; page++) {
    MFRC522::StatusCode status = mfrc522.MIFARE_Ultralight_Write(page, zeroPage, 4);
    
    if (status == MFRC522::STATUS_OK) {
      Serial.print(F("Page "));
      Serial.print(page);
      Serial.println(F(" cleared"));
    } else {
      Serial.print(F("Page "));
      Serial.print(page);
      Serial.print(F(" failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      
      if (page > 39) break;
    }
  }
  
  Serial.println(F("MIFARE Ultralight/NTAG clear complete"));
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void beep(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < count - 1) {
      delay(200);
    }
  }
}