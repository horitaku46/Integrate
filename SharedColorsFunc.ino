

// 指定の色を連続countTheshold回認識したら1を返す（それ以外0)
int identifyColorBlack( int countThreshold ) {
  static int count = 0;

  if ( red_G < 15 && green_G < 15 && blue_G < 15 )  // 黒を感知
    ++count;
  else
    count = 0;

    Serial.print(count);

  if ( count >= countThreshold ) { 
    count = 0;
    Serial.print("done");
    return 1;
  }
  else
    return 0;
}



int identifyColorWhite( int countThreshold ) {
  static int count = 0;

  if ( red_G > 75 && green_G > 75 && blue_G > 75  )  // 白を感知
    ++count;
  else
    count = 0;

  if ( count >= countThreshold) { 
    count = 0;
    return 1;
  }
  else
    return 0;
}



int identifyColorYellow( int countThreshold ) {
  static int count = 0;

  if ( red_G > 70 && green_G > 65 && blue_G < 40  )  // 黄を感知
    ++count;
  else
    count = 0;

  if ( count >= countThreshold ) { 
    count = 0;
    return 1;
  }
  else
    return 0;
}



int identifyColorRed( int countThreshold ) {
  static int count = 0;

  if ( red_G > 65 && green_G < 25 && blue_G < 15  )  // 赤を感知
    ++count;
  else
    count = 0;

  if ( count >= countThreshold ) {
    count = 0;
    return 1;
  }
  else
    return 0;
}



int identifyColorGreen( int oneColor ) {
  static int count = 0;

  if (oneColor == 6 && red_G < 40 && green_G > 50 && blue_G < 40  )  // 緑を感知
    ++count;
  else
    count = 0;

  if ( count >= 3 ) { 
    count = 0;
    return 1;
  }
  else
    return 0;
}



// それぞれのRGB値の振れ幅で判定する関数
bool isColorDecision(Color color, int amp) {
  return (abs(color.r - red_G) < amp) && (abs(color.g - green_G) < amp) && (abs(color.b - blue_G) < amp);
}


