#include <Arduino.h>
#include <RD03D.h>

// Pins für das YD-ESP32-S3 Board
#define SENSOR_RX 16 // rot -> an 17 am ESP32-S3, da 16 am Sensor als RX fungiert
#define SENSOR_TX 17 // schwarz ä> an 16 am ESP32-S3, da 17 am Sensor als TX fungiert
// Objekt ohne Parameter erstellen
RD03D radar(SENSOR_RX, SENSOR_TX, 256000); // RX, TX, Baudrate
//

void setup() {
  Serial.begin(115200);
  delay(2500); 
  
  Serial.println("\n--- RD-03D Radar Start ---");

  // Hardware-Serial für den S3 manuell konfigurieren
  //Serial1.begin(256000, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
  
  // Die Library nutzt bei javier-fg oft 'init' statt 'begin'
  // Wir übergeben die Referenz auf Serial1
  if (radar.initialize(RD03D::RD03DMode::MULTI_TARGET)) {
    Serial.println("Sensor erfolgreich initialisiert!");
  } else {
    Serial.println("Initialisierung fehlgeschlagen! Check RX/TX & Power.");
    while(1){
      Serial.print(".");
      delay(1000);
    }
  }
}

void loop() {
 	static TargetData*  ptrTarget ; //= radar.getTarget();    // get pointer to first target ( SINGLE DETECTION )
  static uint64_t     next_screen_update = 0;
static bool         detected = false;
  // Call the task method frequently to check for new frames.
  radar.tasks();

/*
  // Plot information, We display data a bit less oftern
  if ( millis() > next_screen_update){

    next_screen_update = millis() + 1000;   // Update next tick every second

    // Check if Target is detected, then display the values.
    if(ptrTarget->isValid())
      ptrTarget->printInfo();   // Display target information over serial
  }  */

  // Plot information, We display data a bit less oftern
  if ( millis() > next_screen_update){

    next_screen_update = millis() + 1000;   // Update next tick

    if( radar.getTargetCount() == 0){
      Serial.println("-NO TARGETS");
    }else{
      Serial.println("-TARGETS: ");

      // Iterate over the targets and look for valid
      for(uint8_t i = 0 ; i < RD03D::MAX_TARGETS; i++){
        ptrTarget = radar.getTarget(i);   // Get reference to the target

        if(ptrTarget->isValid()){
          Serial.print("  ");
          ptrTarget->printInfo();
        }
      }
    }
  }
  delay(10);
}
