#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ArduinoJson.h>

// Konstanta untuk AP mode
const char* apSSID = "ESP8266-server";
const char* apPassword = "irfandiproduct";
const char* tokenDevice = "MuY4mUtlH69J3";
const char* gasServerUrl = "http://192.168.100.10/gasdata"; // Ganti dengan URL server tujuan

int Gas_Analog = A0;
int Buzzer = D0;
int dataGas = 0;
bool hasConnection = false;
String logUsername, logPassword;

// Web server pada port 80
AsyncWebServer server(80);

// Fungsi untuk menulis file konfigurasi
void saveConfig(const char* path, const char* ssid, const char* password, bool internet) {
  StaticJsonDocument<256> doc;
  doc["internet"] = internet;
  doc["ssid"] = ssid;
  doc["password"] = password;

  File file = SPIFFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  serializeJson(doc, file);
  file.close();
  Serial.println("Configuration saved successfully");
}

// Fungsi untuk membaca file konfigurasi
bool loadConfig(const char* path, String& ssid, String& password, bool& internet) {
  File file = SPIFFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return false;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse JSON file");
    file.close();
    return false;
  }

  internet = doc["internet"];
  ssid = String((const char*)doc["ssid"]);
  password = String((const char*)doc["password"]);
  file.close();
  return true;
}

// Fungsi untuk memulai mode Access Point
void startAPMode() {
  WiFi.softAP(apSSID, apPassword);
  Serial.println("AP Mode Started");
  Serial.printf("SSID: %s, Password: %s\n", apSSID, apPassword);
  Serial.println(WiFi.softAPIP());
  delay(500);
  digitalWrite(Buzzer, LOW);
  delay(1000);
  digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);
  delay(250);
  digitalWrite(Buzzer, HIGH);
  delay(100);
  digitalWrite(Buzzer, LOW);

  // Konfigurasi halaman web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html lang="en">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Wifi Setup - ESP8266 Configuration</title>
        <style>
          body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
            text-align: center;
            padding: 20px;
          }
          h1 {
            color: #444;
          }
          form {
            display: inline-block;
            background: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
          }
          input[type="text"], input[type="password"] {
            width: 95%;
            padding: 10px;
            margin: 10px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
          }
          button {
            background-color: #007BFF;
            color: white;
            padding: 10px 15px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
          }
          button:hover {
            background-color: #0056b3;
          }
        </style>
      </head>
      <body>
        <h1>WiFi Configuration - Please set up your wifi at here</h1>
        <form action="/setwifi" method="POST">
          <label for="ssid">SSID:</label>
          <input type="text" id="ssid" name="ssid" required><br>
          <label for="password">Password:</label>
          <input type="password" id="password" name="password" required><br>
          <button type="submit">Save</button>
        </form>
      </body>
      </html>
    )rawliteral");
  });

  server.on("/setwifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();

    saveConfig("/config.txt", ssid.c_str(), password.c_str(), true);

    request->send(200, "text/plain", "WiFi credentials saved! please Reboot the ESP8266.");
  });

  server.begin();
}

// Fungsi untuk menghubungkan ke WiFi
void connectToWiFi(const String& ssid, const String& password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.printf("Connecting to WiFi SSID: %s\n", ssid.c_str());

  unsigned long startAttemptTime = millis();

  // Tunggu hingga koneksi berhasil atau timeout setelah 10 detik
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi, switching to AP mode");
    startAPMode();
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
            <title>Home - Air Detector</title>
            <style>
                body {
                    margin: 0;
                    font-family: Arial, sans-serif;
                    background-color: #f0fdf4; /* Light green background */
                    color: #333;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    height: 100vh;
                    flex-direction: column;
                }

                .dashboard {
                    display: flex;
                    flex-wrap: wrap;
                    gap: 20px;
                    justify-content: center;
                }

                .container {
                    text-align: center;
                    background: #ffffff;
                    border-radius: 15px;
                    box-shadow: 0 8px 30px rgba(0, 0, 0, 0.2);
                    padding: 40px;
                    width: 300px;
                    max-width: 90%;
                }

                .icon {
                    margin-bottom: 20px;
                }

                .icon svg {
                    width: 60px; /* Adjust the size of the icon */
                    height: auto;
                }

                .icon.air svg {
                    fill: #16a34a; /* Green for air */
                }

                .icon.temp svg {
                    fill: #f97316; /* Orange for temperature */
                }

                .value {
                    font-size: 4rem;
                    font-weight: bold;
                    margin-bottom: 10px;
                }

                .value.air {
                    color: #16a34a; /* Green */
                }

                .value.temp {
                    color: #f97316; /* Orange */
                }

                .status {
                    font-size: 1.2rem;
                    font-weight: bold;
                    transition: color 0.3s ease;
                }

                .status.good {
                    color: #065f46; /* Dark green */
                }

                .status.moderate {
                    color: #ca8a04; /* Amber */
                }

                .status.poor {
                    color: #dc2626; /* Red */
                }
            </style>
        </head>
        <body>
            <div class="dashboard">
                <div class="container">
                    <div class="icon air">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                            <path d="M12 2a7.48 7.48 0 0 0-5.3 2.2 7.52 7.52 0 0 0-.6 10.7l5.9 5.9a1 1 0 0 0 1.4 0l5.9-5.9a7.52 7.52 0 0 0-.6-10.7A7.48 7.48 0 0 0 12 2zm0 2a5.52 5.52 0 0 1 3.9 1.6 5.53 5.53 0 0 1 .4 7.9l-4.3 4.3-4.3-4.3a5.53 5.53 0 0 1 .4-7.9A5.52 5.52 0 0 1 12 4z" />
                        </svg>
                    </div>
                    <div class="value air" id="airValue">75</div>
                    <div class="status good" id="airStatus">Good Air Quality</div>
                </div>

                <div class="container">
                    <div class="icon temp">
                        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
                            <path d="M13 10.267V5a1 1 0 0 0-2 0v5.267a4 4 0 1 0 2 0zM12 2a3 3 0 0 1 3 3v5.07a6 6 0 1 1-6 0V5a3 3 0 0 1 3-3z" />
                        </svg>
                    </div>
                    <div class="value temp" id="tempValue">25°C</div>
                    <div class="status good" id="tempStatus">Normal Temperature</div>
                </div>
            </div>

            <script>
                function updateAirData(value) {
                    const airValue = document.getElementById('airValue');
                    const airStatus = document.getElementById('airStatus');

                    airValue.textContent = value;

                    if (value >= 0 && value <= 100) {
                        airStatus.textContent = "Excellent Air Quality";
                        airStatus.className = "status good";
                    } else if (value > 100 && value <= 300) {
                        airStatus.textContent = "Minimal Gas Detected";
                        airStatus.className = "status good";
                    } else if (value > 300 && value <= 500) {
                        airStatus.textContent = "Light Gas Detection";
                        airStatus.className = "status moderate";
                    } else if (value > 500 && value <= 700) {
                        airStatus.textContent = "Higher Gas Levels Detected";
                        airStatus.className = "status poor";
                    } else if (value > 700 && value <= 900) {
                        airStatus.textContent = "Hazardous Gas/Smoke Detected";
                        airStatus.className = "status poor";
                    }
                }

                function updateTempData(value) {
                    const tempValue = document.getElementById('tempValue');
                    const tempStatus = document.getElementById('tempStatus');

                    tempValue.textContent = `${value}°C`;

                    if (value < 18) {
                        tempStatus.textContent = "Too Cold";
                        tempStatus.className = "status poor";
                    } else if (value <= 25) {
                        tempStatus.textContent = "Normal Temperature";
                        tempStatus.className = "status good";
                    } else {
                        tempStatus.textContent = "Too Hot";
                        tempStatus.className = "status poor";
                    }
                }

                function fetchGasData() {
                    setInterval(() => {
                        fetch('/gas')
                            .then(response => response.json())
                            .then(data => {
                                const percentage = data.gasPercentage;
                                updateAirData(percentage);
                            })
                            .catch(error => console.error('Error fetching gas data:', error));
                        
                        const randomTempValue = Math.floor(Math.random() * 41); // Simulate temperature data between 0-40°C
                        updateTempData(randomTempValue);
                    }, 1000);
                }

                document.addEventListener('DOMContentLoaded', fetchGasData);
            </script>
        </body>
        </html>
    )rawliteral");
  });

  server.on("/gas", HTTP_GET, [](AsyncWebServerRequest *request) {
      String json = "{\"gasPercentage\": " + String(dataGas) + "}";
      request->send(200, "application/json", json);
  });

  server.begin();
}

void setup() {
  Serial.begin(9600);
  pinMode(Gas_Analog, INPUT);
  pinMode(Buzzer, OUTPUT);

  // Inisialisasi SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
  digitalWrite(Buzzer, HIGH);
  delay(500);
  digitalWrite(Buzzer, LOW);

  // Cek apakah file konfigurasi ada
  String ssid, password;
  bool internet;

  if (loadConfig("/config.txt", ssid, password, internet)) {
    Serial.println("Configuration loaded:");

    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(200);
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);

    Serial.println("Internet: " + String(internet ? "true" : "false"));
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + password);

    if (internet) {
      hasConnection = true;
      connectToWiFi(ssid, password);
      digitalWrite(Buzzer, HIGH);
      delay(1000);
      digitalWrite(Buzzer, LOW);
    } else {
      hasConnection = false;
      digitalWrite(Buzzer, HIGH);
      startAPMode();
    }
  } else {
    digitalWrite(Buzzer, HIGH);
    Serial.println("Failed to load configuration, starting AP mode");
    saveConfig("/config.txt", "", "", false);
    startAPMode();
  }

  server.on("/buzzer", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();
      if (state == "on") {
        digitalWrite(Buzzer, HIGH); // Nyalakan buzzer
        request->send(200, "text/plain", "Buzzer dinyalakan!");
      } else if (state == "off") {
        digitalWrite(Buzzer, LOW); // Matikan buzzer
        request->send(200, "text/plain", "Buzzer dimatikan!");
      } else {
        request->send(400, "text/plain", "Argumen tidak valid! Gunakan /buzzer?state=on atau /buzzer?state=off.");
      }
    } else {
      request->send(400, "text/plain", "Parameter 'state' tidak ditemukan!");
    }
  });
  server.begin();
}

void loop() {
  // Tidak diperlukan loop khusus karena AsyncWebServer menangani semuanya
  int gas = analogRead(Gas_Analog);
  dataGas = gas;

  if (gas > 500) {
    digitalWrite(Buzzer, HIGH);
  } else {
    digitalWrite(Buzzer, LOW);
  }
  delay(500);
}
