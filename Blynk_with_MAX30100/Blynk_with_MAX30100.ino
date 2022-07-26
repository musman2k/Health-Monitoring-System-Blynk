#include <Wire.h>
#include <WiFi.h>
#include <Blynk.h>
#include <MAX30100_PulseOximeter.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char auth[] = "n9oEI3NB9wYKHK5Vdk0SqLTqZcOYKS_t"; 
//Blynk Authentication Token 
 
/* WiFi credentials */
char ssid[] = "ciecie"; //Your WiFi SSID
char pass[] = "wikwikwik"; //Your WiFi Password

#define DHTTYPE DHT11
#define DHTPIN 14
#define REPORTING_PERIOD_MS     3000

float BPM, SpO2;

float t;

PulseOximeter pox;
uint32_t tsLastReport = 0;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WidgetTerminal terminal(V7);

BLYNK_WRITE(V7)
{
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();

  // Ensure everything is sent
  terminal.flush();
}

//DHT11 Funtion
void sendSensor()
{
  //float h = dht.readHumidity();  // uncomment if humidity is required
  t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (/*isnan(h) ||*/ isnan(t)) {         
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, t); //for temperature gauge meter
  Blynk.virtualWrite(V6, t); //for temperature value display
//  Blynk.virtualWrite(V7, h); //for humidity gauge meter
//  Blynk.virtualWrite(V8, h); //for humidity value display
}

void setup()
{
    Serial.begin(115200);
    dht.begin();
    timer.setInterval(1000L, sendSensor);                    //calling dht11 function
    terminal.flush();
    Blynk.begin(auth, ssid, pass);
    Serial.print("Initializing pulse oximeter..");
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
        digitalWrite(1,HIGH);
    }
     pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
}
void loop()
{
  Blynk.run();
  timer.run();
  pox.update();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        // to computer serial monitor
        Serial.print("BPM: ");
        Serial.print(BPM);
        Serial.print("    SpO2: ");
        Serial.print(SpO2);
        Serial.print("%");
        Serial.println("\n");
        Blynk.virtualWrite(V1,pox.getHeartRate());
        Blynk.virtualWrite(V2,pox.getHeartRate());
        Blynk.virtualWrite(V3,pox.getSpO2());
        Blynk.virtualWrite(V4,pox.getSpO2());
        if ((SpO2>95 && SpO2 <100) && (BPM>70 && BPM<100)){
          terminal.println("Patient's health is Normal");
        }
        else {
          terminal.println("Patient's health is Abnormal.");
        }
        tsLastReport = millis();
        terminal.flush();
    }
}
