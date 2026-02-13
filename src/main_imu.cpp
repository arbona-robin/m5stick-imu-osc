#include <Arduino.h>
#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include "config.h"

const int SEND_INTERVAL_MS = 50; // ~20Hz

WiFiUDP udp;

// 240x135 paysage, textSize 2 = 12x16px/char
const int COL1 = 0;
const int COL2 = 124;
const int ROW_H = 32;

void drawValue(int x, int y, float val)
{
  StickCP2.Display.setCursor(x, y);
  StickCP2.Display.printf("% 7.2f", val);
}

void setup()
{
  auto cfg = M5.config();
  StickCP2.begin(cfg);

  StickCP2.Display.setRotation(1);
  StickCP2.Display.setTextSize(2);
  StickCP2.Display.setTextColor(WHITE, BLACK);
  StickCP2.Display.fillScreen(BLACK);

  // WiFi
  StickCP2.Display.setCursor(0, 0);
  StickCP2.Display.println("WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    StickCP2.Display.print(".");
  }

  StickCP2.Display.fillScreen(BLACK);

  // Labels fixes (2 colonnes)
  StickCP2.Display.setCursor(COL1, 0);
  StickCP2.Display.print("Acc");
  StickCP2.Display.setCursor(COL2, 0);
  StickCP2.Display.print("Gyro");

  udp.begin(OSC_PORT);
}

void loop()
{
  StickCP2.update();

  if (StickCP2.Imu.update())
  {
    auto data = StickCP2.Imu.getImuData();

    // OSC: un seul message avec les 6 valeurs
    OSCMessage msg("/imu");
    msg.add(data.accel.x);
    msg.add(data.accel.y);
    msg.add(data.accel.z);
    msg.add(data.gyro.x);
    msg.add(data.gyro.y);
    msg.add(data.gyro.z);
    udp.beginPacket(OSC_HOST, OSC_PORT);
    msg.send(udp);
    udp.endPacket();

    // Affichage 2 colonnes: Acc | Gyro
    drawValue(COL1, ROW_H * 1, data.accel.x);
    drawValue(COL1, ROW_H * 2, data.accel.y);
    drawValue(COL1, ROW_H * 3, data.accel.z);
    drawValue(COL2, ROW_H * 1, data.gyro.x);
    drawValue(COL2, ROW_H * 2, data.gyro.y);
    drawValue(COL2, ROW_H * 3, data.gyro.z);
  }

  delay(SEND_INTERVAL_MS);
}
