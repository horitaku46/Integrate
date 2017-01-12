//棒倒しゾーン
void zone5() {
  static int countZone2=0;
  int timeout = zoneTimeOver( 20002 );
  if ( timeout == 1 ) {
    mode_G = 0;
    zoneNumber_G = 0;

    return;
  }  
 
  //motors.setSpeeds(zone5L,zone5R);
  switch (state) {

    case 0:
      start_azimuth = averageHeading();
      if (start_azimuth < 90) {
        start_azimuth = start_azimuth + 360;
      }
      pinMode(trig, OUTPUT);
      pinMode(echo, INPUT);
      pinMode(power, OUTPUT);
      digitalWrite(power, HIGH); //電源ON
      static bool skip = true;
      motorL_G = 200;
      motorR_G = 200;
      countZone2++;
      if(countZone2>=30){
          state=1;
      }
      
      break;
    case 1: //setup
      
      
      
      state = 2;
      break;



    case 2://左に回転
      azimuth = averageHeading();
      motorL_G = -150;
      motorR_G = 150;
      if (azimuth <= start_azimuth - 90) {
        state = 3;
      }
      break;

    case 3://右に回転しながら探索,case3へ
      motorL_G  = 150;
      motorR_G  = -150;
      
      if (steadyState(100) == 1) {
        state = 4;
      }
      break;


    case 4://距離測定
      motorL_G = 0;
      motorR_G = 0;
      if (skip == true) { //１回目の測定をスキップ
        skip = false;
      }
      else {
        digitalWrite(trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig, LOW);
        interval = pulseIn(echo, HIGH, 5300);//18000μsで306㎝測定できる
        L = C * interval / 10000 / 2; //距離 340
        Serial.println(L);

        if (L == 0) {
          digitalWrite(power, LOW); //電源OFF
          delay(10); //ちょっと待機
          digitalWrite(power, HIGH); //電源ON
          skip = true;
          
          state = 3;
        }
        else if (L != 0) {
          countOneP++;
        
          if (countOneP >3) {
            countOneP = 0;
            state = 5;
          }
          else
            state = 3;
        }
      }
      break;

    case 5://黒を検知するまで直進
      motorL_G = 200;
      motorR_G = 200;
      if (  identifyColorBlack( 3 ) == 1 || steadyState(1500) == 1) {
        state = 6;
      }
      break;

    case 6:
      motorL_G = -100;
      motorR_G = -100;
     
      if (steadyState(1000) == 1) {//計測中
        state = 3;
      }
      break;
  }
}




