int analogPin = A1;

const int averageValue = 500;
long int sensorValue = 0;

float voltage = 0;
float current = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {

  for (int i = 0; i < averageValue; i++)
  {
    sensorValue += analogRead(analogPin);

  
    delay(10);
  }

  sensorValue = sensorValue / averageValue;
  voltage = sensorValue * 5.0 / 1024.0;
  current = (voltage - 2.5) / 0.185;

  Serial.print("ADC Value: ");
  Serial.print(sensorValue);

  Serial.print("   ADC Voltage: ");
  Serial.print(voltage);
  Serial.print("V");

  Serial.print("   Current: ");
  Serial.print(current);
  Serial.println("A");
}

