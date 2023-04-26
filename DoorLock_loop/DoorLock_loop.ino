#include <stdlib.h>


// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 2
#define S1 0
#define S2 4
#define S3 5

#define submit_button 14
#define lock_button 12 // D6
#define solenoid_pin 13
#define pass_len 4

int j = 0;
char correct_password[pass_len + 1] = {'1', '2', '3', '4' , 0};

unsigned long debounceDelay = 50;
unsigned long init_time_unlock = 0;

// Submit Button Handling
int submitButtonState ;
int submitLastButtonState = HIGH; 
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
    pinMode(submit_button, INPUT_PULLUP);
    pinMode(solenoid_pin, OUTPUT);
    pinMode(lock_button, INPUT_PULLUP);

    Serial.begin(9600);

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
}

int is_correct(char *input)
{
  return (!strcmp(input, correct_password));
}

int  is_submit_button_press()
{
  int ret = 0;
  int reading = digitalRead(submit_button);
  if (reading != submitLastButtonState) 
    submitLastDebounceTime = millis();

  if ((millis() - submitLastDebounceTime) > debounceDelay) {
    if (reading != submitButtonState) 
    {
      submitButtonState = reading;
      if (submitButtonState == LOW) 
      {
        j = 0;
        ret = ~ret;
      }
    }
  }
  submitLastButtonState = reading;
  return (ret);
}

void  inside_button_handling()
{
  int reading = digitalRead(lock_button);
  if (reading != lockLastButtonState) 
    lockLastDebounceTime = millis();

  if ((millis() - lockLastDebounceTime) > debounceDelay) {
    if (reading != lockButtonState) 
    {
      lockButtonState = reading;
      if (lockButtonState == LOW)
      {
        if (lock_state == LOW)
          lock_state = HIGH;
        else
        {
          lock_state = LOW;
          init_time_unlock = millis();    
        }
        Serial.println("Hello from inside");
      }
    }
  }
  lockLastButtonState = reading;
}

void  applyLockState()
{
  digitalWrite(solenoid_pin, lock_state);
}

int sel;

void selectChannel(int i){
    sel = static_cast<uint8_t>(i) ^ 0b11111111;
    digitalWrite(S0, sel & 0b00000001);
    digitalWrite(S1, sel & 0b00000010);
    digitalWrite(S2, sel & 0b00000100);
    digitalWrite(S3, sel & 0b00001000);
}

int sss = 0;

int isOpen()
{
  selectChannel(10);
  return (digitalRead(D0));
}

int counter =1;
int timer = 0;
char buffer[pass_len + 1] = {0, 0, 0, 0, 0};

int currentState[9];
int lastState[9] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

void    loop(void)
{
    // int is_open = isOpen(); //i don't know 0,1 of the sensor is true or false >> please correct this
    int is_open = 0; // << ***********

  // these loop are like a task to check all IR and fill recive value to buffer
  if ( /* !is_open */ && millis() - timer > 100)
  {
    timer = millis();
    selectChannel(counter);
    // Serial.print(sel ^ 0b11111111);
    // Serial.print(" status: ");
    Serial.print(currentState[counter]); // don't move this line, it's very nesses
    currentState[counter] = digitalRead(D0);
    if (currentState[counter] == HIGH && currentState[counter] != lastState[counter])
    {
      buffer[j] = (char)((sel ^ 0b11111111) + '1');
      Serial.print(counter + 1);
      Serial.print("is Pressing | Password: ");
      Serial.print(buffer);
      Serial.println();
      j = (j + 1) % 4;
    }
    if(counter == 8)
    {   
      Serial.println();
      Serial.print(" LOCK_STATE: ");
      if (lock_state == HIGH)
        Serial.println("LOCK ");
      else
        Serial.println("UNLOCK ");  
      // Serial.println(buffer);
      Serial.println("============================");
    }
    lastState[counter] = currentState[counter];
    counter = (counter + 1 ) % 9;
  }
    

  // then will check if user submit the input and if it's correct -> unlock 
  if (is_submit_button_press())
  {
    Serial.println("SUBMIT IS PRESSED");
    if (is_correct(buffer))
    {
      Serial.println("Password is Correct, The door is open");
      init_time_unlock = millis();
      digitalWrite(solenoid_pin, HIGH); // open solenoid >> unlock
      lock_state = LOW; // unlock
    }
    else
    {
      Serial.println("Password is not Correct");
    }
    bzero(buffer, pass_len);  // reset buffer to make all buffer contain 0
  }
    // if the door is not open (close) and it's unlock more than 10s --> lock it
  if (/* !is_open && */lock_state == LOW && millis() - init_time_unlock > 10000)
  {
    digitalWrite(solenoid_pin, LOW); // close solenoid >> lock
    lock_state = HIGH; // lock
  }
  inside_button_handling(); // set lock_state 
  applyLockState(); // apply lock_state to solenoid lock                                    <<<<< **** need test *****
}
