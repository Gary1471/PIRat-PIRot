#include <ESP8266WiFi.h>
#include <espnow.h>

#define LED_PIN 0           // GPIO 0 = D3 na desce
#define BUZZER_PIN 14       // GPIO 14 = D5 na desce
#define BUTTON_PIN 5        // GPIO 5 = D1 na desce

volatile bool buttonPressed = false;  // Flag pro ISR
bool normalMode = false;              // Vychozi rezim je "skryty"
bool motionDetected = false;          // Pohyb detekovan flag

// prijimani a odeseilani zprav
typedef struct struct_message {
  int movementDetected;  // 1 = pohyb, 0 = žádný pohyb
} struct_message;

struct_message incomingData; // Přijatá data

// ISR pro button
void ICACHE_RAM_ATTR onButtonPress() {
  buttonPressed = true; // Nastaví flag
}

// Callback (prijimani zprav)
void onDataReceive(uint8_t *macAddr, uint8_t *data, uint8_t len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  motionDetected = (incomingData.movementDetected == 1);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);

  Serial.begin(74880);
  Serial.println("HELLO WORLD - PIRot se inicializuje");

  // init Wi-Fi
  WiFi.mode(WIFI_STA);

  // init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Chyba při inicializaci ESP-NOW");
    return;
  }

  // role a callback
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceive);

  Serial.println("PIRot je připraven");
}

void loop() {
  // stisk buttonu mimo ISR
  if (buttonPressed) {
    normalMode = !normalMode;
    Serial.print("Režim změněn na: ");
    Serial.println(normalMode ? "Normální" : "Skrytý");

    // bliknuti LED
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);

    buttonPressed = false;
  }

  // detekce pohybu
  if (motionDetected) {
    Serial.println("Detekován pohyb!");

    if (normalMode) {
      Serial.println("Normální režim: LED a bzučák");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    } else {
      Serial.println("Skrytý režim: LED");
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    }

    motionDetected = false; // Reset flagu
  }

  delay(100); // stabilizacni pauza xd
}

