

// 指定時間（ms）だけ待って経過後1を返す
int steadyState( unsigned long period ) {
  
  static int flagStart = 0; // 0:待ち状態，1:現在計測中
  static unsigned long startTime = 0;

  if ( flagStart == 0 ) {
    startTime = timeNow_G;
    flagStart = 1; // 現在計測中にしておく
  }

  // 計測開始からの経過時間が指定時間を越えた
  if ( timeNow_G - startTime > period ) {
    flagStart = 0; // 待ち状態に戻しておく
    startTime = 0; // なくても良いが，形式的に初期化
    return 1;
  } else {
    return 0;
  }
}



/** 指定時間（ms）経過後強制的に抜ける
 *  コンテストで一つのzoneの時間は25秒だが、
 *  20秒から移動をしなければならない。
 */
int zoneTimeOver( unsigned long period ) {
  
  static int flagStart = 0; // 0:待ち状態，1:現在計測中
  static int zonePrev = zoneNumber_G; // 前のゾーン
  //static unsigned long startTime = 0;

  if (zonePrev != zoneNumber_G) {
    flagStart = 0;
    zonePrev = zoneNumber_G;
    
  }

  if ( flagStart == 0 ) { 
    startTime = timeNow_G;
    flagStart = 1; // 現在計測中にしておく
  }

  // 計測開始からの経過時間が指定時間を越えた
  if ( timeNow_G - startTime > period ) {
    flagStart = 0; // 待ち状態に戻しておく
    startTime = 0; // なくても良いが，形式的に初期化
    return 1;
  } else {
    return 0;
  }  
}


