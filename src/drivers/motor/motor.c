/*******************************************

motor.c

A library for smartcar Motor control

Authored by John Ching
for HKUST SmartCar team 2013

*******************************************/

void motor_execute_left(int motor_command){
  if (motor_command>0){
    gpio_set(PORTD,7,1);
  }else{
    gpio_set(PORTD,7,0);
    motor_command=motor_command*-1;
  }

  //deadzone
  motor_command+=60;

  //saturation protection
  if (motor_command>900){
  	motor_command=0;
  }
  FTM_PWM_Duty(FTM0,CH6,motor_command);

}

void motor_execute_right(int motor_command){
  if (motor_command>0){
    gpio_set(PORTE,11,1);
  }else{
    gpio_set(PORTE,11,0);
    motor_command=motor_command*-1;
  }
  //deadzone
  motor_command+=60;

  if (motor_command>900){
  	motor_command=0;
  }

  FTM_PWM_Duty(FTM0,CH5,motor_command);


}
