#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 3
// 動作確認用LEDのGPIOPIN なんか光らせたり
#define SIGNAL_PIN 2

// MOTER_PIN 
#define MOTER_PIN 15 // TODO ここでPWM使えるか調べてない

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void configDeviceAP() {
  const char *SSID = "OBAKE_1";
  bool result = WiFi.softAP(SSID, "OBAKE_NO_PW", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void setup() {
  /*
   * 速度9600なのはTeraTermで見るときの初期値がそうなっているので合わせているだけ。
   * 特に速度の影響出たわけではないので必要なら上げても多分OK。
   */
  Serial.begin(9600);
  Serial.println("");
  Serial.println("OBAKE_1 Reseiver Start");

  pinMode(SIGNAL_PIN, OUTPUT);
  pinMode(MOTER_PIN, OUTPUT);

  WiFi.mode(WIFI_AP);
  configDeviceAP();
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);
}

// TODO モーター実装。とりあえずPWM強度と秒数受けて動くぐらい？
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  if(*data == 0){
    Serial.println("zero");
    digitalWrite(MOTER_PIN, HIGH);
  } else if(*data == 1){
    Serial.println("ichi");
    digitalWrite(MOTER_PIN, LOW);
  } else {
    // TODO PWM使えるようにしてから子機間違えそうなのであさーと入れる
    Serial.println("hoka");
  }
  Serial.println("");
}

boolean led_flg = true;
void loop() {
  digitalWrite(SIGNAL_PIN, led_flg = !led_flg );
  delay(10000);
}
