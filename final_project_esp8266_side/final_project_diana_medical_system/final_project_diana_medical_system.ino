//these two lines connect to my local wifi
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

//these 2 lines ensure the communication with redis
#define dianaVM_host "YOUR IP"
#define dianaVM_portredis 6379


//wifi com
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
WiFiClient redis_WifiClient;
//pulse sensor
WiFiClient pulse_client;
IPAddress server();  //YR IP DIVIDED BY  ,
//

WiFiUDP NetworkTimeProtocol_UDP;


// for the DHT sensor
#include <DHT.h>
DHT dht11;
float DHT11_Humidity;
float DHT11_Temp;
String key;
String sensorData;

//for pulse sensor
#define pulsePin A0 
int Last_10_IBI_Array[10]; 
unsigned long pulse_timing_counter = 0; 
unsigned long IBI_last = 0; 
unsigned long lastTime = 0, N;
int BPM = 0; 
int Peak_pulse_wave = 512;
int Trough_pulse_wave = 512; 
int instant_moment_HB = 512; 
int amplitude = 100; 
int Raw_data; 
boolean Pulse = false; 
boolean firstBeat = true; 
boolean secondBeat = true; 
boolean Heartbeat_happened = false; 
int IBI = 0; 

//

//library for Network Time Protocol 
#include <NTPClient.h>
//for the timing
#define NetworkTimeProtocol_sec   60*60*5
#define NetworkTimeProtocol_mlsec 60 * 1000    
#define NetworkTimeProtocol_timezone  "0.pl.pool.ntp.org"
NTPClient time_inPoland(NetworkTimeProtocol_UDP, NetworkTimeProtocol_timezone, NetworkTimeProtocol_sec,  NetworkTimeProtocol_mlsec);
String rawData_dd;
String rawData_time;
String standardizing_time;
String standardizing_date;





void setup() {
  Serial.begin(115200);
  
  Serial.println("Serial initialized.");
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {  
    delay(500);
    Serial.println("Waiting for connection..."); 
  }
  Serial.println("");
  Serial.print("WiFi (");
  Serial.print(WiFi.macAddress());
  Serial.print(") connected with IP ");
  Serial.println(WiFi.localIP());
  Serial.print("DNS0 : ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS1 : ");
  Serial.println(WiFi.dnsIP(1));

  //set the DHT11 output datapin 
  dht11.setup(D4);
  time_inPoland.begin();
  time_inPoland.setTimeOffset(3600); 
}

void loop() {
if (Heartbeat_happened == true){ 
if (pulse_client.connect(server, 80)){ 
pulse_client.print("3\r"); 
String pulseRate = String(BPM); 
//pulseRate +="\r"; 
Serial.print("Pulse rate: ");
Serial.print(BPM);
Serial.println(" BPM.");
pulse_client.print(pulseRate); 
Serial.println("Sent to local server..");
  
  time_inPoland.update();

  standardizing_time = time_inPoland.getFormattedTime();
  standardizing_date = time_inPoland.getFormattedDate();

  
  int split_RawTime_date_time = standardizing_date.indexOf("T");
  rawData_dd = standardizing_date.substring(0, split_RawTime_date_time);
  rawData_time = standardizing_date.substring(split_RawTime_date_time+1, standardizing_date.length()-1);

    if (!redis_WifiClient.connected()) {
      Serial.print("Redis not connected, connecting...");
      if (!redis_WifiClient.connect(dianaVM_host,dianaVM_portredis)) {
        Serial.print  ("Redis could not be connected...");
        return; 
      } else
        Serial.println("Connected to VW_host");
    }




 DHT11_Humidity = dht11.getHumidity();
    DHT11_Temp = dht11.getTemperature();

    key = "MedServices:Wroclaw";
    sensorData= "DATE: "+rawData_dd+" TIME: "+standardizing_time+" || Pacient Heart Rate : "+pulseRate+" || Pacient Temperature : "+String(DHT11_Temp)+" || Pacient Humidity Level : "+String(DHT11_Humidity);
    
    Serial.print("Time Standardised : ");
    Serial.println(standardizing_time);


   redis_WifiClient.print(
      String("*3\r\n")
      +"$5\r\n"+"LPUSH\r\n"
      +"$"+key.length()+"\r\n"+key+"\r\n"
      +"$"+sensorData.length()+"\r\n"+sensorData+"\r\n"
    );
    

}
Heartbeat_happened = false;
pulse_client.stop();
delay(1000);}
else if(millis() >= (lastTime + 2)) {
readPulse();
lastTime = millis();
}

}

void readPulse() {
Raw_data = analogRead(pulsePin); 
pulse_timing_counter += 2; 
int N = pulse_timing_counter - IBI_last; 
detectSetHighLow(); 

if(N > 250){ 
if((Raw_data > instant_moment_HB) && (Pulse == false) && (N > (IBI/5)*3))
pulseDetected();
}
if (Raw_data < instant_moment_HB && Pulse == true) {
Pulse = false;
amplitude = Peak_pulse_wave - Trough_pulse_wave;
instant_moment_HB = amplitude / 2 + Trough_pulse_wave;
Peak_pulse_wave = instant_moment_HB;
Trough_pulse_wave = instant_moment_HB;
}
if (N > 2500) {
instant_moment_HB = 512;
Peak_pulse_wave = 512;
Trough_pulse_wave = 512;
IBI_last = pulse_timing_counter;
firstBeat = true;
secondBeat = true;
}
}

void detectSetHighLow() {
if(Raw_data < instant_moment_HB && N > (IBI/5)* 3)

{
if (Raw_data < Trough_pulse_wave) { 
Trough_pulse_wave = Raw_data;
}
}
if (Raw_data > instant_moment_HB && Raw_data > Peak_pulse_wave) 
{
Peak_pulse_wave = Raw_data; 
} 
}


void pulseDetected()
{
Pulse = true; 
IBI = pulse_timing_counter - IBI_last; 
IBI_last = pulse_timing_counter; 
if (firstBeat) 
{
firstBeat = false; 
return;
}
if (secondBeat) 
{
secondBeat = false; 
for (int i = 0; i <= 9; i++)
{
Last_10_IBI_Array[i] = IBI;
}
}
word runningTotal = 0; 
for (int i = 0; i <= 8; i++) 
{
Last_10_IBI_Array[i] = Last_10_IBI_Array[i + 1]; 
runningTotal += Last_10_IBI_Array[i]; 
}
Last_10_IBI_Array[9] = IBI; 
runningTotal += Last_10_IBI_Array[9]; 
runningTotal /= 10; 
BPM = 60000 / runningTotal;
Heartbeat_happened = true;
if (pulse_client.connect(server, 80)) 
{
pulse_client.print("3\r");

String pulseRate = String(BPM);

pulseRate +="\r";

Serial.print("Your BPM :");
Serial.print(BPM);
Serial.println(" BPM.");
pulse_client.print(pulseRate);
Serial.println("Sent to localhost..");
}
pulse_client.stop();
delay(1000);

}
