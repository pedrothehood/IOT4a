#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <RD03D.h>

const char* ssid = "TP-Link_2.4GHz_0494CA";
const char* password = "pedrothehood007";

#define SENSOR_RX 16 
#define SENSOR_TX 17 

//RD03D radar(Serial1);
RD03D radar(SENSOR_RX, SENSOR_TX, 256000); // RX, TX, Baudrate
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(115200);
    //Serial1.begin(256000, SERIAL_8N1, SENSOR_RX, SENSOR_TX);

    if(!LittleFS.begin(true)) {
        Serial.println("LittleFS Fehler!");
        return;
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nIP: " + WiFi.localIP().toString());

     if (radar.initialize(RD03D::RD03DMode::MULTI_TARGET)) {
    Serial.println("Sensor erfolgreich initialisiert!");
  } else {
    Serial.println("Initialisierung fehlgeschlagen! Check RX/TX & Power.");
    while(1){
      Serial.print(".");
      delay(1000);
    }
  }

    server.addHandler(&ws);
    server.serveStatic("/", LittleFS, "/");
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.begin();
}

void loop() {
  	//static TargetData*  ptrTarget ; //= radar.getTarget();    // get pointer to first target ( SINGLE DETECTION )
  static uint64_t     next_screen_update = 0;
static bool         detected = false;

  radar.tasks();
    ws.cleanupClients();

    static uint32_t lastMsg = 0;
    if (millis() - lastMsg > 50) { // 20Hz für flüssige Grafik
        lastMsg = millis();

        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        bool found = false;

        for (uint8_t i = 0; i < RD03D::MAX_TARGETS; i++) {
            TargetData* t = radar.getTarget(i);
            if (t->isValid()) {
                JsonObject obj = array.add<JsonObject>();
                obj["id"] = i;
                obj["x"] = t->x;
                obj["y"] = t->y;
                obj["s"] = t->speed;
                found = true;
            }
        }

        if (found && ws.count() > 0) {
            String out;
            serializeJson(doc, out);
            ws.textAll(out);
        }
    }
}
