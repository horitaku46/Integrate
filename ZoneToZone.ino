

// identifyZone()用の固定値
const float color[7][3] = {
  { 93.0, 89.0, 32.0 }, // 黄
  { 69.0, 14.0, 10.0 }, // 赤
  { 40.0, 71.0, 81.0 }, // 水
  { 69.0, 30.0, 50.0 }, // 紫
  { 26.0, 61.0, 29.0 }, // 緑
  { 10.0, 18.0, 51.0 }, // 青
  { 18.0, 28.0, 65.0 }  // 薄い青
};


// 各ゾーン(1-6)の脱出方向の角度（各自で設定）
const float direction_exit[6] = { 100.0, 100.0, 200.0, 270.0, 200.0, 270.0 };
float minDistance; // identifyZone()用のグローバル変数


/** startからzoneへの移動（mode_G=2からスタート）
 *  zoneからzoneへの移動（mode_G=0からスタート) 
 */
void zoneToZone() {
  int zoneNumber;
  float direction_target;
  static int zoneNumber_in = 0; // ゾーン1-7に入った時の値を覚えておくゾーン番号を記録
  static int countTurn = 1; // 黒線衝突からの脱出用
  int diff,diff2;
  int e = 4.0;

  diff = target1 - direction_G;
  diff2 = direction_exit[zoneNumber_in - 1] - direction_G;
  
  switch ( mode_G ) {
  case 0:  // setup（必要がなくても形式的に置いておく）
    mode_G = 1;
    break;

  case 1: // direction_targetの方向を向く
    direction_target = direction_exit[ zoneNumber_in - 1 ];
    if(zoneNumber_in == 3){
      if(turnToDirection_notclim( direction_target ))  {
        mode_G = 2;
      }
    }
    else if(turnToDirection( direction_target)){
        mode_G = 2; 
    }
    break;

  case 2: // 黄を検知するまで直進
    goStraight( SPEED );
    if(zoneNumber_in == 0){
      motorL_G = SPEED + e*diff;
      motorR_G = SPEED - e*diff;
    }
    else {
      motorL_G = SPEED + e*diff2;
      motorR_G = SPEED - e*diff2;
    }
  
    
    if ( identifyColorYellow( 3 ) ) { // 黄を検知
      mode_G = 3;
    }
    if ( identifyColorBlack( 3 ) ) { // 黒を検知
      mode_G = 5;  
    }
    break;

  case 3: // 黒を検知するまで直進
    goStraight( SPEED );
    if(zoneNumber_in == 0){
      motorL_G = SPEED + e*diff;
      motorR_G = SPEED - e*diff;
    }
    if ( identifyColorBlack( 3 ) ) { // 黒を検知
      minDistance = 9999999; // identifyZone()関数で使う変数の初期化
      mode_G = 4;
    }
    break;

  case 4: // 白を検知するまで直進（その間ゾーン番号を検知）
    goStraight( SPEED );
    if(zoneNumber_in == 0){
      motorL_G = SPEED + e*diff;
      motorR_G = SPEED - e*diff;
    }
    zoneNumber = identifyZone();
    if ( identifyColorWhite( 3 ) ) { // 白を検知
      stopStay(); // その場で停止
      zoneNumber_in = zoneNumber; // どのゾーンに入ったのか記録
      zoneNumber_G = zoneNumber; // ゾーンの番号を更新
      mode_G = 0; // 状態変数の更新
    }
    break;

  case 5: // 枠線の黒を内側に脱却するまで左右に旋回を繰り返す


    linetracePID(); // 制御あり

    if ( flagFinish2 == 1 ){ // ゾーン脱出、次のゾーンへ
      zoneNumber_G = 0;
      mode_G = 3;
    }

    /*if ( countTurn % 2 == 0 ) {
     turnLeft( 150 );
     } 
     else {
     turnRight( 150 );
     }
     if ( steadyState( 200*countTurn ) ) {
     if ( !(red_G < 25 && green_G < 25 && blue_G < 25) ) { // 黒でない
     countTurn = 1;
     mode_G = 2;
     }
     } 
     else {
     ++countTurn;
     }*/
    break;

  default:
    break;
  }
}



/** 0~360度で指定した方向に向く 
 */
int turnToDirection( int direction_target ) {
  float direction, direction_diff;

  // Zumo の向いている方向を返す（磁北を0度として0～360度，角度は時計回りが正方向）
  direction = averageHeading();

  // ターゲット方向からの差異
  direction_diff = direction - direction_target;

  // direction_diffの値は-360～360度の範囲を取り得るが，-180～180度の範囲に変換
  if( direction_diff > 180.0 )
    direction_diff -= 360.0;
  else if( direction_diff < -180.0 )
    direction_diff += 360.0;

  if ( direction_diff > 2.0 ) {
    turnLeft( 150 );
    return 0;
  }
  else if ( direction_diff < -2.0 ) {
    turnRight( 150 );    
    return 0;
  }
  else{
    goStraight( 0 );
    return 1;
  }
}



int turnToDirection_notclim( int direction_target ) {
  compass.read();
  float direction, direction_diff;

  direction = averageHeading();
  direction_diff = direction - direction_target;
  if( direction_diff > 180.0 )
    direction_diff -= 360.0;
  else if( direction_diff < -180.0 )
    direction_diff += 360.0;

  float r = 5000.0; // 目標値 (要調整)
  float e = r-compass.a.x;// 偏差

  sum_e += e; // 積分値
  float u = Kp*e+Ki*sum_e; // 制御入力

  motorL_G = 250 -u;
  motorR_G = 250 +u;

  if( direction_diff < 2.0  && direction_diff > -2.0 ){
    goStraight( 0 );
    return 1;
  }
  else return 0;
}



// KNNで現在最も近い番号を返す
int identifyZone() {
  static int zoneNumber; // 現状で一番近い識別色の番号
  float distance; // 距離の2乗

  for ( int i = 0; i < 7; ++i ) {
    distance = (red_G - color[i][0]) * (red_G - color[i][0])
      + (green_G - color[i][1]) * (green_G - color[i][1])
        + (blue_G - color[i][2]) * (blue_G - color[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      zoneNumber = i;
    }
  }

  return zoneNumber + 1; // zone番号は1-7なので+1
}



void linetracePID() {
  static unsigned long timePrev = 0; // 経過時間
  static float lightPrevPD = 0.0; // 一つ前の明るさの値
  static int count = 0; // 関数が実行された回数


  float lightNowPD; // 明るさの値
  float error, errorSP; // targetとの誤差
  float diff, diffSP; // 差分
  float speedDiff; // 制御調整速度

  // 青と白のライン上の値
  float target = 50; // lightNowPD is (0~100) 青白の値
  float error_min = 0 - target; // targetとの誤差の最小値
  float error_max = 100 - target; // targetとの誤差の最大値
  float Kp = 1.5; // 重み
  float Kd = 1.8; // 重み


  lightNowPD = ( red_G + green_G + blue_G ) / 3;
  // 制御量を出すためマッピングしp制御とd制御を算出
  error = (lightNowPD - target);
  errorSP =  map(error, error_min, error_max, -SPEED, SPEED );
  diff = (lightNowPD - lightPrevPD) / (timeNow_G - timePrev );
  diffSP  = map(diff, -100.0, 100.0, -SPEED, SPEED );

  // 各重みを掛け合わせ実際に加える制御量を算出
  speedDiff = Kp * errorSP + Kd * diffSP;

  motorL_G = SPEED - speedDiff;
  motorR_G = SPEED + speedDiff;

  //  // 黄黒の混合色を感知
  if( red_G > 40 && green_G > 30 && blue_G < 20 )
    ++count;
  else
    count = 0;


  // 一つ前の値に代入
  timePrev = timeNow_G;
  lightPrevPD = lightNowPD;


  //  // 黄黒の混合色上を走行時脱出し停止する
  if ( count > 3 ) {
    motors.setSpeeds(0, 0);
    timePrev = 0; 
    count = 0;
    lightPrevPD = 0.0;
    flagFinish2 = 1;
  }
}


