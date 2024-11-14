//SoftwareSerial myBT(6, 13); // RX, TX
//6 ve 13 bt
// or
#include <MotorWheel.h>
#include <Omni4WD.h>
#include <PID_Beta6.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <SONAR.h>

#define SIZE 50
#define CARSPD 200

String encryption_chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{}~";
 // Include the header files 
/* 
 \ / 
 wheel1 \ / wheel4 
 Left \ / Right 
 
 
 
 
 / \ 
 wheel2 / \ wheel3 
 Right / \ Left 
 */ 
irqISR(irq1,isr1); // Intterrupt function.on the basis of the pulse,work for wheel1 
MotorWheel wheel1(3,2,4,5,&irq1); 
//This will create a MotorWheel object called Wheel1 
//Motor PWM:Pin5, DIR:Pin4, Encoder A:Pin12, B:Pin13 
irqISR(irq2,isr2); 
MotorWheel wheel2(11,12,14,15,&irq2); 
irqISR(irq3,isr3); 
MotorWheel wheel3(9,8,16,17,&irq3); 
irqISR(irq4,isr4); 
MotorWheel wheel4(10,7,18,19,&irq4); 
Omni4WD Omni(&wheel1,&wheel2,&wheel3,&wheel4); 

SONAR s11(0x11),s12(0x12),s13(0x13),s14(0x14);

int current_sonar = 1;

unsigned short distBuf[4];

unsigned long currentTime;

char char_data;
String text_data = "";
char mission_data[SIZE];

 
int data_index = 0;

unsigned long time_for_bluetooth_send = 0;
unsigned long time_for_read_sonar = 0;



void setup() { 
  //myBT.begin(57600);
  Serial.println("started");
  //TCCR0B=TCCR0B&0xf8|0x01; // warning!! it will change millis() 
  TCCR1B=TCCR1B&0xf8|0x01; // Pin9,Pin10 PWM 31250Hz 
  TCCR2B=TCCR2B&0xf8|0x01; // Pin3,Pin11 PWM 31250Hz 
  Omni.PIDEnable(0.31,0.01,0,10); // Enable PID
  SONAR::init();      //set up some parameters 
  delay(100);    //100 millissecond
  s11.trigger();
  delay(100);
 } 
void loop() {
  currentTime = millis();
  if (Serial.available()) {
    char_data = Serial.read();
    if(char_data == '|') {
      text_data += "|\0";
      if(text_data.substring(0, 2) == "mg" && text_data.length() == 4) { //manuel go
        char_data = (char)text_data.charAt(2);
        if(char_data == 'f') {
          goAhead(CARSPD);
          Omni.PIDRegulate();
        } else if(char_data == 'b') {
          goBack(CARSPD);
          Omni.PIDRegulate();
        } else if(char_data == 'l') {
          goLeft(CARSPD);
          Omni.PIDRegulate();
        } else if(char_data == 'r') {
          goRight(CARSPD);
          Omni.PIDRegulate();
        } else if(char_data == 'p') {
          allStop(CARSPD);
          Omni.PIDRegulate();
        }
      } else if(text_data.substring(0, 2) == "co" && text_data.length() == 5) { //coordinate
        //coordiante code
      
      }
      text_data = "";
    } else {
      text_data += char_data;
    }
  }

  /*funcs:
  Omni.setMotorAllBackoff(0); //locks wheels
  Omni.setCarLeft(0); //goes to directly left
  Omni.setCarRight(0); //goes to directly right
  Omni.setCarRotateLeft(0); //car turns to left from z axis
  Omni.setCarRotateRight(0); //car turns to right from z axis
  Omni.setCarAdvance(0); // goes to directly back
  Omni.setCarBackoff(0); //goes to directly forward


  */


  if(time_for_bluetooth_send + 1000 < currentTime) {
    String data = "ks";
    for(int i = 0; i < 4; i++) {
      data += encryption_chars[round(distBuf[i]/10)];
      //data += round(distBuf[i]/10);
      //data += ' ';
    }

      

    /*if(data.length() == 1)
      data += encryption_chars[0];*/

    Serial.println(data);

    time_for_bluetooth_send = millis();
    time_for_read_sonar = millis();
  }

  if(time_for_read_sonar + 200 < currentTime) {
    switch (current_sonar) {
      case 5:
        current_sonar = 1;
      case 1:
        distBuf[0] = int(s11.getDist());
        s12.trigger();
        break;

      case 2:
        distBuf[1] = int(s12.getDist());
        s13.trigger();
        break;

      case 3:
        distBuf[2] = int(s13.getDist());
        s14.trigger();
        break;

      case 4:
        distBuf[3] = int(s14.getDist());
        s11.trigger();
        break;      
    }
    if(distBuf[current_sonar - 1] > 400) {
      distBuf[current_sonar - 1] = 400;
    }

    current_sonar++;
    time_for_read_sonar = millis();
  }
}

 void goAhead(unsigned int speedMMPS){            // Car moves advance 
    if(Omni.getCarStat()!=Omni4WD::STAT_BACKOFF) Omni.setCarSlow2Stop(300); 
        Omni.setCarBackoff(0);              // If the car’s state is not advance.stop it 
            // else moves advance continue 
        Omni.setCarSpeedMMPS(speedMMPS, 300);    // Set the car speed at 300 
} 

void goBack(unsigned int speedMMPS){            // Car moves advance 
    if(Omni.getCarStat()!=Omni4WD::STAT_ADVANCE) Omni.setCarSlow2Stop(300); 
        Omni.setCarAdvance(0);                   // If the car’s state is not advance.stop it 
            // else moves advance continue 
        Omni.setCarSpeedMMPS(speedMMPS, 300);    // Set the car speed at 300 
} 

 void goLeft(unsigned int speedMMPS){ 
    if(Omni.getCarStat()!=Omni4WD::STAT_LEFT) Omni.setCarSlow2Stop(300); 
        Omni.setCarLeft(0); 
        Omni.setCarSpeedMMPS(speedMMPS, 300); 
} 
 
void goRight(unsigned int speedMMPS){ 
    if(Omni.getCarStat()!=Omni4WD::STAT_RIGHT) Omni.setCarSlow2Stop(300); 
        Omni.setCarRight(0); 
        Omni.setCarSpeedMMPS(speedMMPS, 300); 
} 
 
void turnRight(unsigned int speedMMPS){ 
    if(Omni.getCarStat()!=Omni4WD::STAT_ROTATERIGHT) Omni.setCarSlow2Stop(300); 
        Omni.setCarRotateRight(0); 
        Omni.setCarSpeedMMPS(speedMMPS, 300); 
} 
 
void turnLeft(unsigned int speedMMPS){ 
    if(Omni.getCarStat()!=Omni4WD::STAT_ROTATELEFT) Omni.setCarSlow2Stop(300); 
        Omni.setCarRotateLeft(0); 
        Omni.setCarSpeedMMPS(speedMMPS, 300); 
} 
 
void allStop(unsigned int speedMMPS){ 
    if(Omni.getCarStat()!=Omni4WD::STAT_STOP) Omni.setCarSlow2Stop(300); 
        Omni.setCarStop(); 
}
