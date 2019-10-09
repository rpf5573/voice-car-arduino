int maxPWM = 256 - 1;

// 몸체
int motor_1_pwm = 3;
int motor_1_in1 = 2;
int motor_1_in2 = 4;

// 어깨
int motor_2_pwm = 5;
int motor_2_in3 = A4;
int motor_2_in4 = A5;

// 오른쪽 바퀴
int motor_3_pwm = 6;
int motor_3_in1 = A2;
int motor_3_in2 = 0; // 바퀴는 다른 모터드라이브를 쓰기때문에 in2를 안쓴다

// 왼쪽 바퀴
int motor_4_pwm = 9;
int motor_4_in3 = 7;
int motor_4_in4 = 0; // 바퀴는 다른 모터드라이브를 쓰기때문에 in4를 안쓴다

// 팔꿈치
int motor_5_pwm = 10;
int motor_5_in1 = A1;
int motor_5_in2 = A0;

// 손
int motor_6_pwm = 11;
int motor_6_in3 = 12;
int motor_6_in4 = 13;

void setup() {  
  // motor driver 1
  pinMode(motor_1_pwm, OUTPUT);
  pinMode(motor_1_in1, OUTPUT);
  pinMode(motor_1_in2, OUTPUT);
  pinMode(motor_2_pwm, OUTPUT);
  pinMode(motor_2_in3, OUTPUT);
  pinMode(motor_2_in4, OUTPUT);

  // motor driver 2
  pinMode(motor_3_pwm, OUTPUT);
  pinMode(motor_3_in1, OUTPUT);
  pinMode(motor_3_in2, OUTPUT);
  pinMode(motor_4_pwm, OUTPUT);
  pinMode(motor_4_in3, OUTPUT);
  pinMode(motor_4_in4, OUTPUT);

  // motor driver 3
  pinMode(motor_5_pwm, OUTPUT);
  pinMode(motor_5_in1, OUTPUT);
  pinMode(motor_5_in2, OUTPUT);
  pinMode(motor_6_pwm, OUTPUT);
  pinMode(motor_6_in3, OUTPUT);
  pinMode(motor_6_in4, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  String str_cmd = "";

  while(Serial.available()) {
    char tmp_cmd = (char)Serial.read();
    if (tmp_cmd != '\r' && tmp_cmd != '\n') {
      Serial.println(tmp_cmd);
      str_cmd.concat(tmp_cmd); 
    }

    delay(10);
  }

  if (str_cmd == "") {
    return;
  }

  int param_1 = str_cmd.indexOf(",");
  int param_2 = str_cmd.indexOf(",", param_1+1);
  int req_len = str_cmd.length();
  if (param_2 == -1) {
    param_2 = req_len;
  }

  String motor = str_cmd.substring(0, param_1);
  String dir = str_cmd.substring(param_1+1, param_2);
  String motor_speed = str_cmd.substring(param_2+1, req_len);

  int pwm_speed = speed_to_pwm(motor_speed.toInt());

  if ( motor.equals("bottom") ) {
    motor_control(motor_3_in1, motor_3_in2, motor_3_pwm, "forward", 0);
    motor_control(motor_4_in3, motor_4_in4, motor_4_pwm, "forward", 0);
    delay(50);
    if ( dir.equals("right") ) {
       // 3,4번을 작동시키면된다잉
       motor_control(motor_3_in1, motor_3_in2, motor_3_pwm, "forward", 0);
       motor_control(motor_4_in3, motor_4_in4, motor_4_pwm, "forward", maxPWM);
    } 
    else if ( dir.equals("left") ) {
       // 3,4번을 작동시키면된다잉
       motor_control(motor_3_in1, motor_3_in2, motor_3_pwm, "forward", maxPWM);
       motor_control(motor_4_in3, motor_4_in4, motor_4_pwm, "forward", 0);
    } 
    // forward , backward , stop
    else {
      // 3,4번을 작동시키면된다잉
      motor_control(motor_3_in1, motor_3_in2, motor_3_pwm, dir, pwm_speed);
      motor_control(motor_4_in3, motor_4_in4, motor_4_pwm, dir, pwm_speed);
    }
    return;
  }

  if ( motor.equals("arm") && dir.equals("stop") ) {
    // 2, 5
    motor_control(motor_2_in3, motor_2_in4, motor_2_pwm, "stop", 0);
    motor_control(motor_5_in1, motor_5_in2, motor_5_pwm, "stop", pwm_speed);
    return;
  }

  // motor control individually
  if ( motor.equals("motor-1") ) {
    motor_control(motor_1_in1, motor_1_in2, motor_1_pwm, dir, pwm_speed);
  }
  else if ( motor.equals("motor-2") ) {
    // 2번 모터는 5번모터와 앱상에서 같은 페이지에 있기 때문에, 5번모터를 먼저 멈추고 그다음에 돌려야한다.
    motor_control(motor_5_in1, motor_5_in2, motor_5_pwm, "stop", 0);
    motor_control(motor_2_in3, motor_2_in4, motor_2_pwm, dir, pwm_speed);
  }
  else if ( motor.equals("motor-3") ) {
    motor_control(motor_3_in1, motor_3_in2, motor_3_pwm, dir, pwm_speed);
  }
  else if ( motor.equals("motor-4") ) {
    motor_control(motor_4_in3, motor_4_in4, motor_4_pwm, dir, pwm_speed);
  }
  else if ( motor.equals("motor-5") ) {
    // 2번 모터는 5번모터와 앱상에서 같은 페이지에 있기 때문에, 2번모터를 먼저 멈추고 그다음에 돌려야한다.
    motor_control(motor_2_in3, motor_2_in4, motor_2_pwm, "stop", 0);
    motor_control(motor_5_in1, motor_5_in2, motor_5_pwm, dir, pwm_speed);
  }
  else if ( motor.equals("motor-6") ) {
    motor_control(motor_6_in3, motor_6_in4, motor_6_pwm, dir, pwm_speed);
  }

  delay(10);
}

void motor_control(int motor_in1, int motor_in2, int motor_pwm, String dir, int speed) {
  if (dir.equals("forward")) {
    digitalWrite(motor_in1, HIGH);
    if ( motor_in2 != 0 ) { // 0 인경우에는 pwm / dir 이렇게 두핀만 쓰는것이기 때문에 굳이 LOW를 줄 필요가 없다
      digitalWrite(motor_in2, LOW); 
    }
    analogWrite(motor_pwm, speed);
  } 
  else if (dir.equals("backward")) {
    digitalWrite(motor_in1, LOW);
    if ( motor_in2 != 0 ) { // 0 인경우에는 pwm / dir 이렇게 두핀만 쓰는것이기 때문에 굳이 HIGH를 줄 필요가 없다
      digitalWrite(motor_in2, HIGH);
    }
    analogWrite(motor_pwm, speed);
  } 
  else if (dir == "stop") {
    analogWrite(motor_pwm, 0);
  }
}

int speed_to_pwm(int speed) {
  int value = map(speed, 0, 100, 0, maxPWM);
  return value;
}

void np(String a, int b) {
  a.concat(" : ");
  Serial.print(a);
  Serial.println(b);
}

void sp(String a, String b) {
  a.concat(" : ");
  Serial.print(a);
  Serial.println(b);
}