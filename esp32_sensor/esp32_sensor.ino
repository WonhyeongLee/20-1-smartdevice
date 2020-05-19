
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h> // DHT 라이브러리 추가
#include <ArduinoJson.h> // JSON 사용을 위한 라이브러리 추가
#define DHTTYPE DHT11
DHT dht(15, DHTTYPE); //DHT 설정 dht( 핀, DHT 종류)
// 온도와 습도를 저장할 변수 선언
float temp,humid;

const char* ssid = "KT_GiGA_2G_Wave2_1123";
const char* password = "ke77ff4984";

const int led_pin = 22;
const int ws_port = 52274;
//const int PushButton = 15;
uint8_t client_Ip = NULL;

AsyncWebServer server(80); //웹서버 80포트
WebSocketsServer webSocket = WebSocketsServer(ws_port); //소켓통신


char msg_buf[10]; 
int led_state=0;  //led 상태

// WebSocketsServer 라이브러리 사용 부분
void webSocketEvent(uint8_t client_Ipnum, WStype_t type, uint8_t * payload, size_t length){
  //콜백 웹소켓 받아서 처리 
   IPAddress ip = webSocket.remoteIP(client_Ipnum);
      switch(type) { //WStype_t type 의 유형에 따라 처리한다.
        case WStype_DISCONNECTED: // 연결해제 ,연결안될 때 이 메세지 뜸
            Serial.printf("[%u] Disconnected!\n", client_Ipnum);
            break;
        case WStype_CONNECTED:  //연결시
            {
                Serial.printf(" [%d.%d.%d.%d] Connected : %s\n", ip[0], ip[1], ip[2], ip[3] , payload);
                webSocket.sendTXT(client_Ipnum, "Connected");      //sendTXT 메소드로 클라이언트에 연결메세지를 보냄
                client_Ip = client_Ipnum;
            }
            break;
        case WStype_TEXT: // TEXT 받아서 처리 , 문자일시 그대로 출력해주고 LED 컨트롤을 위한 값일 때 상태조절 코드를 작성예정
            Serial.printf("[%u] get Text: %s\n", client_Ipnum, payload);

            // LED 조절 테스트코드
            if(strcmp((char*)payload, "toggleLED") == 0){
              Serial.printf("toggleLED received\n");
              led_state = led_state ? 0 : 1;
              digitalWrite(led_pin, led_state);
            }
            // LED 현재상태출력
            else if(strcmp((char*)payload, "getLEDstate") == 0){
              sprintf(msg_buf, "%d" , led_state);
              Serial.printf("Sending to [%d.%d.%d.%d] : %s\n",ip[0], ip[1], ip[2], ip[3],msg_buf);
              webSocket.sendTXT(client_Ipnum, msg_buf);
            }
            // 메세지가 없을 시
            else {
              Serial.println("[%u] Message not recognized");
            }
            break;

            
    case WStype_BIN:
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }
}
//웹서버 라이브러리 사용부분 , 스마트폰에서 ESP32에 연결하고 페이지를 요청하면 전송하는 코드
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}
//404not found , 페이지 없을 때 에러 페이지
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}
void setup() {
 
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
 
  if( !SPIFFS.begin()){
    Serial.println("Error mounting SPIFFS");
    while(1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //wifi가 잡힐때까지 1초 간격으로 연결중... 출력
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi");  // 열결시 연결완료 문구 출력 후 연결 IP 출력
  Serial.println(WiFi.localIP());

//
//  WiFi.softAP(ssid, password); 
// 
//  Serial.println();
//  Serial.println("AP running");
//  Serial.print("My IP address: ");
//  Serial.println(WiFi.softAPIP());

   // HTTP_GET 요청에 onIndexRequest메소드 실행 ( index.html 응답 )
  server.on("/", HTTP_GET, onIndexRequest);
 
  // 페이지없으면 에러메세지 출력
  server.onNotFound(onPageNotFound);
 
  // 웹서버 시작
  server.begin();
  //웹소켓 서버 시작
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop(); // loop 로 클라이언트에서 보내는 웹소켓 메세지 수신
   delay(2000);
   temp = dht.readTemperature(); // 온도를 읽어서 tem에 저장
   humid = dht.readHumidity();    // 습도를 읽어서 hum에 저장
   
  StaticJsonDocument<100> doc;  
  JsonObject root = doc.to<JsonObject>();
  root["Type"] = "DHT11_1";
  JsonObject value = root.createNestedObject("value");      
  value["Temp"] = temp;
  value["Humid"] = humid;
 
  char buffer[100];
  serializeJson(root, buffer);
  Serial.println(buffer);
  webSocket.sendTXT(client_Ip, (buffer));
 

  
  

  Serial.println("201510344_LEE WON HYEONG");
  Serial.print(F("Humidity: "));
  Serial.print(humid);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.print(F("°C "));
  Serial.println();



// 8주차 실습코드_ 버튼
//  int Push_button_state = digitalRead(PushButton);
//
//  if(Push_button_state == 1){
//         led_state = Push_button_state;
//         webSocket.sendTXT(client_Ip, "3");
//         digitalWrite(led_pin, led_state); // HIGH == 1 , LOW == 0
//    
//  }else{
//    led_state = Push_button_state;
//    webSocket.sendTXT(client_Ip, "0");
//    digitalWrite(led_pin, led_state);
//  }

}
