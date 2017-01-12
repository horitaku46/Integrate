
void zone3(){

  static unsigned long timePrev = 0;
  
  static int count = 0;

  // 強制ゾーン移動処理
  int timeOver = zoneTimeOver( 20003 );
  if ( timeOver == 1 ) {
    mode_G = 0;
    zoneNumber_G = 0;
    return;
  }

  compass.read();

  switch(mode_G){

  case 0:
    mode_G = 1;
    break;
  case 1:
    goStraight( SPEED );
    if ( identifyColorGreen( 3 ) ) { // 緑を検知
      motors.setSpeeds(100, -100);
      delay(500);
      mode_G = 2;  
    }
    else if(compass.a.x >=4000.0){
      count++;
    }
    if(count >= 300){
      mode_G = 3;
      count = 0;
    }
    break;

  case 2:
    goStraight( SPEED );
    if(compass.a.x >=4000.0){
      count++;
    }
    if(count >= 300){
      mode_G = 3;
      count = 0;
    }
    break;

  case 3:
    motorL_G = 100;
    motorR_G = -100;
    count++;
    if(count >= 150){
      mode_G = 4;
    }
    break;

  case 4:
    if ( timeNow_G - timePrev > 20 ) {
      float r = 5000.0;
      e = r - compass.a.x;
      sum_e += e;
      u = Kp*e + Ki*sum_e;

      motorL_G = 250 -u;
      motorR_G = 250 +u;
      timePrev = timeNow_G;
    }
    break;
  }
}

