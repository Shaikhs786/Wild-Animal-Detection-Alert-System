#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ================== Camera Model ===================
#define CAMERA_MODEL_AI_THINKER
//#include "camera_pins.h"

// ================== WiFi & Telegram =================
const char* ssid = "A9564";
const char* password = "sulueditz";

#define BOTtoken "8382060678:AAG4ntZ4ca82sSEkROnd70ZsYT-vrtMPaSQ"
#define CHAT_ID "7369151617ABC"

// ================== PIR and Buzzer Pins =============
#define PIR_PIN 13
#define BUZZER_PIN 12

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ================== Setup ==========================
void startCameraServer();

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x", err);
    return;
  }

  client.setInsecure();
  Serial.println("System ready...");
}

// ================== Main Loop =======================
void loop() {
  int motion = digitalRead(PIR_PIN);
  if (motion == HIGH) {
    Serial.println("⚠️ Motion detected!");

    // Turn ON buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);

    // Capture photo
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Send photo to Telegram
    bot.sendPhotoByBinary(CHAT_ID, "image/jpeg", fb->len,
      fb->buf, fb->len, "Animal detected in farm!");

    esp_camera_fb_return(fb);
    delay(10000);  // Wait 10 sec before next detection
  }
}
