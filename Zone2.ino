//宝探しゾーン

const float colorzone2[5][3] = { // identifyZone()用の固定値
  {
    100, 100, 100
  },//白
  {
   69, 14, 10
  }
  ,//赤
  {
    26, 61, 29
  }
  ,//緑
  {
    10, 18, 51
  }
  ,//青(濃い)
  {
   8,16,8 
  }//深緑
};

void zone2(){
  int count;
  int blue;
  int green;
  int red;
  
  int timeout = zoneTimeOver( 20002 );
  if ( timeout == 1 ) {
    //reset variables
    mode_G = 0;
    zoneNumber_G = 0;
    //state2=4;
    //azimthswitch = 0;
    return;
  }  
  int color;
  
  switch ( state2 ) {
  
    case 0:
      state2=1;
      break;
    case 1:
    color = identifyZone2();
      motorL_G = 210;
      motorR_G = 200;
      if(identifyZone2()==3){//青を認識すれば送信
        //blue=1;
     sendData();
        
      }
      if(identifyZone2()==2){//緑を認識すれば送信
        //sendData();
       
      }
      
      if (identifyZone2() == 1 ) {//赤を認識すれば送信、モード移行
        //sendData();
        
        motors.setSpeeds(0, 0);
        delay(500);
        state2 = 2;
   
      }
      break;
    case 2:
      motorL_G = -150;
      motorR_G = -150;
      if (identifyZone2() == 3 ) {
        motors.setSpeeds(0, 0);
        delay(500);
        state2 = 3;
      }
      break;
      
    case 3:
      state2=1;
      break;
      
    case 4:
    motorL_G = 0;
    motorR_G = 0;
    //zone_in = 0;
    //zoneNumber_G=0;
     break;
      
  }
}

// KNNで現在最も近い番号を返す
int identifyZone2()
{
  float minDistance = 999999999;
  static int color;
  float distance;

  for ( int i = 0; i < 5; ++i ) {
    distance = (red_G - colorzone2[i][0]) * (red_G - colorzone2[i][0])
               + (green_G - colorzone2[i][1]) * (green_G - colorzone2[i][1])
               + (blue_G - colorzone2[i][2]) * (blue_G - colorzone2[i][2]);
    if ( distance < minDistance ) {
      minDistance = distance;
      color = i;
    }
  }
  return color;
}
