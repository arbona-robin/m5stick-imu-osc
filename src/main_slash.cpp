#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "config.h"

const float SLASH_THRESHOLD = 400.0; // deg/s
const int SLASH_COOLDOWN_MS = 400;

WiFiUDP udp;
unsigned long lastSlashTime = 0;

void showReady()
{
  StickCP2.Display.fillScreen(BLACK);
  StickCP2.Display.setCursor(30, 50);
  StickCP2.Display.print("Ready");
}

void setup()
{
  auto cfg = M5.config();
  StickCP2.begin(cfg);

  StickCP2.Display.setRotation(1);
  StickCP2.Display.setTextSize(3);
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.fillScreen(BLACK);

  StickCP2.Display.setCursor(0, 0);
  StickCP2.Display.println("WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    StickCP2.Display.print(".");
  }

  showReady();
  udp.begin(OSC_PORT);
}

void loop()
{
  StickCP2.update();

  if (StickCP2.Imu.update())
  {
    auto data = StickCP2.Imu.getImuData();
    float gx = data.gyro.x;
    float gy = data.gyro.y;
    float gz = data.gyro.z;

    float mag = sqrtf(gx * gx + gy * gy + gz * gz);

    if (mag > SLASH_THRESHOLD && (millis() - lastSlashTime > SLASH_COOLDOWN_MS))
    {
      lastSlashTime = millis();
      int dir = (gz > 0) ? 1 : 2;

      OSCMessage msg("/slash");
      msg.add(dir);
      udp.beginPacket(OSC_HOST, OSC_PORT);
      msg.send(udp);
      udp.endPacket();

      // Feedback visuel
      StickCP2.Display.fillScreen(dir == 1 ? RED : BLUE);
      StickCP2.Display.setCursor(30, 50);
      StickCP2.Display.printf("Slash %d", dir);
    }

    // Retour a Ready apres le flash
    if (millis() - lastSlashTime > 300 && millis() - lastSlashTime < 350)
    {
      showReady();
    }
  }

  delay(10);
}
