#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>

// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16


// Define signal send port from esp8266 to mux 
#define S0 5
#define S1 4
#define S2 0
#define S3 2
i

#define password_lenght 4

#define outdoor_button 14
#define lock_button 12

enum pad_state {
  SLEEP, ACTIVE 
};
enum solenoid_state {
  LOCK = LOW, UNLOCK = HIGH
};

unsigned int active_timer = 0;

int IR_pad_state = pad_state::SLEEP;
int solenoid_status = solenoid_state::LOCK;
int outdoor_status = LOW;
int lock_button_status = LOW

unsigned int solenoid_timer = 0;

int correct_password[password_lenght] = {1, 2, 3, 4};
int input_password[password_lenght] = {0, 0, 0, 0};
int current_digit = 0;

int is_correct(int input[4])
{
  int i = 0;
  while (i < 4)
  {
    if (correct_password[i] != input_password[i]){
      return (0);
    }
  }
  return (1);
}

void selectChannel(int i){
  uint8_t channel = static_cast<uint8_t>(i) ^ 0b11111111;
  digitalWrite(S0, channel & 0b00000001);
  digitalWrite(S1, channel & 0b00000010);
  digitalWrite(S2, channel & 0b00000100);
  digitalWrite(S3, channel & 0b00001000);
}

class IRNode_Task : public LeanTask {
    private:
        int current_status;
        bool is_active;
        int node_num;
        int (*sel_func)(int);
        unsigned int task_timer = 0;
        
    public:
        IRNode_Task() { }
        
        int init(int num, int (*sel_func_args)(int)) {
            this->node_num = num;
            this->is_active = false;
            this->current_status = LOW;
            this->sel_func = sel_func_args;
            return 0;
        }
        
        void reset_props(){
            this->is_active = false;
            this->current_status = LOW;
        }
        
        bool get_is_active();
        int get_current_status();
        
    protected:
        void setup() {}
        
        void loop() {
            if(this->is_active){
                return;
            }
            sel_func(node_num);                       
            this->current_status = digitalRead(D0);
            if(this->current_status == HIGH && !this->is_active){
                this->is_active = true;
            }
            
        }
} ir_task[10];

void reset_IRNode_props(){
  for(int i =0; i < 10; i++){
    ir_task[i].reset_props();
  }
}

void setup(void){
    WiFi.forceSleepBegin();
    Serial.begin(9600);
    
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(solenoid_pin, OUTPUT);

    pinMode(D0, INPUT);
    pinMode(outdoor_button, INPUT);
    pinMode(lock_button, INPUT);

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    digitalWrite(solenoid_pin, LOW);
    
    for(int num = 0; num < 10; num++)  //create IR handler task
    {
      ir_task[num].init(num, &selectChannel);
      Scheduler.start(&ir_task[num]);
    }
    Scheduler.begin();
}

void loop(void)
{
  unsigned int timer = millis();
  
  int solenoid_status = digitalRead(solenoid_pin);
  int outdoor_status = digitalRead(outdoor_button); //อย่าลืม debrouce
  int lock_button_status = digitalRead(lock_button); //อย่าลืม debrouce
  
  if(lock_button_status == HIGH){
    digitalWrite(solenoid_pin, !solenoid_status);
  }
  
  if(outdoor_status == HIGH && IR_pad_state == SLEEP){
    IR_pad_state = pad_state::ACTIVE;
    active_timer = millis();
  }
  
  if(outdoor_status == LOW && timer - active_timer > 30*1000){
    IR_pad_state = pad_state::SLEEP;
    for(int idx = 0; idx < password_lenght; idx++){ //reset input
      input_password[idx] = 0;
    }
    active_timer = 0;
  }
  
  ////////////////////////////////////////////////////////
  
  if(IR_pad_state == pad_state::SLEEP) {
    return;
  }
  
  ////////////////////////////////////////////////////////
  
  if(outdoor_status == HIGH && IR_pad_state == pad_state::ACTIVE 
    && is_correct(input_password) && solenoid_status == solenoid_state::LOCK){
      digitalWrite(solenoid_pin, solenoid_state::UNLOCK);
      solenoid_timer = millis();
    }
  
  if(solenoid_status == UNLOCK && timer - solenoid_timer > 30*1000){
    reset_IRNode_props();
    digitalWrite(solenoid_pin, solenoid_state::LOCK);
  }
  
  
}