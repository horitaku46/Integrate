

// 直進
void goStraight( int speed ) {
  motorL_G = speed;
  motorR_G = speed;
}


// 停止
void stopStay() {
  motorL_G = 0;
  motorR_G = 0;
}


// 右回転
void turnRight( int speed ) {
  motorL_G = speed;
  motorR_G = -speed;
}


// 左回転
void turnLeft( int speed ) {
  motorL_G = -speed;
  motorR_G = speed;
}


