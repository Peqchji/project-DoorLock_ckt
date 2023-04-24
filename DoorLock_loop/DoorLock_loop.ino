
// Define Input port of esp8266 that connect to Signal port of mux
#define D0 16

// Define signal send port from esp8266 to mux 
#define S0 2
#define S1 3
#define S2 3
#define S3 4

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

byte channel[11][4] = {
  {0,0,0,0},  //  chnl0 -> ---
  {1,0,0,0},  //  chnl1 -> num1
  {0,1,0,0},  //  chnl2 -> num2
  {1,1,0,0},  //  chnl3 -> num3
  {0,0,1,0},  //  chnl4 -> num4
  {1,0,1,0},  //  chnl5 -> num5
  {0,1,1,0},  //  chnl6 -> num6
  {1,1,1,0},  //  chnl7 -> num7
  {0,0,0,1},  //  chnl8 -> num8
  {1,0,0,1},  //  chnl9 -> num9
  {0,1,0,1}   //  chnl10 -> sensor
  }

void selectChannel(int i){
  digitalWrite(S0, channel[i][0]);
  digitalWrite(S1, channel[i][1]);
  digitalWrite(S2, channel[i][2]);
  digitalWrite(S3, channel[i][3]);
}


int correct_password[4] = {1, 2, 3, 4};
int input_password[4] = {0, 0, 0, 0};

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



void    loop(void)
{
    int i = 1;
    int recieve_signal;

    int is_open = 0; //i don't know 0,1 of the sensor is true or false >> please correct this

    int j = 0;
    selectChannel(10);
    is_open = digitalRead(D0);
    while (!is_open && i < 10)
    {
        selectChannel(i);
        if(digitalRead(D0));
        {
          input_password[j] = i - 1;
          j++
        }
      i++;
    }

    if (is_correct(input_password))
      // door open
    else:
      serial.println("Password is NOT correct");
 
}
