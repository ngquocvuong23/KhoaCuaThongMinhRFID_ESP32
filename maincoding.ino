// ------------------------------ Declare Library --------------------------------------
// #include <FirebaseJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
// #include <Firebase_ESP_Client.h>
#include <ESP_Google_Sheet_Client.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ESP32Servo.h>
#include <MFRC522.h>
#include "Adafruit_Keypad.h"
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include <HTTPClient.h>

// ------------------------------ Cấu hình phần cứng ------------------------------
#define RST_PIN 0
#define SS_PIN 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_ADDRESS 0x3C

#define PASSWORD "1234"
#define SERVO_PIN 15

// ------------------------------ RFID, OLED, Servo ------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;

// ------------------------------ Declare Keypad --------------------------------------
#define ROWS 4
#define COLS 4
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
uint8_t rowPins[ROWS] = { 13, 12, 14, 27 };  // Thay đổi tùy mạch
uint8_t colPins[COLS] = { 26, 25, 33, 32 };  // Thay đổi tùy mạch
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------------------ WiFi & Server ------------------------------
#define WIFI_SSID "Bao Ngoc"
#define WIFI_PASSWORD "0856130773"
// AsyncWebServer server(80);

// ------------------------------ FIREBASE Config ----------------------------------------
#define API_KEY "AIzaSyDaH-5OEaMQZpuNpUpWu58LyYfzrJ6b-PE"
#define DATABASE_URL "https://iot-esp32-smartlock-default-rtdb.firebaseio.com/"  // nhớ có dấu "/" cuối

#define USER_EMAIL "nguyenquocvuong160705@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ===============================================================
//                      Google Sheet Config
// ===============================================================
// Google Service Account Info
#define PROJECT_ID "esp32-460507"
#define CLIENT_EMAIL "esp32-logging@esp32-460507.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDSj/GGYOeStZXt\nzxwA9oyPL7UB4b75EHxJwqK8EtxWolirPuekXHdu9gLY68KyQoNFuT/vor3ERDM+\nifak8dzbsorMHK86tLasHq3XQFusCvXFxUohFUZxf+LSiCr5Y/3EI+eF1JBpf0ci\nXTiZAoGGFMS23Bw1oZKzFFscKlSB9nGWXf+On07G559lLBQK5hiRzM2W2S2qmNSV\n2P+tml5iredSToPWt79QblVcF+1XS/eSxLx/Na3EGl6Kc+as0pm1HfXftEaedGxE\ntKF169Fo3f0c8/5VoeMIXkQwcVogQR6DK5sSrAi3cy14eJfGi5CKU6IdarLVa57i\nElYJn6q3AgMBAAECggEAX5GQtIawJJEBez3Y/oDo3CZxl+iPxrjaSJosRYOj+ThX\nWWZEOf/z/2tucGlVZZY1wcQqhkgNrJKt1ZFF3qieLyMyxyCv0cvq62VDTuL5mLUi\nxwBcPHbNEb/TQe4m3ymoqMkJEN+hKhjoPusjwKL4CdTMUhjGPvtWO6yeC8MUtyVC\nBDnRSftvfnLWM5FhnNNXsv+islhF2l4okteTEFMRhdGObB8d1SOIG8Px7LS6rTY2\now4qVqdvekgzt4kD8kBr7v7SA54V0iwiD0nKKO9XZwI1KTrCRKTb3NTqXo1884q6\nyIkXwST4rnOhCapOr6Vi7DL8z38K+h9WlQM1GpSasQKBgQD765Pa73nHx3BexF45\nfgcOkT9wf52FOziTN9pEqT95qlyZqskBIvR5Mo1DdtwLikNBvYqOOV5uyPmTQNj2\nbI+H2rUtNx8kyp1qm6WQU0xVAA7zy/GySsb6rP3c4FE2l7t+N5iLrP2jOf41C9B8\nvbA+HoMLPy4oM5rXA7rq+WBwMQKBgQDV+Ob4Sa3qpKYapi/JvYTckPshrmvC63zs\nb3pA/0R/KoXpbpX/FDGYs1zAAlOKuPGGrkW5B7fFgCRTtpb0MvQTJ0dr1vLIWFol\n4GYhEEu3r9g0elDY34Z/Hwj8uUQjnEjucV2wkO9NfNWnd2Gp+TSZagVqPFo4gkmt\nNRUTQTg3ZwKBgQCIylhUr2hPBIqNys7nJPmr/U6XRZRA/fEwb0mDsusWo1z4y/Wt\nOk/JZ5uxIF9HnOH8VnFf3OLcASETxkiJNlWRjT6HuMCed66WbsGt5q0yEoAL/ZfW\nGiiFgV2vi9ZNcIKMfJ/GaD6pXRodr7abpVyHvoWlm77ycWMIkavzX0nwYQKBgQCF\njRJUfGBh6RyH25xONc0cgh1M+3nlMmhJhpbuxKUzHg5xEYXrrOX74k+MxKbyGdce\ncd2wzGFmk0GZQojbqIXxAn4wj/v/Ei5KH9J+5pXr84Rl1MZrZrCi3XMVkmk2NNDM\nitOoZf4HBliwavH5TkMHUkZI9Zx2UtO7mTsrRnTZFwKBgFukAhERAxoc7ZUkuA3q\nrtTFkn98WqcoDksKpY8jPwh+4kS7pHBDWP2XQqMOetFP3bE1wokmjxKqIF51djzP\nq9xWwfqNpZQ5ox4vVCfKASwLKn7fXhPjcKRKO2xFEGEIyMKTJkiPftSChtF9odz9\n+8qS9U0csjcgw03iwNgxLSaB\n-----END PRIVATE KEY-----\n";

// The ID of the spreadsheet where you'll publish the data
const char spreadsheetId[] = "1E5hCEh8iRhB2lgxtztRTCzj84qOX9ZkBnxMbtFCcZlc";

// NTP để lấy thời gian thực
const char* ntpServer = "pool.ntp.org";

// Biến thời gian
unsigned long epochTime;

// =================================
// Hàm lấy thời gian epoch hiện tại
// =================================
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("⚠️ Failed to obtain time");
    return 0;
  }
  time(&now);
  return now;
}

// ==================================
// Callback theo dõi trạng thái token
// ==================================
void tokenStatusCallback(TokenInfo info) {
  if (info.status == token_status_error) {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  } else {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}

// ------------------------------ Biến toàn cục ------------------------------
String inputPassword = "";

volatile bool flagUnlock = false;

unsigned long lastCheck = 0;
const unsigned long checkInterval = 1000;  // 1 giây

int failedKeypad = 0;
int failedRFID = 0;
int failedApp = 0;
const int MAX_FAILED_ATTEMPTS = 3;

const String authorizedUIDs[] = {
  "A1B2C3D4",
  "12345678",
  "ABCDEF12",
  "7bd3d0"
};
const int numAuthorizedUIDs = sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]);

// ------------------------------ Hàm tiện ích ------------------------------
void showMessage(String status, String content) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("STATUS: " + status);
  if (content != "") display.println(content);
  display.display();
}

void unlockDoor() {
  myServo.write(180);
}

void lockDoor() {
  myServo.write(0);
}

bool isRFIDValid(const String& uid) {
  for (int i = 0; i < numAuthorizedUIDs; i++) {
    if (uid == authorizedUIDs[i]) {
      return true;
    }
  }
  return false;
}

// --------------------------------------------- Kết nối WiFi --------------------------------------
void connectToWiFi() {
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
}

// ------------------------------ Setup ------------------------------
void setup() {
  Serial.begin(115200);
  // ------------------------------- Kết nối WiFi, Email, Google Sheet -----------------------------
  connectToWiFi();


  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Email/password auth
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  //Configure time
  // Thiết lập múi giờ Việt Nam (UTC+7)
  configTime(25200, 0, ntpServer);

  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

  // Thiết lập Token
  GSheet.setTokenCallback(tokenStatusCallback);
  GSheet.setPrerefreshSeconds(10 * 60);
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  // ------------------------------ Setup OLED --------------------------------------
  display.begin(I2C_ADDRESS, true);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  showMessage("LOCKED", "INPUT:");

  // ------------------------------ Setup Keypad --------------------------------------
  customKeypad.begin();

  // ------------------------------ Setup Servo --------------------------------------
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN);
  myServo.write(90);
  delay(500);
  myServo.write(180);
  delay(500);
  myServo.write(0);

  // ------------------------------ Setup RFID --------------------------------------
  SPI.begin();
  mfrc522.PCD_Init();
  delay(1000);

  // ------------------------------ Setup Route "/display" ----------------------------
  // server.on("/display", HTTP_GET, [](AsyncWebServerRequest* request) {
  //   showMessage("OPEN", "Mo khoa thanh cong");
  //   flagUnlock = true;
  //   request->send(200, "text/plain", "UNLOCKED SUCCESS!");
  // });

  // server.begin();

  Serial.println("Setup complete. Waiting for unlock requests...");
}

// --- Xử lý từng phương thức mở khóa ---
void onKeypadUnlockFailed() {
  failedKeypad++;
  Serial.printf("Keypad sai: %d lần\n", failedKeypad);
  if (failedKeypad >= MAX_FAILED_ATTEMPTS) {
    writeLogToGoogleSheet("Keypad", "ALERT");
    failedKeypad = 0;
  }
}

void onRFIDUnlockFailed() {
  failedRFID++;
  Serial.printf("RFID sai: %d lần\n", failedRFID);
  if (failedRFID >= MAX_FAILED_ATTEMPTS) {
    writeLogToGoogleSheet("RFID", "ALERT");
    failedRFID = 0;
  }
}

void onAppUnlockFailed() {
  failedApp++;
  Serial.printf("App sai: %d lần\n", failedApp);
  if (failedApp >= MAX_FAILED_ATTEMPTS) {
    writeLogToGoogleSheet("App", "ALERT");
    failedApp = 0;
  }
}

// ==============================
// Hàm ghi log vào Google Sheets
// ==============================
void writeLogToGoogleSheet(const String& method, const String& status) {
  FirebaseJson valueRange, response;

  // Lấy thời gian epoch
  unsigned long epochTime = getTime();

  // Chuyển epoch sang dạng Date và Time
  time_t rawTime = epochTime;
  struct tm* timeInfo = localtime(&rawTime);

  char dateStr[11];  // yyyy-mm-dd
  char timeStr[9];   // hh:mm:ss

  snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d",
           timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
           timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

  valueRange.add("majorDimension", "COLUMNS");
  valueRange.set("values/[0]/[0]", dateStr);  // Cột A - Date
  valueRange.set("values/[1]/[0]", timeStr);  // Cột B - Time
  valueRange.set("values/[2]/[0]", method);   // Cột C - Method
  valueRange.set("values/[3]/[0]", status);   // Cột D - Status

  // Ghi vào sheet
  bool success = GSheet.values.append(
    &response,
    spreadsheetId,
    "AccessLog!A1",  // Sheet tab và vị trí bắt đầu
    &valueRange);

  if (success) {
    Serial.println("Log sent:");
    response.toString(Serial, true);
  } else {
    Serial.println("Failed to log: " + GSheet.errorReason());
  }

  valueRange.clear();
}

// Hàm xử lý khi mở khóa thành công
void handleUnlock(const String& method) {
  showMessage("OPEN", method + " OK");
  unlockDoor();

  // Reset số lần thất bại
  failedKeypad = 0;
  failedRFID = 0;
  failedApp = 0;

  // Ghi log Google Sheet
  if (GSheet.ready()) {
    writeLogToGoogleSheet(method, "Success");
  }
}

// Hàm xử lý thất bại
void handleFailedAttempt(const String& method, const String& reason) {
  showMessage("LOCKED", reason);

  if (GSheet.ready()) {
    writeLogToGoogleSheet(method, reason);
  }

  if (method == "Keypad") {
    onKeypadUnlockFailed();
  } else if (method == "RFID") {
    onRFIDUnlockFailed();
  } else if (method == "App") {
    onAppUnlockFailed();
  }
}

void loop() {
  if (flagUnlock) {
    flagUnlock = false;
    handleUnlock("App");  // xử lý logging, ...
  }
  ------------------------------ Run Firebase ------------------------------------------
  if (Firebase.ready()) {
    unsigned long currentMillis = millis();

    if (currentMillis - lastCheck >= checkInterval) {
      lastCheck = currentMillis;

      if (Firebase.RTDB.getBool(&fbdo, "/unlock_request")) {
        if (fbdo.dataType() == "boolean") {
          bool request = fbdo.boolData();

          if (request) {
            Serial.println("🔓 Unlock requested!");
            handleUnlock("App");
          } else {
            Serial.println("🔒 Lock requested!");
            lockDoor();
            showMessage("LOCKED", "Nhap lai");
          }

          Reset cờ unlock
          Firebase.RTDB.setBool(&fbdo, "/unlock_request", false);
        }
      } else {
        Serial.print("❌ Firebase read error: ");
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // ------------------------------ Run RFID --------------------------------------
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Đọc UID
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }

    Serial.println("UID: " + uid);
    mfrc522.PICC_HaltA();  // dừng đọc thẻ

    if (isRFIDValid(uid)) {
      handleUnlock("RFID");
    } else {
      handleFailedAttempt("RFID", "Unknown Card");
    }

    inputPassword = "";
    return;
  }

  // ------------------------------ Run Keypad --------------------------------------
  customKeypad.tick();
  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();

    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      char key = (char)e.bit.KEY;
      Serial.print("Key pressed: ");
      Serial.println(key);

      if (key == '#') {
        if (inputPassword == PASSWORD) {
          handleUnlock("Keypad");
        } else {
          handleFailedAttempt("Keypad", "Wrong Password");
        }
        inputPassword = "";
      } else if (key == '*') {
        inputPassword = "";
        showMessage("LOCKED", "Nhap lai");
        lockDoor();
      } else if (inputPassword.length() < 8) {
        inputPassword += key;
        String hidden = "";
        for (int i = 0; i < inputPassword.length(); i++) {
          hidden += "*";
        }
        showMessage("LOCKED", "INPUT: " + hidden);
      }
    }
  }
}
