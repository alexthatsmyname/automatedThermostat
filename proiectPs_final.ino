#include <PID_v1.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BUTTON_OK 6
#define BUTTON_CANCEL 7
#define BUTTON_PREV 8
#define BUTTON_NEXT 9
#define temperaturaPin A0
#define mosfetPin 10

unsigned long prevMillis=0;
unsigned long prevProcessMillis=0;
unsigned long timp_incalzire=40000;
unsigned long timp_mentinere=20000;
unsigned long timp_racire=50000;

bool incalzire=false;
bool mentinere=false;
bool racire=false;

float tempC;
float targetTemp;
float tempIncreaseMs;
double error;
double dErr;
double timeChange;
unsigned long CurrentMillis=0;

enum Buttons {
  OK,
  CANCEL,
  PLUS,
  MINUS,
  EV_NONE,
  EV_MAX_NUM
};

enum Menus {
  MAIN_MENU,
  SET_TEMPERATURA,
  SET_TIMP_INCALZIRE,
  SET_TIMP_MENTINERE,
  SET_TIMP_RACIRE,
  SET_KP,
  SET_KI,
  SET_KD,
  MENU_MAX_NUM
};

Menus scroll_menu=MAIN_MENU;
Menus current_menu=MAIN_MENU;

void state_machine(enum Menus menu, enum Buttons button);
Buttons GetButtons(void);
void afisareDisplay(enum Menus menu,float temp_curr);

typedef void(state_machine_handler_t)(void);

unsigned long lastTime;
double Output, T_set=25;
double errSum, lastErr;
double Kp=1,Ki=0.1,Kd=0.1;

void saveToEEPROM(){
  EEPROM.put(0, T_set);
  EEPROM.put(sizeof(double),Kp);
  EEPROM.put(sizeof(double)*2,Ki);
  EEPROM.put(sizeof(double)*3,Kd);
  EEPROM.put(sizeof(unsigned long)*4,timp_incalzire);
  EEPROM.put(sizeof(unsigned long)*6,timp_racire);
  EEPROM.put(sizeof(unsigned long)*5,timp_mentinere);

}

void loadFromEEPROM(){
  EEPROM.get(0, T_set);
  EEPROM.get(sizeof(double),Kp);
  EEPROM.get(sizeof(double)*2,Ki);
  EEPROM.get(sizeof(double)*3,Kd);
  EEPROM.get(sizeof(unsigned long)*4,timp_incalzire);
  EEPROM.get(sizeof(unsigned long)*6,timp_racire);
  EEPROM.get(sizeof(unsigned long)*5,timp_mentinere);

}

void setup() {
  loadFromEEPROM();
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(temperaturaPin, INPUT);
  pinMode(mosfetPin, OUTPUT);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);

}


void loop() {
  volatile Buttons event=GetButtons();
  CurrentMillis=millis();
  if(event !=EV_NONE){
  state_machine(current_menu,event);
  }
  afisareDisplay(scroll_menu,tempC);

  if(prevProcessMillis<CurrentMillis){
  int reading = analogRead(temperaturaPin);
  float voltage = reading * (5000 / 1024.0);
  //float temperature = voltage / 10;
  tempC=voltage/10;
  //incalzire
  if(tempC<T_set || (CurrentMillis-prevMillis<=timp_incalzire)){
    targetTemp=tempC+tempIncreaseMs*(CurrentMillis-prevMillis);
    if(targetTemp>T_set){
      targetTemp=T_set;
    }
    calculPID(targetTemp);

    digitalWrite(mosfetPin,Output);
    incalzire=true;
    mentinere=false;
    racire=false;
    if(CurrentMillis-prevMillis>=timp_incalzire){
      prevMillis=CurrentMillis;
    }
  }else if (tempC>=T_set && CurrentMillis-prevMillis>timp_incalzire && CurrentMillis-prevMillis<=timp_mentinere+timp_incalzire){
    //mentinerea temperaturii
    digitalWrite(mosfetPin,LOW);
    incalzire=false;
    mentinere=true;
    racire=false;
    if(CurrentMillis-prevMillis>=timp_mentinere+timp_incalzire){
      prevMillis=CurrentMillis;
    }
  }else if (CurrentMillis-prevMillis>timp_mentinere+timp_incalzire){
    //racire
    digitalWrite(mosfetPin,LOW);
    incalzire=false;
    mentinere=false;
    racire=true;
    if(CurrentMillis-prevMillis>=timp_mentinere+timp_incalzire+timp_racire){
      prevMillis=CurrentMillis;
    }
  }
  }
  prevProcessMillis=CurrentMillis;
  delay(300);
}

void calculPID(double Temp_curenta){
  timeChange=double (CurrentMillis-lastTime);//timpul intre 2 citiri de temperatura
  //calcularea erorilor
  error=T_set-Temp_curenta;// eroare de temperatura
  errSum+=(error*timeChange);//suma erorilor
  dErr=(error-lastErr)/timeChange;//rata de schimbare a erorii
  Output=Kp*error+Ki*errSum+Kd*dErr;//semnalul de iesire al pid
  //setare limite
  if(Output>254){
    Output=254;
  }else if (Output<0){
    Output=0;
  }
  Serial.print("Tset : ");
  Serial.println(T_set);
  Serial.print("Tcur : ");
  Serial.println(Temp_curenta);
  Serial.print("out : ");
  Serial.println(Output);
}

Buttons GetButtons(void){
  enum Buttons ret_val=EV_NONE;
  if(!bouncedRead(OK)){ret_val=OK;}
  if(!bouncedRead(CANCEL)){ret_val=CANCEL;}
  if(!bouncedRead(PLUS)){ret_val=PLUS;}
  if(!bouncedRead(MINUS)){ret_val=MINUS;}
  return ret_val;
}

int bouncedRead(int button){
  int getButton=digitalRead(button);
  return digitalRead(button) && getButton;
}

void afisareDisplay(enum Menus menu, float temp_curr){
  lcd.clear();
  switch(menu){

    case SET_TEMPERATURA:
    lcd.print("Tset : ");
    lcd.print(T_set);
    break;

    case SET_TIMP_INCALZIRE:
    lcd.print("Tinc : ");
    lcd.print(timp_incalzire);
    break;

    case SET_TIMP_MENTINERE:
    lcd.print("Tinc : ");
    lcd.print(timp_mentinere);
    break;

    case SET_TIMP_RACIRE:
    lcd.print("Trac : ");
    lcd.print(timp_racire);
    break;

    case SET_KP:
    lcd.print("Kp : ");
    lcd.print(Kp);
    break;

    case SET_KI:
    lcd.print("Ki : ");
    lcd.print(Ki);
    break;

    case SET_KD:
    lcd.print("Kd : ");
    lcd.print(Kd);
    break;
    case MAIN_MENU:
    default:
    lcd.print("Temp : ");
    lcd.print(T_set);
    lcd.setCursor(0,1);
    lcd.print("Tcur : ");
    lcd.print(temp_curr);
    break;
  }
  if(current_menu!=MAIN_MENU){
    lcd.setCursor(0, 1);
    lcd.print("Modifica !");
  }
}

void enter_menu(void){current_menu=scroll_menu;}
void go_home(void){
  scroll_menu=MAIN_MENU;
  current_menu=scroll_menu;
}
void go_next(void){
  scroll_menu=(Menus)((int)scroll_menu+1);
  scroll_menu=(Menus)((int)scroll_menu%MENU_MAX_NUM);
}

void go_prev(void){
  scroll_menu=(Menus)((int)scroll_menu-1);
  scroll_menu=(Menus)((int)scroll_menu%MENU_MAX_NUM);
}

void inc_kp(void){
  Kp++;
  saveToEEPROM();
}
void dec_kp(void){
  Kp--;
  saveToEEPROM();
}

void inc_ki(void){
  Ki++;
  saveToEEPROM();
}
void dec_ki(void){
  Ki--;
  saveToEEPROM();
}

void inc_kd(void){
  Kd++;
  saveToEEPROM();
}
void dec_kd(void){
  Kd--;
  saveToEEPROM();
}

void inc_temp(void){
  T_set++;
  saveToEEPROM();
}
void dec_temp(void){
  T_set--;
  saveToEEPROM();
}

void inc_timp_incalzire(void){
  timp_incalzire+=1000;
  saveToEEPROM();
}
void dec_timp_incalzire(void){
  timp_incalzire-=1000;
  saveToEEPROM();
}

void inc_timp_mentinere(void){
  timp_mentinere+=1000;
  saveToEEPROM();
}
void dec_timp_mentinere(void){
  timp_mentinere-=1000;
  saveToEEPROM();
}

void inc_timp_racire(void){
  timp_racire+=1000;
  saveToEEPROM();
}
void dec_timp_racire(void){
  timp_racire-=1000;
  saveToEEPROM();
}

state_machine_handler_t* sm[MENU_MAX_NUM][EV_MAX_NUM]={
  {enter_menu,go_home,go_next,go_prev},
  {go_home,go_home,inc_temp,dec_temp},
  {go_home,go_home,inc_kp,dec_kp},
  {go_home,go_home,inc_timp_incalzire,dec_timp_incalzire},
  {go_home,go_home,inc_timp_mentinere,dec_timp_mentinere},
  {go_home,go_home,inc_timp_racire,dec_timp_racire},
};

void state_machine(enum Menus menu, enum Buttons button){
  sm[menu][button]();
}
