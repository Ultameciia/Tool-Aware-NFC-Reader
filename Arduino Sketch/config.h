#ifndef CONFIG_H
#define CONFIG_H

// --- WiFi Credentials ---
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

// --- Spoolman Server ---
#define SPOOLMAN_IP "10.0.0.1"
#define SPOOLMAN_PORT 7912

// --- Printer Configuration ---
// Total number of printers defined below
#define NUM_PRINTERS 4

struct PrinterConfig {
  const char* name;
  const char* ip;
  int toolCount;
  bool isToolchanger;
  bool isMMU;
};

// Define your fleet here printer name, ip, number of tools/lanes, isToolchanger, isMMU
const PrinterConfig printerFleet[NUM_PRINTERS] = {
  {"Voron 2.4",  "10.0.0.2",  4, true, false},
  {"Trident",    "10.0.0.3",  2, true, false},
  {"Salad Fork", "10.0.0.4", 1, false, false},
  {"Pandora",    "10.0.0.5", 1, false, false}
};

// --- Hardware Pin Mapping ---
#define SW_PIN D3   // Encoder Button
#define ENC_A  D6   // Encoder Phase A (CLK)
#define ENC_B  D5   // Encoder Phase B (DT)
#define OLED_ADDR 0x3C

#endif
