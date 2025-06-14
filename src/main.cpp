#include <esp_now.h>
#include <WiFi.h>

#define NUM_BUTTONS 12
const uint8_t ledPin = 2;

struct ButtonInfo {
  uint8_t pin;
  bool state;
  char name;
  
};

struct ButtonMessage {
  ButtonInfo buttons[NUM_BUTTONS];
};

ButtonMessage msg;

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if (len != sizeof(ButtonMessage)) return;

  memcpy(&msg, incomingData, sizeof(ButtonMessage));

  bool anyPressed = false;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (msg.buttons[i].state) {
      anyPressed = true;
      break;
    }
  }

  digitalWrite(ledPin, anyPressed ? HIGH : LOW);

  Serial.print("Received states: ");
  for (int i = 0; i < NUM_BUTTONS; i++) {
    Serial.print(msg.buttons[i].name);
    Serial.print("=");
    Serial.print(msg.buttons[i].state ? "ON " : "OFF ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true);
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  // Всё обрабатывается в прерывании приёма
}