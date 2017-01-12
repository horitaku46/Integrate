const float colorzone1[9][3] = {
  {
    22.0 ,21.0 ,21.0                            }
  , //灰(ソ)
  {
    93.0 ,89.0 ,32.0                            }
  , //黄(ラ)
  {
    69.0 ,30.0 ,50.0                            }
  , //紫(シ)
  {
    40.0 ,71.0 ,81.0                            }
  , //水(ド)
  {
    69.0 ,14.0 ,10.0                            }
  , //赤(レ)
  {
    26.0 ,61.0 ,29.0                            }
  , //緑(ミ)
  {
    10.0 ,18.0 ,51.0                            }
  , //青(ファ)
  {
    18.0 ,28.0 ,65.0                            } 
  , //薄青(ソ)
  {
    100.0 ,100.0,100.0                            }//白(鳴らさない)
};

void zone1(){ 

  int note;
  static unsigned long timePrev1 = 0;
  int diff;
  int e = 4.0;

  // 強制ゾーン移動処理
  int timeOver = zoneTimeOver( 20001 );
  if ( timeOver == 1 ) {
    mode_G = 0;
    zoneNumber_G = 0;
    noTone( buzzerPin );
    return;
  }

  diff = target1 - direction_G;

  switch(mode_G){
  case 0:
    mode_G = 1;
    break;

  case 1:

    //if(diff > 0){
    motorL_G = SPEED + e*diff;
    motorR_G = SPEED - e*diff;

    //}
    //else if (diff < 0){
    //motorL_G = SPEED - e*diff;
    //motorR_G = SPEED + e*diff; 
    //}
    //else {
    //motorL_G = SPEED;
    //motorR_G = SPEED; 
    // }
    if(identifyColorRed(3)){
      mode_G=2;
    }
    break;
  case 2:
    //if(diff > 0){
    motorL_G = 60 + e*diff;
    motorR_G = 60 - e*diff;

    //}
    //else if (diff < 0){
    //motorL_G = 50 - e*diff;
    //motorR_G = 50 + e*diff; 
    //}
    //else {
    //motorL_G = 50;
    //motorR_G = 50; 
    //}
    if ( timeNow_G - timePrev1 > 20 ) {
      note = identifycolor();
      //Serial.write('M' ); // Arduino R へのデータ送信
      //Serial.write( note );

      playNote( note );    // 指定の音を鳴らす（または音の停止）//
      timePrev1 = timeNow_G;
    }
    if(identifyColorYellow(3)){
      mode_G = 3;
    }
    break;

  case 3:
    goStraight( -SPEED );
    if(identifyColorBlack(3)){
      mode_G = 1;
    }
    break;

  }
}

// KNNで現在最も近い番号を返す
int identifycolor() {
  static int iro; // 現状で一番近い識別色の番号
  float distance; // 距離の2乗
  float minDistance1 = 9999999;

  for ( int i = 0; i < 9; ++i ) {
    distance = (red_G - colorzone1[i][0]) * (red_G - colorzone1[i][0])
      + (green_G - colorzone1[i][1]) * (green_G - colorzone1[i][1])
        + (blue_G - colorzone1[i][2]) * (blue_G - colorzone1[i][2]);
    if ( distance < minDistance1 ) {
      minDistance1 = distance;
      iro = i;
    }
  }

  return iro;
}

void playNote( int note ) {

  static int frequency[9] = { 
    3136,3520,3952,4186,4699,5274,5588,6272,0                      }; 

  if ( note == 8 ){
    noTone( buzzerPin );
    return;
  }
  else{
    tone( buzzerPin, frequency[note] );
  }
}









