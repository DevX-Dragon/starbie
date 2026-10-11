#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// --- OLED Setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- MPU6050 Setup ---
Adafruit_MPU6050 mpu;

// --- DHT11 Setup ---
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Button Pins ---
const int PIN_SW1 = 2;
const int PIN_SW2 = 3;
const int PIN_SW3 = 9;
const int PIN_SW4 = 10;

// --- LED Pin ---
const int PIN_LED = 8;

unsigned long previousMillis = 0;
const long interval = 2000; // Update sensor readings every 2 seconds

float temperature = 0.0;
float humidity = 0.0;
sensors_event_t a, g, temp;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Initialize Pins
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  pinMode(PIN_SW3, INPUT_PULLUP);
  pinMode(PIN_SW4, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Initialize I2C (SDA = GPIO6, SCL = GPIO7)
  Wire.begin(6, 7);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Startbie Initializing..."));
  display.display();
  delay(1000);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println(F("Failed to find MPU6050 chip"));
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("MPU6050 Error!"));
    display.display();
    while (1) {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize DHT11
  dht.begin();
}

void loop() {
  // Read Buttons (Active LOW)
  bool sw1State = (digitalRead(PIN_SW1) == LOW);
  bool sw2State = (digitalRead(PIN_SW2) == LOW);
  bool sw3State = (digitalRead(PIN_SW3) == LOW);
  bool sw4State = (digitalRead(PIN_SW4) == LOW);

  // Toggle LED if SW1 is pressed as a simple test
  digitalWrite(PIN_LED, sw1State ? HIGH : LOW);

  // Periodically read sensors
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read DHT11
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    // Read MPU6050
    mpu.getEvent(&a, &g, &temp);
  }

  // Update Display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("=== STARTBIE ==="));

  // Display DHT11 data
  display.setCursor(0, 12);
  if (isnan(humidity) || isnan(temperature)) {
    display.print(F("DHT Error!"));
  } else {
    display.print(F("Temp: "));
    display.print(temperature, 1);
    display.print(F("C"));
    
    display.setCursor(0, 22);
    display.print(F("Hum:  "));
    display.print(humidity, 1);
    display.print(F("%"));
  }

  // Display MPU6050 Accel Data
  display.setCursor(0, 34);
  display.print(F("Acc X:"));
  display.print(a.acceleration.x, 1);

  // Display Button States
  display.setCursor(0, 48);
  display.print(F("BTN:"));
  display.print(sw1State ? "1 " : "- ");
  display.print(sw2State ? "2 " : "- ");
  display.print(sw3State ? "3 " : "- ");
  display.print(sw4State ? "4" : "-");

  display.display();
  delay(50);
}