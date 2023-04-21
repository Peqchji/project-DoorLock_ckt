#include <ESP8266WiFi.h>
#include "src/IR_node_task.cpp"
// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 5
#define S1 4
#define S2 0
#define S3 2

#define password_lenght 4

#define outdoor_button 14
#define lock_button 12

enum pad_state {
  SLEEP, ACTIVE 
};
enum solenoid_state {
  LOCK, UNLOCK
};

unsigned int active_timer = 0;

int IR_pad_state = LOW;
int outdoor_status = LOW;

int correct_password[password_lenght] = {1, 2, 3, 4};
int input_password[password_lenght] = {0, 0, 0, 0};
int current_digit = 0;

byte channel[11][4] = {
  {0, 0, 0, 0},  //  chnl0 -> ---
  {1, 0, 0, 0},  //  chnl1 -> num1
  {0, 1, 0, 0},  //  chnl2 -> num2
  {1, 1, 0, 0},  //  chnl3 -> num3
  {0, 0, 1, 0},  //  chnl4 -> num4
  {1, 0, 1, 0},  //  chnl5 -> num5
  {0, 1, 1, 0},  //  chnl6 -> num6
  {1, 1, 1, 0},  //  chnl7 -> num7
  {0, 0, 0, 1},  //  chnl8 -> num8
  {1, 0, 0, 1},  //  chnl9 -> num9
  {0, 1, 0, 1}   //  chnl10 -> sensor
};

int is_correct(int input[4])
{
  int i = 0;
  while (i < 4)
  {
    if (correct_password[i] != input_password[i])
      return (0);
  }
  return (1);
}


int selectChannel(int i){
  digitalWrite(S0, channel[i][0]);
  digitalWrite(S1, channel[i][1]);
  digitalWrite(S2, channel[i][2]);
  digitalWrite(S3, channel[i][3]);
  return 0;
}

IRNode_Task ir_task[10];

void setup(void){
    WiFi.forceSleepBegin();
    Serial.begin(9600);
    
    pinMode(D0, INPUT);
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    pinMode(outdoor_button, INPUT);
    pinMode(lock_button, INPUT);

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    
    for(int num = 0; num < 10; num++){ //create IR handler task
      ir_task[num].init(num, &selectChannel);
      Scheduler.start(&ir_task[num]);
    }
    Scheduler.begin();
}

void loop(void)
{
  unsigned int timer = millis();

  outdoor_status = digitalRead(outdoor_button);
  if(outdoor_status == HIGH && IR_pad_state == SLEEP){
    IR_pad_state = ACTIVE;
    active_timer = millis();
  }
  
  if(outdoor_status == LOW && timer - active_timer > 30*1000){
    IR_pad_state = SLEEP;
    for(int idx = 0; idx < password_lenght; idx++){ //reset input
      input_password[idx] = 0;
    }
    active_timer = 0;
  }
  
  ////////////////////////////////////////////////////////
  
  if(IR_pad_state == SLEEP) {
    return;
  }
  
  ////////////////////////////////////////////////////////
  
  if(outdoor_status == HIGH && IR_pad_state == ACTIVE){
    is_correct(input_password);
  }
}