#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define yellow D4
#define blue D3
#define soil A0
#define buz D7

const String ssid = "xxxxxxxxxxx";
const String password = "xxxxxxxxxxxx";

ESP8266WebServer server(80);

const char DASHBOARD[] PROGMEM = R"===(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
      <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
      <script src="https://code.jquery.com/jquery-3.7.1.min.js" integrity="sha256-/JqT3SQfawRcv/BIHPThkBvs0OEvtFFmqPF/lYI/Cxo=" crossorigin="anonymous"></script>
      <title>Controlling Water Moisture using Soil Sensor in Web Server</title>
  </head>
  <body class="statusColor">
      <div class="container bg-info text-center rounded-2 p-3 mt-4 text-white">
          <h1>Controlling Water Moisture using Soil Sensor in Web Servers</h1>
      </div>

      <div class="container">
          <div class="row" style="border: 2px solid grey; padding: 20px 0;">
              <div class="col-md-4 border-secondary" style="margin: auto;">
                  <h3>SOIL SENSOR</h3>
                  <h4 class="soilsensor">Water: </h4>
                  <h4 class="statussoil">Status: </h4>
              </div>
          </div>
      </div>
      <script>
        setInterval(function() {
          $.get("/getData", function(data, status){
              var moistureValue = parseFloat(data);
              $(".soilsensor").text("Water Percentage: " + moistureValue + "%")
              if (moistureValue <= 40) {
                  $(".statussoil").text("Status: Please Water the Plants")
                  $(".statusColor").css("background-color", "red")
              } else {
                  $(".statussoil").text("Status: Enough Water")
                  $(".statusColor").css("background-color", "#89cff0")
              }
          })
        }, 3000)
      </script>
  </body>
  </html>
)===";

// SONG
int length = 16; 
//DO RE MI FA SO LA SI DO
char notes[] = "cdefgabCCbagfedc"; 
//ketukan
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1,4 };
int tempo = 300;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(buz, HIGH);
    delayMicroseconds(tone);
    digitalWrite(buz, LOW);  
    delayMicroseconds(tone);
    }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
      }
  }
}

void indexPage() {
  String html = DASHBOARD;
  server.send(200, "text/html", html);
}

void soilMoisture() {
  float moisturePercent;
  int delayLong = 2000;
  moisturePercent = (100.00 - ((analogRead(soil)/1023.00) * 100.00));
  Serial.print("\nSoil Moisture (inPercentage) = ");
    // Buzzer
  if (moisturePercent <= 40) {
    Serial.println("Air Kering!!");
    digitalWrite(yellow, HIGH);
    digitalWrite(blue, LOW);
    digitalWrite(buz, HIGH);
    delay(500);
    digitalWrite(buz, LOW);
    delay(500);
    digitalWrite(buz, HIGH);
    delay(500);
    digitalWrite(buz, LOW);
    delay(500);
    digitalWrite(yellow, LOW);
  } else if (moisturePercent > 40) {
    Serial.println("Air Cukup!!");
    digitalWrite(blue, HIGH);
    digitalWrite(yellow, LOW);
    for (int i = 0; i < length; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } 
      else {
        playNote(notes[i], beats[i] * tempo);
      }
      // pause between notes
      delay(tempo / 2);
    }
    digitalWrite(blue, LOW);
    delay(500);
  }

  Serial.print(moisturePercent);
  Serial.println(" %");
  server.send(200, "text/html", String(moisturePercent, 2));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(buz, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(blue, OUTPUT);
  delay(1000);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("WiFi is Connected!");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, indexPage);
  server.on("/getData", HTTP_GET, soilMoisture);
  server.begin();
  Serial.println("Server is running...");
}

void loop() {
  // put your main code here, to run repeatedly:
  // Soil Moisture
  server.handleClient();
}
