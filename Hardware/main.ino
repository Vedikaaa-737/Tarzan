#include <ESP8266WiFi.h>
#include <espnow.h>
#include <ESP8266WebServer.h>

#define FORWARD 1
#define BACKWARD -1
#define STOP 0

struct MOTOR_PINS {
  int pinIN1;
  int pinIN2;
};

// Motor pin configurations using GPIO numbers
MOTOR_PINS motorPins[] = {
  {6, 7},  // FRONT_RIGHT_MOTOR (D1)
  {4, 5},  // BACK_RIGHT_MOTOR (D2)
  {0, 1}, // FRONT_LEFT_MOTOR (D5)
  {2, 3}  // BACK_LEFT_MOTOR (D7)
};

ESP8266WebServer server(80);

// HTML page
const char* htmlPage PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    button {
      font-size: 30px;
      padding: 20px;
      margin: 10px;
    }
  </style>
</head>
<body>
  <h1>Car Control</h1>
  <button onclick="sendCommand('1')">Forward</button>
  <button onclick="sendCommand('2')">Backward</button>
  <button onclick="sendCommand('0')">Stop</button>

  <script>
    function sendCommand(command) {
      fetch('/command?value=' + command);
    }
  </script>
</body>
</html>
)HTML";

// Motor control functions
void rotateMotor(int motorNumber, int motorDirection) {
  if (motorDirection == FORWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  } else if (motorDirection == BACKWARD) {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
  } else {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);
  }
}

void processCarMovement(String inputValue) {
  switch (inputValue.toInt()) {
    case 1: // Forward
      for (int i = 0; i < 4; i++) rotateMotor(i, FORWARD);
      break;
    case 2: // Backward
      for (int i = 0; i < 4; i++) rotateMotor(i, BACKWARD);
      break;
    case 0: // Stop
    default:
      for (int i = 0; i < 4; i++) rotateMotor(i, STOP);
      break;
  }
}

// Handle web requests
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleCommand() {
  if (server.hasArg("value")) {
    String commandValue = server.arg("value");
    processCarMovement(commandValue);
    server.send(200, "text/plain", "Command Received");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);

  // Set motor pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);
    rotateMotor(i, STOP); // Initialize motors to STOP
  }

  // Connect to WiFi
  WiFi.begin("taksh", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Serve the HTML page and handle commands
  server.on("/", handleRoot);
  server.on("/command", handleCommand);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  server.handleClient();
}
