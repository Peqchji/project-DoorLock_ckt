#include <stdlib.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); 

// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 15 //D8
#define S1 13  //D7
#define S2 12  //D6
#define S3 14 //D5

// #define submit_button 0 //D3
#define lock_button  0 //D4 -> D3
#define solenoid_pin 2//D3 -> D4
#define pass_len 4

int j = 0;
char correct_password[pass_len + 1] = {'9', '8', '7', '6' , 0};

unsigned long debounceDelay = 50;
unsigned long init_time_unlock = 0;

// Submit Button Handling
int submitButtonState ;
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
    pinMode(solenoid_pin, OUTPUT);
    pinMode(lock_button, INPUT_PULLUP);

    Serial.begin(9600);

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    digitalWrite(solenoid_pin, HIGH);

    lcd.begin();
    lcd.backlight();
  //   lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  //   lcd.print("LCD1602 I2c Test"); //พิมพ์ข้อความ "LCD1602 I2c Test"
  //   lcd.setCursor(2, 1); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรกำแหน่งที3 แถวที่ 2 เตรียมพิมพ์ข้อความ
  // lcd.print("myarduino.net"); //พิมพ์ข้อความ "myarduino.net"

}

int is_correct(char *input)
{
  return (!strcmp(input, correct_password));
}

int  is_submit_button_press()
{
  int ret = 0;

  selectChannel(9);
  int reading = digitalRead(D0);
  // while (reading == 1)
  // {
  //   reading = digitalRead(D0);
  //   Serial.print("pressed");
  // }
  // int reading = digitalRead(submit_button);
  if (reading != submitLastButtonState) 
    submitLastDebounceTime = millis();

  if ((millis() - submitLastDebounceTime) > debounceDelay) {
    if (reading != submitButtonState) 
    {
      submitButtonState = reading;
      if (submitButtonState == HIGH) 
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
  digitalWrite(solenoid_pin, lock_state);\
  lcd.setCursor(0, 0); // กำหนดให้ เคอร์เซอร์ อยู่ตัวอักษรตำแหน่งที่0 แถวที่ 1 เตรียมพิมพ์ข้อความ
  lcd.clear();
  if (lock_state == LOW)
    lcd.print("UNLOCK"); //พิมพ์ข้อความ "LCD1602 I2c Test"
  else
    lcd.print("LOCK  ");

}

uint8_t sel;

void selectChannel(int i){
    sel = static_cast<uint8_t>(i) ^ 0b11111111;
    digitalWrite(S0, sel & 0b00000001);
    digitalWrite(S1, sel & 0b00000010);
    digitalWrite(S2, sel & 0b00000100);
    digitalWrite(S3, sel & 0b00001000);
}

int counter = 0;
int timer = 0;
char buffer[pass_len + 1] = {0, 0, 0, 0, 0};

int currentState[9] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW} ;
int lastState[9] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

int is_reset()
{
  int i;

  i = 0;
  while (i < 9)
  {
    if (currentState[i] == HIGH)
      return (0);
    i++;
  }
  return (1);
}

void    loop(void)
{
  // these loop are like a task to check all IR and fill recive value to buffer
  // selectChannel(counter);
  // Serial.println(digitalRead(D0));
  if (millis() - timer > 10) 
  {
    if (is_submit_button_press())
    {
      Serial.println("SUBMIT IS PRESSED");
      if (is_correct(buffer))
      {
        Serial.println("Password is Correct, The door is open");
        init_time_unlock = millis();
        digitalWrite(solenoid_pin, LOW); // open solenoid >> unlock
        lock_state = LOW; // unlock
      }
      else
      {
        Serial.println("Password is not Correct");
      }
      bzero(buffer, pass_len);  // reset buffer to make all buffer contain 0
    }
    timer = millis();
    selectChannel(counter);
    Serial.print(sel);
    Serial.print("status :");
    currentState[counter] = digitalRead(D0);
    Serial.println(currentState[counter]);
    // Serial.print(currentState[counter]);
    // int temp = digitalRead(D0);
    // Serial.print(temp);
    Serial.print(" Password: ");
    Serial.println(buffer);
    if (currentState[counter] == HIGH && currentState[counter] != lastState[counter])
    {
      buffer[j] = (char)((sel ^ 0b11111111) + '1');
      j = (j + 1) % 4;
    }
    lastState[counter] = currentState[counter];
    if(counter == 8)
    { 
      // if (is_reset())
      // {
      //   bzero(buffer, 5);
      //   j = 0;
      // }
      Serial.println();
      Serial.print(" LOCK_STATE: ");
      if (lock_state == HIGH)
        Serial.println("LOCK ");
      else
        Serial.println("UNLOCK ");  
      Serial.println("============================");
    }
    counter = (counter + 1 ) % 9;
  }
    

  // then will check if user submit the input and if it's correct -> unlock 
  // if (strlen(buffer) == 4)
  if (is_submit_button_press())
  {
    Serial.println("SUBMIT IS PRESSED");
    if (is_correct(buffer))
    {
      Serial.println("Password is Correct, The door is open");
      init_time_unlock = millis();
      digitalWrite(solenoid_pin, LOW); // open solenoid >> unlock
      lock_state = LOW; // unlock
    }
    else
    {
      Serial.println("Password is not Correct");
    }
    bzero(buffer, pass_len);  // reset buffer to make all buffer contain 0
  }

  //   // if the door is not open (close) and it's unlock more than 10s --> lock it
  if (lock_state == LOW && millis() - init_time_unlock > 20000)
  {
    digitalWrite(solenoid_pin, HIGH); // close solenoid >> lock
    lock_state = HIGH; // lock
  }
  inside_button_handling(); // set lock_state 
  applyLockState(); // apply lock_state to solenoid lock                                    <<<<< **** need test *****
}
