#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

//  請填入你的 WiFi 資訊
#define WIFI_SSID     "iPhone kc"
#define WIFI_PASSWORD "415621612"

//  請填入 GitHub Pages 上的 .bin 檔連結（用 HTTPS）
#define FIRMWARE_URL  "https://github.com/yanqun0911/arduino/blob/main/OTAWebUpdater.ino.bin"

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  performOTA();
}

void loop() {
  // 可依條件再次呼叫 performOTA()
  delay(10000);  // 每 10 秒空轉一次
}

void performOTA() {
  Serial.println("Starting OTA update...");

  WiFiClientSecure client;
  client.setInsecure();  //  不驗證憑證（適用 GitHub Pages）

  HTTPClient http;
  http.begin(client, FIRMWARE_URL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (!Update.begin(contentLength)) {
      Serial.println("Not enough space to begin OTA");
      return;
    }

    Serial.println("Writing firmware...");
    size_t written = Update.writeStream(*stream);

    if (written == contentLength) {
      Serial.println("Firmware written successfully");
    } else {
      Serial.printf("Incomplete write: %d/%d\n", written, contentLength);
    }

    if (Update.end()) {
      if (Update.isFinished()) {
        Serial.println("OTA update completed. Rebooting...");
        delay(1000);
        ESP.restart();
      } else {
        Serial.println("Update not finished properly");
      }
    } else {
      Serial.printf("Update failed: %s\n", Update.errorString());
    }

  } else {
    Serial.printf("HTTP GET failed. Code: %d\n", httpCode);
  }

  http.end();
}
