#include "include\header.hpp"
#include "src\IR_node.cpp"
#include "scr\IR_node_task.cpp"


enum pad_state {
  SLEEP, ACTIVE 
}
int IR__pad_state = 0;

int correct_password[4] = {1, 2, 3, 4};
int input_password[4] = {0, 0, 0, 0};

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
  }

IRNode_Task ir_task[10];

#define outdoor_button 14
#define lock_button 12

void setup(void){
    WiFi.forceSleepBegin();
    
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
    
    for(int i = 0; i < 10; i++){ //create IR handler task
      ir_task[i] = IRNode_Task(i, IRNode);
      Scheduler.start(&ir_task[i]);
    }
    Scheduler.begin();
}

int is_correct(int input[4])
{
  int i = 0;
  while (i < 4)
  {
    if (correct_password[i] != input_password[j])
      return (0)
  }
  return (1)
}


void selectChannel(int i){
  digitalWrite(S0, channel[i][0]);
  digitalWrite(S1, channel[i][1]);
  digitalWrite(S2, channel[i][2]);
  digitalWrite(S3, channel[i][3]);
}

enum solenoid_state {
  LOCK, UNLOCK
}

void loop(void)
{
  head_loop:
  outdoor_status = digitalRead(outdoor_button);
  if(outdoor_status == HIGH and IR__pad_state == SLEEP){
    IR__pad_state = ACTIVE
  }
  
  if(IR__pad_state == SLEEP) {
    goto head_loop;
  }
  
}