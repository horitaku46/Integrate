

// Zone4
const Color COLOR_BROWN_WHITE(70, 65, 65);
const Color COLOR_LIGHT_BLUE(40, 75, 85);
// Point Color
const Color COLOR_DARK_GREEN(6, 16, 6);
const Color COLOR_GLAY(72, 92, 72);
const Color COLOR_PURPLE(75, 25, 25);
const Color COLOR_THIN_BLUE(15, 26, 62);
const Color COLOR_BLUE(3, 7, 28);


// アイテムグラバーゾーン
void zone4() {
  
  // 強制ゾーン移動処理
  int timeOver = zoneTimeOver( 20004 );
  if ( timeOver == 1 ) {
    mode_G = 0;
    zoneNumber_G = 0;
    return;
  }
  
  detectColorPoint();
  
  switch( mode_G ) {
    case 0: // setup（必要がなくても形式的に置いておく）
      
//      linetraceBrownWhite(); // ライントレーステスト

//      detectColorPoint(); // カラーポイント検知テスト

      mode_G = 1;
      break;
      
    case 1: // 茶白を見つけるまで前進
      goStraight( 120 );
      // 茶白を判定
      if ( isColorDecision(COLOR_BROWN_WHITE, 10) ) {
        mode_G = 2;
      }
      break;
      
    case 2: // 茶白をライントレース
      linetraceBrownWhite();
      // 水色の判定
      if ( isColorDecision(COLOR_LIGHT_BLUE, 10) ) {
        mode_G = 3;
      }
      break;
      
    case 3: // カラーポイントを感知
      goStraight( 100 );
      // 水色の判定
      if ( isColorDecision(COLOR_LIGHT_BLUE, 10) ) {
        mode_G = 4;
      }
      break;
      
    case 4: // 少しだけ進む
      goStraight( 120 );
      // 200ms間だけ前進して停止
      if ( steadyState( 200 ) ) {
        stopStay();
        mode_G = 5;
      }
      break;

    case 5: // 次のラインを探索
      // その場で右回転
      if ( detectColor_G == 1 ) {
        turnLeft( 200 );
      } else {
        turnRight( 200 );
      }
      // 茶白を検知
      if ( isColorDecision(COLOR_BROWN_WHITE, 10) ) {
        mode_G = 2;
      }
      break;
      
    default:
      break;
  }
}



// 茶白色をライントレース
void linetraceBrownWhite() {

  static int zoneSpeed = 200;
  
  static unsigned long timePrev = 0; // 経過時間
  static float lightPrevPD = 0.0; // 一つ前の明るさの値

  float lightNowPD; // Green値
  float error, errorSP; // 誤差
  float diff, diffSP; // 差分
  float speedDiff; // 制御速度

  float brownValue = 35.0; // 茶色のライン上の値
  float error_min = 0 - brownValue; // brownValueとの誤差の最小値
  float error_max = 100 - brownValue; // brownValueとの誤差の最大値
  float Kp = 2.0; // P制御の重み
  float Kd = 2.5; // D制御の重み

  // 現在の明るさを代入
  lightNowPD = ( red_G + green_G + blue_G ) / 3.0;
  
  // 制御量を出すためマッピングしP制御とD制御の値を算出
  error = (lightNowPD - brownValue);
  errorSP =  map(error, error_min, error_max, -zoneSpeed, zoneSpeed );
  diff = (lightNowPD - lightPrevPD) / (timeNow_G - timePrev );
  diffSP = map(diff, -100.0, 100.0, -zoneSpeed, zoneSpeed );
  
  // 各重みを掛け合わせ実際に加える制御量を算出
  speedDiff = Kp * errorSP + Kd * diffSP;

  // 白なら右回転、茶なら左回転
  if ( speedDiff > brownValue ) {
    motorL_G = zoneSpeed - speedDiff;
    motorR_G = zoneSpeed;
  } else {
    motorL_G = zoneSpeed;
    motorR_G = zoneSpeed + speedDiff;
  }


  // 一つ前の値に代入
  timePrev = timeNow_G;
  lightPrevPD = lightNowPD;
}



// カラーポイントを検知する
int detectColorPoint() {
  // 検知した色の番号を代入
  if ( isColorDecision(COLOR_DARK_GREEN, 10) ) {
    detectColor_G = 1;
  } else if ( isColorDecision(COLOR_GLAY, 10) ) {
    detectColor_G = 2;
  } else if ( isColorDecision(COLOR_PURPLE, 10) ) {
    detectColor_G = 3;
  } else if ( isColorDecision(COLOR_THIN_BLUE, 10) ) {
    detectColor_G = 4;
  } else if ( isColorDecision(COLOR_BLUE, 10) ) {
    detectColor_G = 5;
  }
}


