
#include "OTA_Driver.h"


RTC_DATA_ATTR int OTA_Window_Missed = 0; // RTC memory variable to keep track of OTA window


uint8_t connectTimeout = 15;

// Static IP Configuration (for Android hotspot)
IPAddress static_IP_Android(192, 168, 146, 150);
IPAddress static_IP_Iphone(172, 20, 10, 4); // Static IP address
IPAddress gateway;
IPAddress subnet;

const char* host = "esp32";
const char* ssid = "CSE";
const char* password = "12345678";

WebServer server(80);
const int ledPin = 15; // Built-in LED on most ESP32 boards (you can change this if needed)



/*
 * Login page
 */
const char* serverIndex = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: sans-serif;
        text-align: center;
        padding: 20px;
        background-color: #f4f4f4;
      }
      form {
        background: white;
        padding: 20px;
        border-radius: 10px;
        display: inline-block;
        box-shadow: 0 0 10px rgba(0,0,0,0.1);
      }
      input[type="file"] {
        margin: 10px 0;
      }
      input[type="submit"] {
        padding: 10px 20px;
        font-size: 16px;
        background-color: #007BFF;
        border: none;
        color: white;
        border-radius: 5px;
        cursor: pointer;
      }
      input[type="submit"]:hover {
        background-color: #0056b3;
      }
      #prg {
        margin-top: 10px;
        font-size: 14px;
        color: #333;
      }
    </style>
  </head>
  <body>
    <h2>ESP32 Firmware Uploader</h2>
<form method='POST' action='/update' enctype='multipart/form-data' id='upload_form'>
      <input type='file' name='update'><br>
      <input type='submit' value='Update Firmware'>
    </form>
    <div id='prg'>Progress: 0%</div>
  
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
    <script>
      $('form').submit(function(e){
        e.preventDefault();
        var form = $('#upload_form')[0];
        var data = new FormData(form);
        $.ajax({
          url: '/update',
          type: 'POST',
          data: data,
          contentType: false,
          processData: false,
          xhr: function() {
            var xhr = new window.XMLHttpRequest();
            xhr.upload.addEventListener('progress', function(evt) {
              if (evt.lengthComputable) {
                var per = evt.loaded / evt.total;
                $('#prg').html('Progress: ' + Math.round(per*100) + '%');
              }
            }, false);
            return xhr;
          },
          success: function(d, s) {
            console.log('Success!');
          },
          error: function(a, b, c) {
            console.log('Error uploading firmware');
          }
        });
      });
    </script>
  </body>
  </html>
  )rawliteral";
  


  uint8_t setupOTA() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // LED OFF initially

    Serial.println("Starting AP for OTA...");

    // Start ESP32 in Access Point mode
    WiFi.mode(WIFI_AP);
    bool apStarted = WiFi.softAP(ssid, password);

    if (!apStarted) {
        Serial.println("Failed to start AP");
        return 0;
    }

    IPAddress ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(ip);

    Serial.print("Waiting for client to connect");
    unsigned long startAttemptTime = millis();
    while ((WiFi.softAPgetStationNum() == 0) && (millis() - startAttemptTime < connectTimeout*1000)) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.softAPgetStationNum() == 0) {
        Serial.println("\nNo client connected within timeout.");
        WiFi.softAPdisconnect(true); // Stop AP
        return 0;
    }

    Serial.println("\nClient connected!");

    digitalWrite(ledPin, HIGH); // LED ON to show AP and client active
    delay(3000);


    // Serve the update form
    server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
    });

    // Handle the OTA update
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", Update.hasError() ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("Update: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
                OTA_Window_Missed = 1;
            } else {
                Update.printError(Serial);
            }
        }
    });

    server.begin();
    return 1;
}

      void disableWiFi() {
        WiFi.disconnect(true);   // Disconnect and erase credentials (true = erase)
        WiFi.mode(WIFI_OFF);     // Turn off WiFi hardware
        btStop();                // Optional: disable Bluetooth radio
      }

  void LoopOTA() {
    unsigned long lastActivity = millis();

    while (WiFi.softAPgetStationNum() > 0) {
        server.handleClient();
        delay(1);

        // Optional: Reset activity timer if serving requests
        lastActivity = millis();
    }

    Serial.println("No more clients connected to AP.");
    digitalWrite(ledPin, LOW); // LED OFF when no clients

    WiFi.softAPdisconnect(true); // Stop AP
    WiFi.mode(WIFI_OFF);
}