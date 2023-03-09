#include "MsTimer2.h"

#define LED1 10
#define LED2 9
#define LED3 8
#define SW1 4
#define SW2 5
#define SW3 6
#define SENSOR A2 // analog
#define BUZZER 2

#define BUTTON_TIME 5000 // ボタン押してから認識が継続する時間(ms)

unsigned long sw1_time, sw2_time, sw3_time; // ボタンを放した時間
int buzzer_one_shot = 0; // ブザーを短時間鳴らすための変数

bool sw1 = false;
bool sw2 = false;
bool sw3 = false;
bool sensor = false;
int sensor_raw;
char out[127];

void setup() {

  MsTimer2::set(10, time_handler);
  MsTimer2::start();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  // pinMode(SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);

  sw1_time = 0;
  sw2_time = 0;
  sw3_time = 0;

  Serial.begin(115200);
  while (!Serial);
}

void time_handler() {
  // 起動直後はボタン反応しません。
  if (millis() < BUTTON_TIME) {
    digitalWrite(BUZZER, LOW);
    return;
  }
  if (buzzer_one_shot > 0) {
    digitalWrite(BUZZER, HIGH);
    buzzer_one_shot--;
  } else {
    digitalWrite(BUZZER, LOW);
  }
}


void loop() {
  sensor_raw = analogRead(SENSOR);
  if (sensor_raw < 300) {
    sensor = true;
  } else {
    sensor = false;
  }

  if (digitalRead(SW1) == LOW) {
    buzzer_one_shot = 2;
    sw1_time = millis();
    sw2_time = 0;
    sw3_time = 0;
  }
  if (digitalRead(SW2) == LOW) {
    buzzer_one_shot = 2;
    sw1_time = 0;
    sw2_time = millis();
    sw3_time = 0;
  }
  if (digitalRead(SW3) == LOW) {
    buzzer_one_shot = 2;
    sw1_time = 0;
    sw2_time = 0;
    sw3_time = millis();
  }

  unsigned long now = millis();
  if (now - sw1_time < BUTTON_TIME) {
    sw1 = true;
    led_ctl(LED1, now - sw1_time);
  } else {
    sw1 = false;
    digitalWrite(LED1, LOW);
  }
  if (now - sw2_time < BUTTON_TIME) {
    sw2 = true;
    led_ctl(LED2, now - sw2_time);
  } else {
    sw2 = false;
    digitalWrite(LED2, LOW);
  }
  if (now - sw3_time < BUTTON_TIME) {
    sw3 = true;
    led_ctl(LED3, now - sw3_time);
  } else {
    sw3 = false;
    digitalWrite(LED3, LOW);
  }

  // 起動直後はボタン反応しません。
  if (now < BUTTON_TIME) {
    sw1 = false;
    sw2 = false;
    sw3 = false;
  }


  // 何かホストからデータが送られてきたときデータを送信します。
  if (Serial.available() > 0) {
    Serial.read();
    // sprintf(out, "_____nlab_project_alpha;sw1=%d;sw2=%d;sw3=%d;sensor=%d;sensor_raw=%04d;millis=%lu;_____", sw1, sw2, sw3, sensor, sensor_raw, now);
    // Serial.println(out);

    int d = get_data();
    Serial.write((byte) 0x00);
    Serial.write((byte) d);
    Serial.write((byte) ~d);
  }
  delay(20);
}

// ピン番号と差分時刻を受け取って、差分時刻の値によって点滅させる
// 500msごとに点滅
void led_ctl(int pin, unsigned long time) {
  int time_delta = time / 500;
  if (time_delta % 2 == 0) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

bool serial_receive() {
  delay(1);
  int data_size = Serial.available();
  int input[data_size] = {};
  for (int i = 0; i < data_size; i++) {
    input[i] = Serial.read();
  }
}

int get_data() {
  int d = 0b10000000;
  if (sensor) {
    d += 0b00000001;
  }
  if (sw1) {
    d += 0b00000010;
  }
  if (sw2) {
    d += 0b00000100;
  }
  if (sw3) {
    d += 0b00001000;
  }
  return d;
}
