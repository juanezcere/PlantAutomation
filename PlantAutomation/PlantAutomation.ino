#include <Wire.h>
#include <U8g2lib.h>
#include <SparkFunBME280.h>

#define CLOCK       400000
#define LED         9
#define MOTOR       3
#define SENSE       A2

boolean control = false;
uint8_t page = 0;
uint32_t secs = 0;
float soil = 0.0F, temperature = 0.0F, humidity = 0.0F, pressure = 0.0F;
BME280 sensor;
U8G2_SSD1306_128X64_NONAME_1_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);

void read(void) {
  temperature = 0.0;
  for (uint8_t i = 0; i < 5; i++) {
    temperature += (sensor.readTempC() / 5.0);
    delay(10);
  }
  humidity = 0.0;
  for (uint8_t i = 0; i < 5; i++) {
    humidity += (sensor.readFloatHumidity() / 5.0);
    delay(10);
  }
  pressure = 0.0;
  for (uint8_t i = 0; i < 5; i++) {
    pressure += (sensor.readFloatPressure() / 5.0);
    delay(10);
  }
  pressure /= 1000;
  soil = 0.0;
  for (uint8_t i = 0; i < 5; i++) {
    soil += (analogRead(SENSE) / 5.0);
    delay(10);
  }
  soil = map(soil, 550, 0, 0, 100);
}

void draw(void) {
  screen.firstPage();
  do {
    if (page == 0) {
      char msg[7];
      screen.setFont(u8g2_font_fub17_tf);
      screen.setCursor(0, 22);
      screen.print(F("     Temp"));
      screen.setCursor(110, 63);
      screen.print(F("C"));
      screen.drawCircle(100, 40, 3, U8G2_DRAW_ALL);
      dtostrf(temperature, 3, 1, msg);
      screen.setFont(u8g2_font_fur30_tn);
      screen.setCursor(3, 63);
      screen.print(msg);
    } else if (page == 1) {
      char msg[7];
      screen.setFont(u8g2_font_fub17_tf);
      screen.setCursor(0, 22);
      screen.print(F("  Humidity"));
      screen.setCursor(100, 63);
      screen.print(F("%"));
      dtostrf(humidity, 3, 1, msg);
      screen.setFont(u8g2_font_fur30_tn);
      screen.setCursor(3, 63);
      screen.print(msg);
    } else if (page == 2) {
      char msg[7];
      screen.setFont(u8g2_font_fub17_tf);
      screen.setCursor(0, 22);
      screen.print(F("  Pressure"));
      screen.setCursor(85, 63);
      screen.print(F("kPa"));
      dtostrf(pressure, 3, 1, msg);
      screen.setFont(u8g2_font_fur30_tn);
      screen.setCursor(3, 63);
      screen.print(msg);
    } else if (page == 3) {
      char msg[7];
      screen.setFont(u8g2_font_fub17_tf);
      screen.setCursor(0, 22);
      screen.print(F("     Soil"));
      screen.setCursor(100, 63);
      screen.print(F("%"));
      dtostrf(soil, 3, 1, msg);
      screen.setFont(u8g2_font_fur30_tn);
      screen.setCursor(3, 63);
      screen.print(msg);
    }
  } while (screen.nextPage());
  page++;
  if (page >= 4) {
    page = 0;
  }
}

void setup(void) {
  pinMode(LED, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  analogReference(DEFAULT);
  Wire.begin();
  Wire.setClock(CLOCK);
  sensor.settings.commInterface = I2C_MODE;
  sensor.settings.I2CAddress = 0x76;
  sensor.settings.runMode = 1;
  sensor.settings.tStandby = 5;
  sensor.settings.filter = 0;
  sensor.settings.tempOverSample = 1;
  sensor.settings.humidOverSample = 1;
  sensor.settings.pressOverSample = 1;
  sensor.begin();
  sensor.beginI2C();
  screen.begin();
}

void loop(void) {
  read();
  digitalWrite(MOTOR, control ? HIGH : LOW);
  draw();
  delay(1000);
}
