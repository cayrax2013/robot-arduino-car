// Обновленный скетч для совместного IR & BT управления машинкой на базе arduino UNO
// Для языкового лагеря EnjoyCamp 2018
// https://enjoy-camp.ru/
// Преподаватель Сидов Д.А. 

#include <IRremote.h>


#define RECV_PIN 10 // IR пин

#define IR_Go      0x00FF629D
#define IR_Back    0x00ffa857
#define IR_Left    0x00ffc23d
#define IR_Right   0x00ff22dd
#define IR_Stop    0x00ff02fd
#define IR_ESC     0x00ff52ad

#define Lpwm_pin  3    // ускорение В --ENB
#define Rpwm_pin  9    // ускорение А --ENA
#define pinLB 5        // переднее правое колесо --IN3
#define pinLF 4        // заднее левое колесо --IN4
#define pinRB 7        // заднее правое колесо --IN1
#define pinRF 6        // переднее правое колесо --IN2

#define Lpwm_val 200   // значение ШИМ
#define Rpwm_val 200

unsigned char Bluetooth_val;       // переменная для передачи BT

IRrecv irrecv(RECV_PIN);
decode_results results;


int Car_state = 0;            // отображает состояние машинки (для отладки)
void M_Control_IO_config(void)
{
  pinMode(pinLB, OUTPUT); // pin2
  pinMode(pinLF, OUTPUT); // pin4
  pinMode(pinRB, OUTPUT); // pin7
  pinMode(pinRF, OUTPUT); // pin8
  pinMode(Lpwm_pin, OUTPUT); // pin11 (PWM)
  pinMode(Rpwm_pin, OUTPUT); // pin10 (PWM)
}
void Set_Speed(unsigned char Left, unsigned char Right)
{
  analogWrite(Lpwm_pin, Left);
  analogWrite(Rpwm_pin, Right);
}

void advance()     // движение вперед
{
  digitalWrite(pinRB, LOW); 
  digitalWrite(pinRF, HIGH);
  digitalWrite(pinLB, LOW); 
  digitalWrite(pinLF, HIGH);
  Car_state = 1;
}
void turnR()        // поворот направо 2мя парами
{
  digitalWrite(pinRB, LOW); 
  digitalWrite(pinRF, HIGH);
  digitalWrite(pinLB, HIGH);
  digitalWrite(pinLF, LOW);
  Car_state = 4;
}
void turnL()        // поворот налево 2мя парами
{
  digitalWrite(pinRB, HIGH);
  digitalWrite(pinRF, LOW );
  digitalWrite(pinLB, LOW);  
  digitalWrite(pinLF, HIGH);
  Car_state = 3;
}
void stopp()         // остановка
{
  digitalWrite(pinRB, HIGH);
  digitalWrite(pinRF, HIGH);
  digitalWrite(pinLB, HIGH);
  digitalWrite(pinLF, HIGH);
  Car_state = 5;
}
void back()          // движение назад
{
  digitalWrite(pinRB, HIGH); 
  digitalWrite(pinRF, LOW);
  digitalWrite(pinLB, HIGH);
  digitalWrite(pinLF, LOW);
  Car_state = 2;
}

void IR_Control(void)
{
  unsigned long Key;

  while (Key != IR_ESC)
  {
    if (irrecv.decode(&results)) // проверка наличия входящих данных
    {
      Key = results.value;
      switch (Key)
      {
        case IR_Go: advance();  // вперед
          break;
        case IR_Back: back();   // назад
          break;
        case IR_Left: turnL();  // налево
          break;
        case IR_Right: turnR(); // направо
          break;
        case IR_Stop: stopp();  // остановиться
          break;
        default:
          break;
      }
      irrecv.resume(); // ожидаем следующей передачи
    }
  }
  stopp();
}

void setup()
{
  M_Control_IO_config();
  Set_Speed(Lpwm_val, Rpwm_val);
  irrecv.enableIRIn(); // старт ИК
  Serial.begin(9600);   // старт BT посредством серийника (9600)
  stopp();
}

void loop()
{

  if (Serial.available()) // проверка наличия входящих данных
  {
    Bluetooth_val = Serial.read(); // чтение данных, приходящих по BT
    //Serial.println(Bluetooth_val);
         
    switch (Bluetooth_val)
    {
       case 'A':advance(); // вперед
       break;
       case 'C': back();   // назад
       break;
       case 'B':turnL();   // налево
       break;
       case 'D':turnR();   // направо
       break;
       case 'G':stopp();   // остановка
       break;   
    }
  }

  if (irrecv.decode(&results)) {
    //Serial.println(results.value,HEX);
    if (results.value == IR_Stop )IR_Control();
    irrecv.resume(); // ожидаем следующего значения
  }
}
