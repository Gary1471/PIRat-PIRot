#include <ESP8266WiFi.h>
#include <espnow.h>

#define PIR_PIN 4           // GPIO 4 = D2 na desce
#define BUZZER_PIN 14       // GPIO 14 = D5 na desce
#define BUTTON_PIN 5        // GPIO 5 = D1 na desce
#define LED_PIN 0           // GPIO 0 = D3 na desce

volatile bool buttonPressed = false;  // Flag pro ISR
int mode = 0;                         // Režim zařízení: 0 = LED, 1 = LED + BUZZER, 2 = NONE
bool motionDetected = false;          // Stav pohybu

// Struktura pro odesílání dat
typedef struct struct_message {
  int movementDetected;  
} struct_message;

struct_message myData;

// Adresa PIRot (nahraďte skutečnou MAC adresou PIRot)
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x2F, 0x7E, 0xA9};

void ICACHE_RAM_ATTR onButtonPress() {
  buttonPressed = true; // Nastaví flag (bezpečné pro ISR)
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);

  Serial.begin(74880);
  Serial.println("HELLO WORLD - PIRat inicializace");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Chyba ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Předpokládáme, že PIRot je vždy dostupný, připojujeme se k němu.
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Zpracování stisku tlačítka mimo ISR
  if (buttonPressed) {
    mode = (mode + 1) % 3; // Přepnutí režimu (0, 1, 2)
    Serial.print("Režim změněn na: ");
    if (mode == 0) {
      Serial.println("LED režim");
    } else if (mode == 1) {
      Serial.println("LED + BUZZER režim");
    } else {
      Serial.println("NONE režim");
    }

    // Potvrzení bliknutím LED
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    buttonPressed = false;
  }

  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH && !motionDetected) {
    motionDetected = true;
    myData.movementDetected = 1;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.println("Zpráva o pohybu odeslána PIRot");

    // Podle režimu provést akce
    if (mode == 0) {  // LED režim
      Serial.println("LED režim: LED bliká");
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
    } else if (mode == 1) {  // LED + BUZZER režim
      Serial.println("LED + BUZZER režim: LED bliká a buzzer pípá");
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    } else {  // NONE režim
      // V režimu NONE pouze odesíláme zprávu, žádná vizuální nebo zvuková indikace
      Serial.println("NONE režim: Pouze zpráva odeslána.");
    }
  } else if (pirState == LOW && motionDetected) {
    motionDetected = false;
    myData.movementDetected = 0;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    Serial.println("Zpráva o pohybu přestal odeslána PIRot");
  }

  delay(100);  // Krátká pauza pro stabilitu
}
