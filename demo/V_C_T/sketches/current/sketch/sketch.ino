#include <WiFiS3.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

int analogPin = A0;

const int averageValue = 500;
long int sensorValue = 0;
float sensitivity = 0.100; 

float voltage = 0;
float current = 0;
float zeroOffset = 0;
unsigned long lastSample = 0;
const int sampleInterval = 1000; // 1 second

char serverAddress[] = "192.168.0.111";
int port = 8086;

char ssid[] = "Arvind";   // WiFi SSID
char pass[] = "Chinnu2002"; // WiFi password

WiFiServer server(801);  // create a server on port 801 (HTTP)
String accessKey = "9zj9f8rzehh2hexpdt0a";

WiFiClient wifi;
HttpClient apiClient = HttpClient(wifi, serverAddress, port);

const String influxDBBucket = "telemetry";
const String influxDBToken = "A0uOFYv_WhO13kMh0m1frgynZBnB1KEwnaALsIzGKDUqeNrLM9vMjQRM9ZA9ToepqHSZcvVFmF0YCvbID4K94g==";
const String org = "Vidasmi Drone Private Limited";
const String orgID = "5e718852ea4f4dff";
const String bucket = "SolarTelemetry";

void setup() {
  Serial.begin(9600);
  long sum = 0;

  for (int i=0; i < averageValue; i++) {
    sum += analogRead(analogPin);
    delay(5);
  }
  zeroOffset = (sum/averageValue) * 5.0 / 1024.0;
  Serial.print("Zero Offset: ");
  Serial.println(zeroOffset);

  // attempt to connect to WiFi
  Serial.print("Connecting to WiFi...");
  int status = WiFi.begin(ssid, pass);

  while (status != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    status = WiFi.status();
  }

  delay(5000);
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the web server
  server.begin();
  Serial.println("Web server started.");
}

void getCurrent_MA() {
  
  sensorValue = 0;
  for (int i = 0; i < averageValue; i++)
  {
    sensorValue += analogRead(analogPin);
    delay(10);
  }

  sensorValue = sensorValue / averageValue;
  voltage = sensorValue * 5.0 / 1024.0;
  current = (voltage - zeroOffset) / sensitivity;
  //current = (voltage) / 0.185;

  Serial.print("ADC Value: ");
  Serial.print(sensorValue);

  Serial.print("   ADC Voltage: ");
  Serial.print(voltage);
  Serial.print("V");

  Serial.print("   Current: ");
  Serial.print(current);
  Serial.println("A");
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample >= sampleInterval) {
    lastSample = now;
    getCurrent_MA();
  }
  //publishTelemetryJSON(current);
  publishTelemetryToInfluxDB(current);
  delay(10);
}

void publishTelemetryToInfluxDB(float current)
{
  String url = "/api/v2/write?org=" + orgID + "&bucket=" + bucket + "&precision=s";
  String data = "SolarTelemetry Current=" + String(random(0, 12)) + // Amps
                        ",Voltage=" + String(random(500, 1500)) + // VDC
                        ",Temperature=" + String(random(-10, 85)) +  // C
                        ",HorizontalIrradiance=" + String(random(0, 1200)) + // W
                        ",ModuleTemperature=" + String(random(-10, 85)) + // C
                        ",ArrayCurrent=" + String(random(-20, 50)) + // Amps
                        ",ArrayVoltage=" + String(random(500,1500)) + // VDC
                        ",InverterACPower=" + String(100*(random(10, 20)/10.0)) + // kWH/min for 100 acres
                        ",InverterEfficiency=" + String(random(960, 990)/10.0) + // %ge
                        ",GridFrequency=" + String(random(49.5, 50.5)) + // Hz
                        ",ActivePower=" + String(random(-20, 50)) + 
                        ",CUF=" + String(random(150, 250)/10.0); // %ge
          
  apiClient.beginRequest();
  apiClient.post(url); // Your server endpoint
  apiClient.sendHeader("Content-Type", "text/plain");
  apiClient.sendHeader("Authorization", "Token " + String(influxDBToken));
  apiClient.sendHeader("Content-Length", data.length());

  Serial.print("Writing to API: "); 
  Serial.println(url);
  Serial.println(data);

  apiClient.beginBody();
  apiClient.print(data);
  apiClient.endRequest();

  // Response
  int statusCode = apiClient.responseStatusCode();
  String response = apiClient.responseBody();
  Serial.print("Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  apiClient.stop();
}

void publishJSONToAPI(String content)
{
  String url = "/api/v1/" + accessKey + "/telemetry";
  apiClient.beginRequest();
  apiClient.post(url); // Your server endpoint
  apiClient.sendHeader("Content-Type", "application/json");
  apiClient.sendHeader("Content-Length", content.length());
  //apiClient.sendHeader("Authorization", accessKey);
  Serial.print("Writing to API: ");
  Serial.println(content);

  apiClient.beginBody();
  apiClient.print(content);
  apiClient.endRequest();

  // Response
  int statusCode = apiClient.responseStatusCode();
  String response = apiClient.responseBody();
  Serial.print("Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  apiClient.stop();
}

void publishTelemetryJSON(float current)
{
  StaticJsonDocument<200> doc;
  doc["Current"] = random(0, 5000) / 1000.0; // 1.0; //current
  doc["Voltage"] = random(0, 220);
  doc["Temperature"] = random(-40, 50);
  //doc["Current"] = current;
  // Serialize to string
  String jsonString;
  serializeJson(doc, jsonString);
  publishJSONToLocalServer(jsonString);
  publishJSONToAPI(jsonString);
}

void publishText(float current)
{
   // Handle web server
  WiFiClient client = server.available();
  if (client) {
      Serial.println("New client connected");

      char buf[64];
      String request = client.readStringUntil('\r');  // read request
      Serial.println(request);
      client.flush();

      // Send response
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println();

      client.println("<!DOCTYPE html><html>");
      client.println("<h1>Vidasmi IoT dashboard</h1>");
      sprintf (buf, "<p>Current value: %0.3f A </p>", current);
      client.println(buf);
      client.println("</html>");

      client.stop();  // close the connection
      Serial.println("Client disconnected");
  }
}


void publishJSONToLocalServer(String content)
{
  WiFiClient client = server.available();
  if (client) {
      Serial.println("New client connected");

      char buf[64];
      String request = client.readStringUntil('\r');  // read request
      Serial.println(request);
      client.flush();

      // Send response
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:application/json");
      client.println();

      client.println(content);

      client.stop();  // close the connection
      Serial.println("Client disconnected");
}

}
