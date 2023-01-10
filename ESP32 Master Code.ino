#include <Wire.h>
#include <MPU6050_tockn.h>
#include <NewPing.h>

#define TRIGGER_PIN  33  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     25  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define I2C_SLAVE_ADDR 0x04 // 4 in hexadecimal

MPU6050 mpu6050(Wire);
int left , right , angle, flag=1;
float gyroz,distance=250;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() 
{
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  Wire.begin();   // join i2c bus (address optional for the master) - on the Arduino NANO the default I2C pins are A4 (SDA), A5 (SCL)
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop()
{
  if(flag)
  {
    goforwards();
    transmit(left,right,angle);
    delay(1000);
    stop();
    transmit(left,right,angle);
    delay(500);

    turn(1);
    transmit(left,right,angle);
    while(gyroz < 180)
    {
      mpu6050.update();
      gyroz=mpu6050.getAngleZ();
    }
    stop();
    transmit(left,right,angle);
    delay(500);

    gobackwards();
    transmit(left,right,angle);
    while(distance>10)
    {
      distance=sonar.ping_cm();
    }
    distance=250;
    stop();
    transmit(left,right,angle);
    delay(500);

    turn(0);
    transmit(left,right,angle);
    while(gyroz > 90)
    {
      mpu6050.update();
      gyroz=mpu6050.getAngleZ();
    }
    stop();
    transmit(left,right,angle);
    delay(500);
    
    gobackwards();
    transmit(left,right,angle);
    while(distance>10)
    {
      distance=sonar.ping_cm();
    }
    distance=250;
    stop();
    transmit(left,right,angle);
    flag=0;
  }
  else
    delay(100);
}

void gobackwards()
{
  left=-210;
  right=-210;
  angle=86;
}

void turn(int drctn)
{
  if(drctn)//turn left
  {
    angle=56;
    right=200;
    left=180;
  }
  if(!drctn)//turn right
  {
    angle=116;
    left=200;
    right=180;
  }
}

void goforwards()
{
  angle=86;
  right=255;
  left=255;
}

void stop()
{
  left=0;
  right=0;
  angle=86;
}

void transmit(int left,int right,int angle)
{
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device #4
  Wire.write((byte)((left & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(left & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((right & 0x0000FF00) >> 8));   
  Wire.write((byte)(right & 0x000000FF));          
  Wire.write((byte)((angle & 0x0000FF00) >> 8));    
  Wire.write((byte)(angle & 0x000000FF));
  Wire.endTransmission();   // stop transmitting
}
