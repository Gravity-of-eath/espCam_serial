/*
  This is a simple MJPEG streaming webserver implemented for AI-Thinker ESP32-CAM and
  ESP32-EYE modules.
  This is tested to work with VLC and Blynk video widget.
  Inspired by and based on this Instructable: $9 RTSP Video Streamer Using the ESP32-CAM Board
  (https://www.instructables.com/id/9-RTSP-Video-Streamer-Using-the-ESP32-CAM-Board/)
  Board: AI-Thinker ESP32-CAM
*/

#include "OV2640.h"
// #include <WiFi.h>
// #include <WebServer.h>
// #include <WiFiClient.h>

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
// #define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

/*
Next one is an include with wifi credentials.
This is what you need to do:
1. Create a file called "home_wifi_multi.h" in the same folder   OR   under a separate subfolder of the "libraries" folder of Arduino IDE. (You are creating a "fake" library really - I called it "MySettings").
2. Place the following text in the file:
#define SSID1 "replace with your wifi ssid"
#define PWD1 "replace your wifi password"
3. Save.
Should work then
*/
// #include "home_wifi_multi.h"
const char BOUNDARY[] = "-1234567890987654321-";
const int bdrLen = strlen(BOUNDARY);
OV2640 cam;
bool flag = false;

void handle_jpg_stream(void)
{

  cam.run();
  Serial1.write(BOUNDARY, bdrLen);
  int s = cam.getSize();
  byte *len = new byte[4]{1, 2, 3, 4};
  len[3] = (byte)(s);
  len[2] = (byte)(s >> 8);
  len[1] = (byte)(s >> 16);
  len[0] = (byte)(s >> 24);
  // Serial.write(0x04);
  Serial1.write(len, sizeof(len));
  Serial1.write((char *)cam.getfb(), s);
  // Serial.write("Serial1 write data len=" + s + 3);
}

void setup()
{

  Serial.begin(115200);
  Serial.write("app-runing");
  Serial1.begin(115200, SERIAL_8N1, 14, 15); // data serial
  while (!Serial1)
    ; // wait for serial connection.

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
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame parameters
  //  config.frame_size = FRAMESIZE_UXGA;
  config.frame_size = FRAMESIZE_HD;
  config.jpeg_quality = 12;
  config.fb_count = 2;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  cam.init(config);
}

void loop()
{
  if (Serial1.available())
  {
    int a = Serial1.read();
    if (a == 1)
    {
      flag = true;

      Serial.write(BOUNDARY, bdrLen);
      // Serial1.write(BOUNDARY, bdrLen);
    }
    else if (a == 2)
    {
      flag = false;
    }
    char buf[32];
    sprintf(buf, "-----%d----\r\n", flag);
    Serial.write(buf);
    Serial.write("\r\n");
  }
  if (flag)
  {
    Serial.write("- ");
    handle_jpg_stream();
  }
}