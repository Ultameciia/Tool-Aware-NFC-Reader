#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PN532.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Encoder.h>
#include "config.h" // This pulls in all your settings

Encoder myEnc(ENC_A, ENC_B);
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_PN532 nfc(2, 3);

enum MenuState { SELECT_PRINTER, SELECT_TOOL, SCANNING };
MenuState currentState = SELECT_PRINTER;

int selectedPrinter = 0;
int selectedTool = 0;
long oldPosition  = -999;

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  
  if (currentState == SELECT_PRINTER) {
    display.setCursor(0,0);
    display.println("SELECT PRINTER:");
    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
    for(int i=0; i < NUM_PRINTERS; i++) {
      display.setCursor(5, 15 + (i*12));
      if(selectedPrinter == i) display.print("> ");
      else display.print("  ");
      display.println(printerFleet[i].name);
    }
  } 
  else if (currentState == SELECT_TOOL) {
    display.setCursor(0,0);
    display.print("P: "); display.println(printerFleet[selectedPrinter].name);
    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
    
    int toolCount = printerFleet[selectedPrinter].toolCount;
    int totalItems = toolCount + 1; 
    int startItem = (selectedTool >= 3) ? selectedTool - 2 : 0;

    for(int i = startItem; i < totalItems; i++) {
      int yPos = 15 + ((i - startItem) * 12);
      if (yPos > 52) break; 
      display.setCursor(5, yPos);
      if(selectedTool == i) display.print("> ");
      else display.print("  ");

    if (i < toolCount) {
        if (printerFleet[selectedPrinter].isMMU) {
        display.print("Lane "); 
    } else {
        display.print("Tool T");
    }
    display.println(i);
      } else {
        display.println("[BACK TO LIST]");
      }
    }
  }
  else if (currentState == SCANNING) {
    display.setTextSize(2);
    display.setCursor(15, 10); display.println("READY");
    display.setTextSize(1);
    display.setCursor(15, 40);
    display.print("Scanning "); display.println(printerFleet[selectedPrinter].name);
  }
  display.display();
}

void sendToKlipper(String tagID) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClient client;
  HTTPClient http;
  http.setTimeout(2000);

  String k_url;
  if (printerFleet[selectedPrinter].isToolchanger) {
    k_url = "http://" + String(printerFleet[selectedPrinter].ip) + "/printer/gcode/script?script=SAVE_TOOL_SPOOL%20TOOL=" + String(selectedTool) + "%20ID=" + tagID;
  } else {
    k_url = "http://" + String(printerFleet[selectedPrinter].ip) + "/printer/gcode/script?script=SET_ACTIVE_SPOOL%20ID=" + tagID;
  }
  
  if (http.begin(client, k_url)) {
    http.POST("");
    http.end();
  }

  String s_url = "http://" + String(SPOOLMAN_IP) + ":" + String(SPOOLMAN_PORT) + "/api/v1/spool/" + tagID;
  String loc = String(printerFleet[selectedPrinter].name);
  if (printerFleet[selectedPrinter].isToolchanger) {
    loc += " T" + String(selectedTool);
} else if (printerFleet[selectedPrinter].isMMU) {
    loc += " Lane " + String(selectedTool);
}
  
  if (http.begin(client, s_url)) {
    http.addHeader("Content-Type", "application/json");
    http.sendRequest("PATCH", "{\"location\": \"" + loc + "\"}");
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SW_PIN, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  nfc.begin();
  nfc.SAMConfig();
  updateDisplay();
}

void loop() {
  long newPosition = myEnc.read() / 4; 
  if (newPosition != oldPosition) {
    bool up = (newPosition > oldPosition);
    oldPosition = newPosition;
    if (currentState == SELECT_PRINTER) {
      selectedPrinter = (up) ? (selectedPrinter + 1) % NUM_PRINTERS : (selectedPrinter - 1 + NUM_PRINTERS) % NUM_PRINTERS;
    } else if (currentState == SELECT_TOOL) {
      int limit = printerFleet[selectedPrinter].toolCount + 1;
      selectedTool = (up) ? (selectedTool + 1) % limit : (selectedTool - 1 + limit) % limit;
    }
    updateDisplay();
  }

  if (digitalRead(SW_PIN) == LOW) {
    delay(50);
    while(digitalRead(SW_PIN) == LOW) yield(); 
    if (currentState == SELECT_PRINTER) {
      currentState = SELECT_TOOL;
      selectedTool = 0;
    } else if (currentState == SELECT_TOOL) {
      if (selectedTool == printerFleet[selectedPrinter].toolCount) currentState = SELECT_PRINTER;
      else currentState = SCANNING;
    } else if (currentState == SCANNING) {
      currentState = SELECT_TOOL;
    }
    updateDisplay();
  }

  if (currentState == SCANNING) {
    uint8_t uid[7]; uint8_t uidLen;
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 500)) {
      String id = ""; uint8_t data[4];
      for (uint8_t p = 5; p <= 8; p++) {
        if (nfc.mifareultralight_ReadPage(p, data)) {
          for (int i=0; i<4; i++) if(isDigit((char)data[i])) id += (char)data[i];
        }
      }
      if (id.length() > 0) {
        sendToKlipper(id);
        display.clearDisplay();
        display.setCursor(20,25); display.setTextSize(2); display.println("SAVED!");
        display.display(); delay(1500);
        currentState = SELECT_PRINTER;
        updateDisplay();
      }
    }
  }
}