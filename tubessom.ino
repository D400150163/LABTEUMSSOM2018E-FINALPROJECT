#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <BH1750.h>
#include <Servo.h>
#define TRIG 8
#define ECHO 9
#define relay 3
#define servo 10
#define buz 2

BH1750 lightMeter;
Servo myservo;
int pos = 0;
long duration, distance;

// define three tasks for Blink, AnalogRead & DigitalRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );
void TaskDigitalRead( void *pvParameters );

void setup() {
  Serial.begin(9600);
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(servo,OUTPUT);
  pinMode(buz,OUTPUT);
  Wire.begin();
  lightMeter.begin();
  myservo.attach(10);
  
    while (!Serial) {
    ;
    }
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *)"Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskAnalogRead
    ,  (const portCHAR *) "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

  xTaskCreate(
    TaskDigitalRead
    ,  (const portCHAR *) "DigitalRead"
    ,  128  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}
void loop()
{
  // Empty. Things are done in Tasks.
}

void TaskBlink( void *pvParameters ) 
{
  (void) pvParameters;
  for (;;) // A Task shall never return or exit.
  {
        digitalWrite(buz,HIGH);
        vTaskDelay( 100 / portTICK_PERIOD_MS );
        digitalWrite(buz,LOW);    
        vTaskDelay( 10000 / portTICK_PERIOD_MS );
    } 
}
void TaskAnalogRead(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
    for (;;)
  {
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.println(lux);
    if (lux <= 10){
      digitalWrite(relay,HIGH);
    } else {
      digitalWrite(relay,LOW);
    }
    vTaskDelay(100);  // one tick delay (15ms) in between reads for stability
  }
  
} 
void TaskDigitalRead(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
   for (;;)
  {
    digitalWrite(TRIG,HIGH); delayMicroseconds(10);
    digitalWrite(TRIG,LOW); 

    duration = pulseIn(ECHO,HIGH);
  
    distance = duration /58.2;
    if (distance >=0 && distance <= 5){
        Serial.println("BAHAYA  ");
        myservo.write(135);
        delay(500);
        digitalWrite(relay,HIGH);
        digitalWrite(buz,HIGH);
        vTaskDelay( 100 / portTICK_PERIOD_MS );
        digitalWrite(relay,LOW);
        digitalWrite(buz,LOW);
        vTaskDelay( 50 / portTICK_PERIOD_MS );
        }    
        else {
        Serial.print("AMAN  ");
        Serial.println(distance);
        myservo.write(0);
        delay(500);
        }
  
  vTaskDelay(1); // wait for one second
  }
  
}
