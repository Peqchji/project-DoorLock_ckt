#include <stdlib.h>


// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 2
#define S1 3
#define S2 3
#define S3 4

#define submit_button 14
#define lock_button 12
#define solenoid_pin 13
#define pass_len 4

char correct_password[pass_len] = {1, 2, 3, 4};

unsigned long debounceDelay = 50;

// Submit Button Handling
int submitButtonState;
int submitLastButtonState = LOW; 
unsigned long submitLastDebounceTime = 0;

// Lock button Handling
int lock_state = HIGH;
int lockButtonState;
int lockLastButtonState = HIGH; 
unsigned long lockLastDebounceTime = 0;

void    setup(void)
{
    pinMode(D0, INPUT);
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
}

void selectChannel(int i){
  digitalWrite(S0,  (i >> 3) & 1);
  digitalWrite(S1,  (i >> 2) & 1);
  digitalWrite(S2,  (i >> 1) & 1);
  digitalWrite(S3,  (i >> 0) & 1);
}

int is_correct(int input[pass_len])
{
  int i = 0;
  while (i < pass_len)
  {
    if (correct_password[i] != input_password[j])
      return (0)
  }
  return (1)
}

int  is_submit_button_press()
{
  int ret = 0;
  int reading = digitalRead(submit_button);
  if (reading != submitButtonState) 
    submitLastDebounceTime = millis();

  if ((millis() - submitLastDebounceTime) > debounceDelay) {
    if (reading != submitButtonState) 
    {
      submitButtonState = reading;
      if (buttonState == HIGH) 
      {
        j = 0;
        ret = ~ret
      }
    }
  }
  submitLastButtonState = reading;
  return (ret)
}

void  inside_button_handling()
{
  int reading = digitalRead(lock_button);
  if (reading != lockButtonState) 
    lockLastDebounceTime = millis();

  if ((millis() - lockLastDebounceTime) > debounceDelay) {
    if (reading != lockButtonState) 
    {
      lockButtonState = reading;
      if (lockBttonState == HIGH)
        lock_state = ~lock_state;     
    }
  }
  lockLastButtonState = reading;
}

void  applyLockState()
{
  digitalWrite(solenoid_pin, lock_state);
}

int isOpen()
{
  selectChannel(10);
  return (digitalRead(D0))
}

int j = 0;

void    loop(void)
{
    char buffer[pass_len] = {0, 0, 0, 0};
    int is_open = isOpen(); //i don't know 0,1 of the sensor is true or false >> please correct this

    int i = 1;

  // these loop are like a task to check all IR and fill recive value to buffer
    while (!is_open && i < 10)
    {
        selectChannel(i);
        if(!digitalRead(D0)); // have '!' bacause it's PULL UP 
        {
          buffer[j] = i;
          j++;
        }
      i++;
    }

    // then will check if user submit the input and if it's correct -> unlock 
    if (is_submit_button_press())
    {
      if (is_correct(buffer))
      {
        int init_time_open = millis();
        digitalWrite(solenoid_pin, HIGH); // open solenoid >> unlock
        lock_state = HIGH; // unlock
      }
      else
      {
        Serial.println("Password is not Correct");
      }
      bzero(buffer); // reset buffer to make all buffer contain 0
    }

    // if the door is open and it's open more than 10s --> lock it
    if (is_open && millis() - init_time_open > 10000)
    {
      digitalWrite(solenoid_pin, LOW); // close solenoid >> lock
      lock_state = LOW; // lock
    }

    inside_button_handling(); // set lock_state 
    applyLockState(); // apply lock_state to physical lock
}
