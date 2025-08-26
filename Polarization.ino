/* Author: Lazarev Daniil
 * Creation Date: 2024-06-30
 * NodeMCU ESP8266 V3
 * Nema 17, A4988
 * Arduino IDE 2.3.2
 * Creates a web interface to control a stepper motor via WiFi.
 * Buttons on the web page allow turning the motor to different angles or resetting it to the initial position.
 * IP: 192.168.1.100 (replace with your network IP)
 * Change History:
 * - 2024-06-30: Created.
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// Pins connected to A4988 driver
#define STEP_PIN 5  // D1 -> GPIO5
#define DIR_PIN 4   // D2 -> GPIO4

#define STEPS_PER_REV 200  
#define STEP_DELAY 1000UL  

// Wi-Fi access point parameters
const char *ssid = "ESP8266-AP";
const char *password = "";  // Password for your ESP8266 AP, if needed

ESP8266WebServer server(80);
DNSServer dnsServer;

int currentAngle = 0;  // Current motor position

void setup() {
  // Initialize pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Setup serial for debugging
  Serial.begin(115200);
  delay(1000);  // Delay for serial port initialization

  // Calibration: move motor to initial position (0°)
  rotateToPosition(0);

  // Setup Wi-Fi access point
  Serial.println("Setting up access point...");
  WiFi.softAP(ssid, password);

  // Setup DNS server to intercept all requests
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Setup web server for Captive Portal
  server.on("/", handleRoot);  // Main page with buttons for rotation
  server.on("/rotate", handleRotation);  // Handle rotation requests
  server.on("/home", handleHome);  // Handle requests to move to home position
  server.onNotFound(handleNotFound);  // Redirect all other requests to the main page
  server.begin();
  Serial.println("Web server started");

  // Redirect all requests to the main page
  server.onNotFound([]() {
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Redirecting to main page");
  });
}

void loop() {
  dnsServer.processNextRequest();  // Process DNS requests
  server.handleClient();  // Handle web server requests
}

// Page for polarization calibration
String getPage() {
  String html = "<!DOCTYPE html>"
                "<html lang=\"ru\">"
                "<head>"
                "  <meta charset=\"UTF-8\">"
                "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "  <title>Управление двигателем</title>"
                "  <style>"
                "    body {"
                "      font-family: Arial, sans-serif;"
                "      background-color: #f0f0f0;"
                "      text-align: center;"
                "      margin: 0;"
                "      padding: 0;"
                "    }"
                "    .header {"
                "      background-color: #333;"
                "      color: #fff;"
                "      padding: 10px 0;"
                "    }"
                "    h1 {"
                "      margin-top: 20px;"
                "      color: #333;"
                "    }"
                "    .button-container {"
                "      display: flex;"
                "      justify-content: center;"
                "      align-items: center;"
                "      flex-wrap: wrap; /* Если необходимо переносить на новую строку */"
                "      margin-top: 30px;"
                "      flex-direction: column; /* Добавляем столбцовую ориентацию */"
                "    }"
                "    .button-container .button-row {"
                "      display: flex;"
                "      justify-content: center;"
                "      align-items: center;"
                "      margin-bottom: 20px; /* Отступ между рядами кнопок */"
                "    }"
                "    .button-row button {"
                "      font-size: 24px;"
                "      margin: 10px;"
                "      padding: 15px 30px;"
                "      background-color: #4CAF50;"
                "      color: white;"
                "      border: none;"
                "      border-radius: 8px;"
                "      cursor: pointer;"
                "      transition: background-color 0.3s ease;"
                "    }"
                "    .button-row button:hover {"
                "      background-color: #45a049;"
                "    }"
                "    .current-angle {"
                "      font-size: 36px;"
                "      margin-top: 20px;"
                "    }"
                "  </style>"
                "</head>"
                "<body>"
                "  <div class=\"header\">"
                "    <h2>Space.Lab</h2>"
                "  </div>"
                "  <h1>Калибровка поляризации</h1>"
                "  <div class=\"current-angle\">"
                "    Текущий угол: " + String(currentAngle) + "°"
                "  </div>"
                "  <div class=\"button-container\">"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=2&direction=clockwise'\">2° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=2&direction=counter-clockwise'\">2° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=5&direction=clockwise'\">5° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=5&direction=counter-clockwise'\">5° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=10&direction=clockwise'\">10° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=10&direction=counter-clockwise'\">10° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=20&direction=clockwise'\">20° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=20&direction=counter-clockwise'\">20° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=40&direction=clockwise'\">40° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=40&direction=counter-clockwise'\">40° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/rotate?angle=90&direction=clockwise'\">90° по часовой</button>"
                "      <button onclick=\"location.href='/rotate?angle=90&direction=counter-clockwise'\">90° против часовой</button>"
                "    </div>"
                "    <div class=\"button-row\">"
                "      <button onclick=\"location.href='/home'\">Начальное положение</button>"
                "    </div>"
                "  </div>"
                "</body>"
                "</html>";
  return html;
}

// Handler for main page
void handleRoot() {
  String html = getPage();
  server.send(200, "text/html", html);
}

// Handler for rotation requests
void handleRotation() {
  String angleStr = server.arg("angle");
  String direction = server.arg("direction");

  if (angleStr != "") {
    int angle = angleStr.toInt();
    if (direction == "clockwise") {
      rotateToPosition(currentAngle + angle);
    } else if (direction == "counter-clockwise") {
      rotateToPosition(currentAngle - angle);
    }
  }

  String html = getPage();
  server.send(200, "text/html", html);
}

// Handler for home button press
void handleHome() {
  rotateToPosition(0);  // Move motor to initial position (0°)
  currentAngle = 0;
  String html = getPage();
  server.send(200, "text/html", html);
}

// Handler for 404 - Not Found requests
void handleNotFound() {
  dnsServer.processNextRequest();
}

// Function to rotate motor to a specified position (angle)
void rotateToPosition(int targetAngle) {
  int stepsToMove = abs(currentAngle - targetAngle) * STEPS_PER_REV / 360;

  if (targetAngle > currentAngle) {
    digitalWrite(DIR_PIN, LOW);
  } else {
    digitalWrite(DIR_PIN, HIGH);
  }

  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY);
  }

  currentAngle = targetAngle;
}

