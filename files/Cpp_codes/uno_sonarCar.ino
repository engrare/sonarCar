//For coding select: Arduino Duemilanove or Diecimila
#include <MotorWheel.h>
#include <Omni4WD.h>
#include <PID_Beta6.h>
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <SONAR.h>



#define SIZE 50
#define CARSPD 200
#define CARDELAY 250
#define MAPDIST 40



/*#define MAXCOMMAND 100
enum Commands { allStop, turnLeft, turnRight, goRight, goLeft, goBack, goAhead, LeftObstStop, RightObstStop, ForwardObstStop, BackwardObstStop};
Commands command_queue[MAXCOMMAND];
int queue_first = 0;
int queue_last = 0;
struct CommandData {
  Commands type;
  int speed;             // speed parameter (between 0-500)
  long time;           // time parameter (milisecond, -1: unlimited)
};*/


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
irqISR(irq1, isr1);  // Intterrupt function.on the basis of the pulse,work for wheel1
MotorWheel wheel1(3, 2, 4, 5, &irq1);
//This will create a MotorWheel object called Wheel1
//Motor PWM:Pin5, DIR:Pin4, Encoder A:Pin12, B:Pin13
irqISR(irq2, isr2);
MotorWheel wheel2(11, 12, 14, 15, &irq2);
irqISR(irq3, isr3);
MotorWheel wheel3(9, 8, 16, 17, &irq3);
irqISR(irq4, isr4);
MotorWheel wheel4(10, 7, 18, 19, &irq4);
Omni4WD Omni(&wheel1, &wheel2, &wheel3, &wheel4);

SONAR s11(0x11), s12(0x12), s13(0x13), s14(0x14);

int current_sonar = 1;
int new_X_coor = 0, new_Y_coor = 0;
int mission_type = 0;
int mapping_pos = 0;
int mapping_edge_count = 0;
bool map_start_part_1 = true, map_start_part_2 = true, map_start_part_3 = true, map_pre_start = true;
int minRollIndex = -1, minPitchIndex = -1;
int restricted_dir = 1;
int move_types[2] = {1, 2};
int move_index = 0;
int rist_dir_change;
int temp_rist_dir_change;
bool is_obst_exist = true;
bool set_slow_stop = false;





unsigned short distBuf[4];  //RIGHT - BACK - LEFT - FRONT
#define RIGHT 0
#define BACK 1
#define LEFT 2
#define FRONT 3



unsigned long currentTime;

char char_data;
String text_data = "";
char mission_data[SIZE];


int data_index = 0;

unsigned long time_for_bluetooth_send = 0;
unsigned long time_for_read_sonar = 0;
unsigned long time_for_regulate_PID = 0;
unsigned long time_for_mapping = 0;
unsigned long time_for_prestart = 0;
unsigned long time_for_mapping_delay = 0;
unsigned long time_for_distance_calc = 0;


typedef struct {
    float direction;   // Yön açısı (derece cinsinden olabilir)
    float distance; // Bu yönde gidilen mesafe (örneğin metre cinsinden)
} direction_info;

direction_info direction_matrix[50];

int current_position[2] = {MAPDIST, MAPDIST}; //x, y
int is_front_y;
int is_right_x;

int direction_index = 0;

void setup() {
  //myBT.begin(57600);
  
  Serial.println("started");
  //TCCR0B=TCCR0B&0xf8|0x01; // warning!! it will change millis()
  TCCR1B = TCCR1B & 0xf8 | 0x01;      // Pin9,Pin10 PWM 31250Hz
  TCCR2B = TCCR2B & 0xf8 | 0x01;      // Pin3,Pin11 PWM 31250Hz
  Omni.PIDEnable(0.26, 0.02, 0, 10);  // Enable PID
  SONAR::init();                      //set up some parameters
  delay(100);
}
void loop() {
  currentTime = millis();
  if (Serial.available()) {
    char_data = Serial.read();
    if (char_data == '|') {
      text_data += "|\0";
      if (text_data.substring(0, 2) == "ks" && text_data.length() == 4) {  //manuel go
        mission_type = 0;
        map_pre_start = true;
        map_start_part_1 = true;
        map_start_part_2 = true;
        map_start_part_3 = true;
        time_for_prestart = currentTime;

        char_data = (char)text_data.charAt(2);
        Serial.print("goRCV-");
        Serial.println(char_data);


        if (char_data == 'p' || Omni.getCarStat() != Omni4WD::STAT_STOP)
          allStop();

        if (char_data == 'f') {
          goAhead(CARSPD);
        } else if (char_data == 'b') {
          goBack(CARSPD);
        } else if (char_data == 'l') {
          goLeft(CARSPD);
        } else if (char_data == 'r') {
          goRight(CARSPD);
        } else if (char_data == 'q') {
          turnLeft(CARSPD);
        } else if (char_data == 'e') {
          turnRight(CARSPD);
        } else if (char_data == 'm') {
          mission_type = 1;
          mapping_pos = 0;
          distBuf[0] = 400;
          distBuf[1] = 400;
          distBuf[2] = 400;
          distBuf[3] = 400;
        }

      } else if (text_data.substring(0, 2) == "sk") {  //coordinate
        //coordiante code
        new_X_coor = text_data.substring(2, text_data.indexOf("a")).toInt() * 20;
        new_Y_coor = text_data.substring(text_data.indexOf("a") + 1, text_data.length() - 1).toInt() * 20;
        mission_type = 2;

        Serial.print("coorRCV (");
        Serial.print(new_X_coor);
        Serial.print(", ");
        Serial.print(new_Y_coor);
        Serial.println(")");
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
  if (time_for_regulate_PID + 10 < currentTime) {
    Omni.PIDRegulate();
    time_for_regulate_PID = millis();
  }

  if (time_for_bluetooth_send + 1000 < currentTime && false) {  //CHANGE IT TO WRITE DATA OF SONARS
    String data = "ks";
    for (int i = 0; i < 4; i++) {
      data += encryption_chars[round(distBuf[i] / 10)];
      //data += round(distBuf[i]/10);
      //data += ' ';
    }



    /*if(data.length() == 1)
      data += encryption_chars[0];*/

    Serial.println(data);

    time_for_bluetooth_send = millis();
    time_for_read_sonar = millis();
  }
  
  if (mission_type == 1) {  //mapping
    if (map_start_part_3) {
      if (map_pre_start) {
        if (time_for_prestart + 1000 < currentTime) {
          map_pre_start = false;
        }
      } else if (map_start_part_1) { //found which direction is closest
        minRollIndex = -1;
        minPitchIndex = -1;
        if (distBuf[RIGHT] > distBuf[LEFT]) {
          minPitchIndex = LEFT;
        } else {
          minPitchIndex = RIGHT;
        }
        if (distBuf[BACK] > distBuf[FRONT]) {
          minRollIndex = FRONT;
        } else {
          minRollIndex = BACK;
        }
        map_start_part_1 = false;
      } else if (map_start_part_2) { //go to closest Pitch
        if (minPitchIndex == RIGHT) {
          is_right_x = -1;
          goRight(CARSPD);
          if (distBuf[RIGHT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            map_start_part_2 = false;
          }
        } else if (minPitchIndex == LEFT) {
          is_right_x = 1;
          goLeft(CARSPD);
          if (distBuf[LEFT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            map_start_part_2 = false;
          }
        }

      } else if (map_start_part_3) { //go to closest Roll
        if (minRollIndex == FRONT) {
          is_front_y = -1;
          goAhead(CARSPD);
          if (distBuf[FRONT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            restricted_dir = minPitchIndex == RIGHT ? LEFT : RIGHT;
            rist_dir_change = BACK;
            temp_rist_dir_change = rist_dir_change;
            move_types[0] = 2;
            move_types[1] = minPitchIndex == RIGHT ? 7 : 8;
            map_start_part_3 = false;
          }
        } else if (minRollIndex == BACK) {
          is_front_y = 1;
          goBack(CARSPD);
          if (distBuf[BACK] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            restricted_dir = minPitchIndex == RIGHT ? LEFT : RIGHT;
            rist_dir_change = FRONT;
            temp_rist_dir_change = rist_dir_change;
            move_types[0] = 4;
            move_types[1] = minPitchIndex == RIGHT ? 11 : 12;
            map_start_part_3 = false;
          }
        }
      }
    } else if(time_for_mapping_delay + 1000 < currentTime) {
      if(set_slow_stop) {
        Omni.setCarSlow2Stop(CARDELAY);
        set_slow_stop = false;
        time_for_distance_calc = millis();
      }
      if (distBuf[rist_dir_change] < MAPDIST + 5) {
            Omni.setCarSlow2Stop(CARDELAY);
            
            if(rist_dir_change == RIGHT) {
              move_types[0] = 4;
              move_types[1] = 11;
              restricted_dir = LEFT;
              rist_dir_change = FRONT;
            } else if(rist_dir_change == BACK) {
              move_types[0] = 1;
              move_types[1] = 5;
              restricted_dir = FRONT;
              rist_dir_change = RIGHT;
            } else if(rist_dir_change == LEFT) {
              move_types[0] = 2;
              move_types[1] = 8;
              restricted_dir = RIGHT;
              rist_dir_change = BACK;
            } else if(rist_dir_change == FRONT) {
              move_types[0] = 3;
              move_types[1] = 10;
              restricted_dir = BACK;
              rist_dir_change = LEFT;
            }
            if(rist_dir_change == temp_rist_dir_change) {
              move_types[0] = 0;
              move_types[1] = 0;
            }
          }
      switch (move_types[move_index]) { //RIGHT - BACK - LEFT - FRONT
        case 0: //STOP
          break;
        case 1: //go RIGHT until see someting on RIGHT
          if (distBuf[RIGHT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            move_types[0] = distBuf[BACK] > distBuf[FRONT] ? 2 : 4;
            move_types[1] = move_types[0] == 2 ? 7 : 11;
            time_for_mapping_delay = millis();
            direction_matrix[direction_index].direction = RIGHT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc;
            time_for_distance_calc = millis();
            direction_index++;
            current_position[0] = current_position[0] + is_right_x*(millis() - time_for_distance_calc); //x, y

          } else {
            current_position[0] = current_position[0] + is_right_x*(millis() - time_for_distance_calc); //x, y
            is_front_y;
            is_right_x;
            goRight(CARSPD);
          }
          
          break;
        case 2: //go BACK until see someting on BACK
          if (distBuf[BACK] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            move_types[0] = distBuf[LEFT] > distBuf[RIGHT] ? 3 : 1;
            move_types[1] = move_types[0] == 3 ? 9 : 5;
            time_for_mapping_delay = millis();
            direction_matrix[direction_index].direction = BACK;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc;
            time_for_distance_calc = millis();
            direction_index++;
          } else {
            goBack(CARSPD);
          }

          break;
        case 3: //go LEFT until see someting on LEFT
          if (distBuf[LEFT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            move_types[0] = distBuf[BACK] > distBuf[FRONT] ? 2 : 4;
            move_types[1] = move_types[0] == 2 ? 8 : 12;
            time_for_mapping_delay = millis();
            direction_matrix[direction_index].direction = LEFT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc;
            time_for_distance_calc = millis();
            direction_index++;
          } else {
            goLeft(CARSPD);
          }

          break;
        case 4: //go FRONT until see someting on FRONT
          if(distBuf[FRONT] < MAPDIST) {
            Omni.setCarSlow2Stop(CARDELAY);
            move_types[0] = distBuf[LEFT] > distBuf[RIGHT] ? 3 : 1;
            move_types[1] = move_types[0] == 3 ? 10 : 6;
            time_for_mapping_delay = millis();
            direction_matrix[direction_index].direction = FRONT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc;
            time_for_distance_calc = millis();
            direction_index++;
          } else {
            goAhead(CARSPD);
          }

          break;
        case 5: //go RIGHT until don't see someting on BACK
          if(distBuf[BACK] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[BACK] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 2;
            move_types[1] = rist_dir_change == RIGHT ? 8 : 7;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = RIGHT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goRight(CARSPD);
          }

          break;
        case 6: //go RIGHT until don't see someting on FRONT
          if(distBuf[FRONT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[FRONT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 4;
            move_types[1] = rist_dir_change == RIGHT ? 12 : 11;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = RIGHT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goRight(CARSPD);
          }

          break;
        case 7: //go BACK until don't see someting on RIGHT
          if(distBuf[RIGHT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[RIGHT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 1;
            move_types[1] = rist_dir_change == BACK ? 6 : 5;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = BACK;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goBack(CARSPD);
          }

          break;
        case 8: //go BACK until don't see someting on LEFT
          if(distBuf[LEFT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[LEFT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 3;
            move_types[1] = rist_dir_change == BACK ? 10 : 9;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = BACK;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goBack(CARSPD);
          }

          break;
        case 9: //go LEFT until don't see someting on BACK
          if(distBuf[BACK] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[BACK] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 2;
            move_types[1] = rist_dir_change == LEFT ? 7 : 8;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = LEFT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goLeft(CARSPD);
          }

          break;
        case 10: //go LEFT until don't see someting on FRONT
          if(distBuf[FRONT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[FRONT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 4;
            move_types[1] = rist_dir_change == LEFT ? 11 : 12; //correct
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = LEFT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goLeft(CARSPD);
          }

          break;
        case 11: //go FRONT until don't see someting on RIGHT
          if(distBuf[RIGHT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[RIGHT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 1;
            move_types[1] = rist_dir_change == FRONT ? 5 : 6;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = FRONT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goAhead(CARSPD);
          }

          break;
        case 12: //go FRONT until don't see someting on LEFT
          if(distBuf[LEFT] < MAPDIST + 7)
            is_obst_exist = true;

          if (distBuf[LEFT] > MAPDIST + 5 && is_obst_exist) {
            move_types[0] = 3;
            move_types[1] = rist_dir_change == FRONT ? 9 : 10;
            time_for_mapping_delay = millis();
            is_obst_exist = false;
            set_slow_stop = true;
            direction_matrix[direction_index].direction = FRONT;
            direction_matrix[direction_index].distance = millis() - time_for_distance_calc + 1000;
            direction_index++;
          } else {
            goAhead(CARSPD);
          }

          break;
      }
      move_index = !move_index;
          
      
    }







  } else if (mission_type == 2) {  //go to coordinate
  }

  if (mission_type) {
    if (time_for_read_sonar + 200 < currentTime) {
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
      if (distBuf[current_sonar - 1] > 400) {
        distBuf[current_sonar - 1] = 400;
      }

      current_sonar++;
      time_for_read_sonar = millis();
    }
  }
}


void goAhead(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_BACKOFF) {
    Omni.setCarBackoff(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void goBack(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_ADVANCE) {
    Omni.setCarAdvance(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void goLeft(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_LEFT) {
    Omni.setCarLeft(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void goRight(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_RIGHT) {
    Omni.setCarRight(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void turnRight(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_ROTATERIGHT) {
    Omni.setCarRotateRight(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void turnLeft(unsigned int speedMMPS) {
  if (Omni.getCarStat() != Omni4WD::STAT_ROTATELEFT) {
    Omni.setCarRotateLeft(0);
    Omni.setCarSpeedMMPS(speedMMPS, CARDELAY);
  }
}

void allStop() {
  if (Omni.getCarStat() != Omni4WD::STAT_STOP) {
    Omni.setCarSlow2Stop(CARDELAY);
    Omni.setCarStop();
  }
}