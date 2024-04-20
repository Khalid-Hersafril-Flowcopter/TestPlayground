int motor_1_pin = 3;
int motor_2_pin = 5;
int motor_3_pin = 9;
int motor_4_pin = 10;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(motor_1_pin, OUTPUT);
  pinMode(motor_2_pin, OUTPUT);
  pinMode(motor_3_pin, OUTPUT);
  pinMode(motor_4_pin, OUTPUT);

}

int scale_fd_to_pwm(double Fd) {
  int pwm_val = Fd * 255;
  return pwm_val;
}

void loop() {
  // put your main code here, to run repeatedly:
  int pwm_val = scale_fd_to_pwm(0.1);
  analogWrite(motor_1_pin,  pwm_val);
  analogWrite(motor_2_pin,  pwm_val);
  analogWrite(motor_3_pin,  pwm_val);
  analogWrite(motor_4_pin,  pwm_val);

  Serial.println(pwm_val);
  
}
