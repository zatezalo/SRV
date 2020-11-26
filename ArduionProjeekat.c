#include <Arduino_FreeRTOS.h>

#define BUTTON 2
#define PERIODIC 7
#define APERIODIC 8
#define DEBOUNCE 2000

void printS(char *s){
  Serial.print(s);
  Serial.flush();
}

void setup() {
  Serial.begin(9600);
  pinMode(PERIODIC, OUTPUT);
  pinMode(APERIODIC, OUTPUT);
  pinMode(BUTTON, INPUT);
  set_print_str(&printS);
}

volatile int last_interrupt_time = 0;
char string[64];
int string_len = 0;
int names = 0;

void loop() {
  digitalWrite(PERIODIC,0);
  digitalWrite(APERIODIC,0);
  if(Serial.available()){
    portCHAR karakter = Serial.read();
    string[string_len++] = karakter;
    if(karakter == '\n'){
      string[string_len] = '\0';
      char *token = strtok(string, ";");
      
      char type = token[0];
      token = strtok(NULL, ";");
      char* name;
      name = pvPortMalloc(6*sizeof(char));
      strcpy(name, token);
      if(type == 'D') {
        deleteCeo(name);
      }
      else {
        token = strtok(NULL, ";");
        int arrivle = atoi(token);
        token = strtok(NULL, ";");
        int dOrP = atoi(token);
        token = strtok(NULL, ";");
        int duration = atoi(token);
        string_len = 0;
        if(type == 'P') {
          //P;joca;5;30;20
            xTaskCreatePeriodic(P_task, name, 100, name, 3, NULL, arrivle, dOrP, duration);
        }
        else if(type == 'A') {
          //A;ap;5;30;20
            xTaskCreateAperiodic(A_task, name, 100, name, 3, NULL, arrivle, dOrP, duration);
        }
    }
   }
  }
      
  attachInterrupt(digitalPinToInterrupt(BUTTON), button_task, RISING);
   //Serial.println("started"); 
}

void P_task(void* pvParameters)
{
  int task_start = xTaskGetTickCount();
  char* msg = (char*)pvParameters;
  int i;
  Serial.print("Start P-");
  Serial.flush();
  Serial.println(msg);
  Serial.flush(); 
  while(xTaskGetTickCount()<=task_start+20){
    digitalWrite(APERIODIC,0);
    digitalWrite(PERIODIC,1);
  }
  Serial.print("End ");
  Serial.flush();
  Serial.println(msg);
  Serial.flush();
  
  vTaskDeletePeriodic();
}

void button_task(){
  int interrupt_time = millis();
  Serial.println('a');
  if(interrupt_time - last_interrupt_time > DEBOUNCE){
    last_interrupt_time = interrupt_time;
    xTaskCreateAperiodic(A_task, "button", 100, "button", 3, NULL, 10, 30, 20);  
  }
}

void A_task(void* pvParameters)
{
  int task_start = xTaskGetTickCount();
  char* msg = (char*)pvParameters;
  int i;
  Serial.print("Start A-");
  Serial.flush();
  Serial.println(msg);
  Serial.flush(); 
  while(xTaskGetTickCount()<=task_start+20){
    digitalWrite(APERIODIC,1);
    digitalWrite(PERIODIC,0);
  }
  Serial.print("End ");
  Serial.flush();
  Serial.println(msg);
  Serial.flush();
  
  vTaskDelete(0);
}