#include <WiFi.h>       //WIFI 라이브러리
#include <DHT.h>        //DHT11 라이브러리
#include <EspMQTTClient.h> // PubSubclient 라이브러리를 포함한 ESP32 MQTT Client 라이브러리 


#define DHTTYPE DHT11  // DHT 센서 타입 정의
DHT dht(15, DHTTYPE); //DHT 설정 dht( 핀, DHT 종류)
float temp1,humid1;

//const char* ssid = "KT_GiGA_2G_Wave2_1123";
//const char* password = "ke77ff4984";

char temp[10]; // temp, humid 를 문자열로 합쳐서 전송하기 위한 변수 정의
char humid[10];

EspMQTTClient client(
  "KT_GiGA_2G_Wave2_1123",
  "ke77ff4984",
  "broker.mqtt-dashboard.com",  // MQTT Broker 서버 IP 
  "201510344",   
  "9607",   
  "TestClient",     // 클라이언트 이름
  1883              // MQTT 포트번호
  );

//WiFiClient  client;
//클라이언트 공식문서 참고할 것 https://www.arduino.cc/en/Reference/WiFiClient

void setup()
{
  Serial.begin(115200);  //Initialize serial
  dht.begin();
  client.enableDebuggingMessages(); 
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");
}
  void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
//  client.subscribe("mytopic/test_201510344", [](const String & payload) {
//    Serial.println(payload);
//  });


  // Publish a message to "mytopic/test"
  client.publish("mytopic/test_201510344", "This is a message");

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/test_201510344", "This is a message sent 5 seconds later");
  });
}

//WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) { //wifi가 잡힐때까지 1초 간격으로 연결중... 출력
//    delay(1000);
//    Serial.println("Connecting to WiFi...");
//  }
//  Serial.println("Connected to the WiFi");  // 열결시 연결완료 문구 출력 후 연결 IP 출력
//  Serial.println(WiFi.localIP());


void loop() {
  client.loop();
  temp1 = dht.readTemperature(); // 온도를 읽어서 tem에 저장
  humid1 = dht.readHumidity();    // 습도를 읽어서 hum에 저장
  
  sprintf(temp, "%.2f" ,dht.readTemperature());
  sprintf(humid, "%.2f" ,dht.readHumidity());
  
  Serial.print(F("Humidity : "));
  Serial.println(humid1);
  Serial.print(F("Temperature : "));
  Serial.println(temp1);
  
 client.publish("mytopic/test_201510344_temp", temp);
 client.publish("mytopic/test_201510344_humid", humid);



  delay(5000);

}
