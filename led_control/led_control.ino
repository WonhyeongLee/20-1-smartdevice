const int LED_BUILTIN = 22; // GPIO22번 을 사용하니 지정

void setup() {
  pinMode(LED_BUILTIN,OUTPUT); // 연결한 (지정한) 핀 번호 ,입력할지 출력할지  설정

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   //정한 핀 번호의 전압을 올려서 킨다
  delay(1000);                       // 1초 딜레이
  digitalWrite(LED_BUILTIN, LOW);    //정한 핀 번호의 전압을 내려서 끈다 
  delay(1000);                       // 1초 딜레이
}
