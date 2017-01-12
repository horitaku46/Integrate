#include <Integrate.h>
#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>


ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;


/** グローバル定数
 *  Processingに送信したい値を実装（語尾に_G）
 *  フラグの場合は各Zoneファイルに実装
 */
#define CARIBARATION_AUTO // RGBと地磁気のキャリブレーション行う
//#define CARIBARATION_MANUAL // RGBと地磁気のキャリブレーション値を入力

#define SPEED 160 // Zumoのモータに与える回転力の基準値

// RGBのキャリブレーションの手動入力値
#define CARIBARATION_R_MAX 10148
#define CARIBARATION_G_MAX 17393
#define CARIBARATION_B_MAX 10376
#define CARIBARATION_R_MIN 1289
#define CARIBARATION_G_MIN 3102
#define CARIBARATION_B_MIN 1562

// 地磁気のキャリブレーションの手動入力値
#define CARIBARATION_COMPASS_MAX_X -1617.0
#define CARIBARATION_COMPASS_MAX_Y -368.0
#define CARIBARATION_COMPASS_MIN_X -4073.0
#define CARIBARATION_COMPASS_MIN_Y -3182.0


float red_G, green_G, blue_G; // カラーセンサーで読み取ったRGB値（0-100）
int zoneNumber_G; // ゾーン番号を表す状態変数
int mode_G; // 各ゾーンでのモードを表す状態変数
int dataR_max, dataG_max, dataB_max; // Colorキャリブレーション値
int dataR_min, dataG_min, dataB_min;
unsigned long timeInit_G, timeNow_G; //  スタート時間，経過時間
int motorR_G, motorL_G; // 左右のZumoのモータに与える回転力
float direction_G; // 現在Zumoが向いている方向(0~360)


int flagFinish2 = 0; // Line traceing done, Exit zone
unsigned long startTime;

// デバッグ用のブザーピン
const int buzzerPin = 3;

int target1;

//zone2(宝探し)
int state2=0;

// Zone3
float Kp = 0.02;
float Ki = 0;
float sum_e = 0.0;
float u,e;

// Zone4のグローバル変数
int detectColor_G; // ポイントとなる色の判定色(深緑:1,灰:2,紫:3,薄い青:4,青:5)

// Zone5(棒倒し)
// int zone_in=0;
boolean findFlag = false;
boolean approachFlag = false;
int state=0;//switch文で使用
float azimuth = 0;
float start_azimuth;
unsigned long L, distanceL; //距離
float C = 340;//音速
int zone5L = 0, zone5R = 0;
int countP = 0;//ペットボトルを倒した数
int countOneP = 0;
const int trig = 4;//Trig ピンをデジタル 4 番に接続
const int echo = 5; //Echo ピンをデジタル 2 番に接続
const int power = 13;
unsigned long interval;
extern int pattern[3];
extern int masu[5][5];


// 設定関数
void setup() {
  Serial.begin(9600); // シリアル通信を9600bpsに初期化
  Wire.begin(); // I2C通信開始


#ifdef CARIBARATION_AUTO
  button.waitForButton();
  setupColorSensor(); // カラーセンサーのsetup
  CalibrationColorSensorWhite(); // カラーセンサーのキャリブレーション 
  button.waitForButton();
  CalibrationColorSensorBlack(); // カラーセンサーのキャリブレーション
  setupCompass(); // 地磁気センサーのsetup
  CalibrationCompass(); // 地磁気センサーのキャリブレーション
  
#elif CARIBARATION_MANUAL
  setupColorSensor(); // カラーセンサーのsetup
  CalibrationColorSensorManual(); // カラーセンサーのキャリブレーション（手動設定）
  setupCompass(); // 地磁気センサーのsetup
  CalibrationCompassManual(); // 地磁気センサーのキャリブレーション（手動設定）
  
#endif

  // 初期設定
  zoneNumber_G = 0; // zoneToZoneから
  mode_G = 2; // スタートからは2、途中のゾーンから次の方向に向くのは0
  
  button.waitForButton();
  // スタート時間
  timeInit_G = millis();
  // スタート時点の方角を保存
  target1 = averageHeading();

  // 200ms間だけブザーを鳴らす
  tone(buzzerPin, HIGH);
  delay(200);
  noTone(buzzerPin);
}



// main関数
void loop() {
  // カラーセンサでRGB値を取得(0-100)
  readRGB();
  // 経過時間
  timeNow_G = millis() - timeInit_G;
  // Zumoの方向
  direction_G = averageHeadingLP();
  
  // 左右モーターへの回転力入力
  motors.setSpeeds(motorL_G, motorR_G);

  // データ送信
  sendData();

  switch ( zoneNumber_G ) {
  case 0:
    zoneToZone(); // ゾーンからゾーンの移動
    break;

  case 1:
    zone1(); // ミュージックプレイゾーン
    break;

  case 2:
    zone2(); // 宝集めゾーン
    break;

  case 3:
    zone3(); // 山登らずゾーン
    break;

  case 4:
    zone4(); // アイテムグラバーゾーン
    break;

  case 5:
    zone5(); // 棒倒しゾーン
    break;

  case 6:
    zone6(); // 図形認識ゾーン
    break;

  case 7:
    stopStay();
//      zone_finish(); // フィニッシュアクション
    break;

  default:
    break;
  }
}



// Processingに送信
void sendData() {
  static unsigned long timePrev = 0;
  int sec;

  sec = timeNow_G / 1000; // 秒数に変換

  if ( timeNow_G - timePrev > 50 ) { // 50msごとにデータ送信
    Serial.write('H');
    Serial.write(zoneNumber_G);
    Serial.write(mode_G);

    // 経過時間（秒）
    Serial.write(sec);

    // カラーセンサのキャリブレーション値
    Serial.write(dataR_max >> 8);
    Serial.write(dataR_max & 255);
    Serial.write(dataG_max >> 8);
    Serial.write(dataG_max & 255);
    Serial.write(dataB_max >> 8);
    Serial.write(dataB_max & 255);
    Serial.write(dataR_min >> 8);
    Serial.write(dataR_min & 255);
    Serial.write(dataG_min >> 8);
    Serial.write(dataG_min & 255);
    Serial.write(dataB_min >> 8);
    Serial.write(dataB_min & 255);

    // 地磁気センサのキャリブレーション値
    Serial.write((int)(100*compass.m_max.x) >> 8);
    Serial.write((int)(100*compass.m_max.x) & 255);
    Serial.write((int)(100*compass.m_max.y) >> 8);
    Serial.write((int)(100*compass.m_max.y) & 255);
    Serial.write((int)(100*compass.m_min.x) >> 8);
    Serial.write((int)(100*compass.m_min.x) & 255);
    Serial.write((int)(100*compass.m_min.y) >> 8);
    Serial.write((int)(100*compass.m_min.y) & 255);

    // RGB
    Serial.write((int)red_G);
    Serial.write((int)green_G);
    Serial.write((int)blue_G);

    // Zumoの方向
    Serial.write((int)(100*direction_G) >> 8);
    Serial.write((int)(100*direction_G) & 255);

    // Zone4でのポイントで検知した色
    Serial.write((int)detectColor_G);
    
    // 前回の時間に現在の時間を代入
    timePrev = timeNow_G;
  }
}


