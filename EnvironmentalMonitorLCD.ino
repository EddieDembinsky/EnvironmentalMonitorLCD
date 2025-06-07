#include <LiquidCrystal.h>
#include <DHT.h>

// === LCD Setup ===
// LCD(RS, Enable, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// === DHT11 Setup ===
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === Pin definitions ===
const int lightPin = A1;
const int buttonPin = 8;

// === Variables ===
float temperature = 0.0;
int lightLevel = 0;
int displayMode = 0;
int buttonState = 0;
int lastButtonState = 0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;  // 1 second update

// === Custom Characters ===
byte degreeSymbol[8] = {
  0b01110,
  0b01010,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte sunSymbol[8] = {
  0b00000,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00000,
  0b00000
};

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, degreeSymbol);
  lcd.createChar(1, sunSymbol);
  
  dht.begin();  // Start DHT sensor

  pinMode(buttonPin, INPUT_PULLUP);  // Internal pull-up

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Environmental");
  lcd.setCursor(0, 1);
  lcd.print("Monitor v1.0");
  delay(2000);
  
  Serial.begin(9600);
  Serial.println("Environmental Monitoring Station Started");
}

void loop() {
  if (millis() - lastUpdate >= updateInterval) {
    readSensors();
    lastUpdate = millis();
  }

  // Button handling
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    displayMode = (displayMode + 1) % 3;
    delay(50);  // Debounce
  }
  lastButtonState = buttonState;

  updateDisplay();

  delay(100);
}

void readSensors() {
  temperature = dht.readTemperature();  // Celsius

  if (isnan(temperature)) {
    Serial.println("DHT11 read failed!");
    temperature = -999.0;  // Flag value
  }

  lightLevel = analogRead(lightPin);

  Serial.print("Temp: ");
  if (temperature != -999.0) {
    Serial.print(temperature);
    Serial.print("°C, ");
  } else {
    Serial.print("N/A, ");
  }

  Serial.print("Light: ");
  Serial.println(lightLevel);
}

void updateDisplay() {
  lcd.clear();

  switch (displayMode) {
    case 0:  // Temperature
      lcd.setCursor(0, 0);
      lcd.print("Temperature:");
      lcd.setCursor(0, 1);
      if (temperature != -999.0) {
        lcd.print(temperature, 1);
        lcd.write((byte)0);
        lcd.print("C");

        lcd.setCursor(8, 1);
        if (temperature < 20) {
          lcd.print("COOL");
        } else if (temperature > 25) {
          lcd.print("HOT ");
        } else {
          lcd.print("OK  ");
        }
      } else {
        lcd.print("Sensor Error");
      }
      break;

    case 1:  // Light level
      lcd.setCursor(0, 0);
      lcd.print("Light Level:");

      lcd.setCursor(0, 1);
      lcd.print("                ");  // Clear line 1 fully

      lcd.setCursor(0, 1);
      if (lightLevel < 200) {
        lcd.print("DARK");
      } else if (lightLevel > 800) {
        lcd.print("BRIGHT");
      } else {
        lcd.print("NORMAL");
      }
      break;


    case 2:  // Both
      lcd.setCursor(0, 0);
      lcd.print("T:");
      if (temperature != -999.0) {
        lcd.print(temperature, 1);
        lcd.write((byte)0);
        lcd.print("C");
      } else {
        lcd.print("Err");
      }

      lcd.setCursor(9, 0);
      lcd.print("L:");
      lcd.print(map(lightLevel, 0, 1023, 0, 99));
      lcd.print("%");

      lcd.setCursor(0, 1);
      lcd.print("Press for more");
      break;
  }

  //lcd.setCursor(15, 0);
  //lcd.print(displayMode);  // Show current mode
}

