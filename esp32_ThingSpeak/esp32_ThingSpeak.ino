#include <ThingSpeak.h> //ThingSpeak 라이브러리 
#include <WiFi.h>       //WIFI 라이브러리
#include <DHT.h>        //DHT11 라이브러리

#define DHTTYPE DHT11  // DHT 센서 타입 정의
DHT dht(15, DHTTYPE); //DHT 설정 dht( 핀, DHT 종류)
float temp,humid;

const char* ssid = "KT_GiGA_2G_Wave2_1123";
const char* password = "ke77ff4984";

unsigned long myChannelNumber = 1072346;         //자기 채널의 ID 입력
const char * myWriteAPIKey = "CJYCFVW0UBCDSME3"; //자기 채널의 Write API Key 입력
WiFiClient  client;
//클라이언트 공식문서 참고할 것 https://www.arduino.cc/en/Reference/WiFiClient

void setup() {
  Serial.begin(115200);  //Initialize serial
  dht.begin();
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
  while (WiFi.status() != WL_CONNECTED) { //wifi가 잡힐때까지 1초 간격으로 연결중... 출력
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi");  // 열결시 연결완료 문구 출력 후 연결 IP 출력
  Serial.println(WiFi.localIP());

}

void loop() {

  temp = dht.readTemperature(); // 온도를 읽어서 tem에 저장
  humid = dht.readHumidity();    // 습도를 읽어서 hum에 저장

  ThingSpeak.setField(1, humid);
  ThingSpeak.setField(2, temp);
  
  Serial.print(F("Humidity : "));
  Serial.println(humid);
  Serial.print(F("Temperature : "));
  Serial.println(temp);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
    
  delay(50000);

}
