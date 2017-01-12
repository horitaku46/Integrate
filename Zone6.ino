

// Zone6
const Color COLOR_LIGHT_GREEN(25, 59, 25);
const Color COLOR_PINK(65, 30, 50);
const Color COLOR_BLUE_WHITE(55, 60, 80);


// 図形認識ゾーン
void zone6() {

//  // 強制ゾーン移動処理
  int timeOver = zoneTimeOver( 20006 );
  if ( timeOver == 1 ) {
    mode_G = 0;
    zoneNumber_G = 0;
    return;
  }

  // Zumoの方向を計測
  calcDirection();

  switch ( mode_G ) {
    case 0: // setup（必要がなくても形式的に置いておく）
      mode_G = 1;
      break;
      
    case 1:// 青白を感知するまで前進
      // 黄緑とピンクを感知したら図の方向に正す
      if ( isColorDecision(COLOR_LIGHT_GREEN, 10) ) {
        motorL_G = SPEED + 100;
        motorR_G = SPEED - 100;
      } else if ( isColorDecision(COLOR_PINK, 10) ) {
        motorL_G = SPEED - 100;
        motorR_G = SPEED + 100;
      } else {
        goStraight( 100 );
      }

      // 青白を感知
      if ( isColorDecision(COLOR_BLUE_WHITE, 10) ) {
        mode_G = 2;
      }
      break;
    
    case 2: // 青と白のライントレース
      linetracePD_Blue();
      break;
    
    default:
      break;
  }
}



// 青のライントレース
void linetracePD_Blue() { 
  
  static unsigned long timePrev = 0; // 経過時間
  static float lightPrevPD = 0.0; // 一つ前の明るさの値
  float lightNowPD; // 現在の明るさ
  float error, errorSP; // 誤差
  float diff, diffSP; // 差分
  float speedDiff; // 制御速度
  float lightNowPD_MIN = 31; // 明るさの最小値
  float lightNowPD_MAX = 100; // 明るさの最大値
  float Kp = 3.0; // P制御の重み
  float Kd = 6.0; // D制御の重み

  // 現在の明るさを算出
  lightNowPD =  ( red_G + green_G + blue_G ) / 3.0;

  // 制御量を出すためマッピングしP制御とD制御の値を算出
  errorSP = map(lightNowPD, lightNowPD_MIN, lightNowPD_MAX, -SPEED, SPEED );
  diff = (lightNowPD - lightPrevPD) / (timeNow_G - timePrev );
  diffSP  = map(diff, -100.0, 100.0, -SPEED, SPEED );
  speedDiff = Kp * errorSP + Kd * diffSP;

  // モーターに制御速度を実装
  motorL_G = SPEED - speedDiff;
  motorR_G = SPEED + speedDiff;

  // 前回の値を記憶
  timePrev = timeNow_G; 
  lightPrevPD = lightNowPD; 
}


