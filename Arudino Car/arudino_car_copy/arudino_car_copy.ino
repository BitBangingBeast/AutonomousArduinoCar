#include <LiquidCrystal.h>
#include <Keypad.h>
#include "KeypadManager.h"
#include <Wire.h>
#include "LIDARLite_v4LED.h"
#include <EEPROM.h>



const int rs = 11, en = 12, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

KeypadManager keypadManager;
LIDARLite_v4LED myLIDAR; 

#define CMPS14_address 0x60
#define forwrad  1
#define backward  0
#define  X_port  A8
#define Y_port  A9
#define R_Motor_P_port  9
#define L_Motor_P_port  10
#define R_Motor_D_port  8
#define L_Motor_D_port  7
#define Channel_A  2
#define Channel_B  3

// joystick postion and reading.
int X_postion;
int Y_postion;
int X_reading;
int Y_reading;


// R for right -- L for left -- I for impulse. 
volatile int Channel_L_I_counter;
volatile int Channel_R_I_counter;

// time Buffer_varible.
int interval = 1000;
int time_passed = 0;

// coefficient for movment over X and Y axes.
int X_coefficient = 0;
int Y_coefficient = 0;

// left and right motor power. 
int L_Motor_P = 0;
int R_Motor_P = 0;

// R for right motor -- L for left motor.
int previous_R_postion = 0;
int previous_L_postion = 0;
int current_R_postion = 0; 
int current_L_postion = 0;

// motor rotaion 
int L_RPM = 0;
int R_RPM = 0;
int Average_speed = 0;
float total_Distance = 0;
int distance_counter = 0;
bool count_distance = true;
int Lidar_distance = 0;
int Distance_Buffer = 0;


int impulse_per_rotaion = 174;
float impulse_per_cm = 13; 
byte impulse_per_cm_lidar;
int freaz_camps;
int data;
int Dist = 0;
int travled_Dist = 0;
int starting_point = 0;

char key;
char save_direction;
int save_distance;
int angle;
bool printCamps = false;
bool remote_control = false;
bool printLidar = false;
bool Track = false;
bool set_buffer = false;
bool flow_track = false;
bool printStats = true;
bool calculate_area = false;
bool flow_track_bool;
bool go;
bool save_bool = false;
bool calibrate_bool = false;
int step;
int Buffer_var;
int buffer_var;
int direct_hold;
int Direct_Hold;
byte raw;
String message = "";



void move_to(int dist = 0, String dire = " "){
    
  int distance_list [20];
  String direction_list[20];

  for (int i = 0; i < 20; i++){

    distance_list[i] = 0;
    direction_list[i] = 'x';
  }
  if (dist != 0 && dire !=  " "){

    distance_list[0] = dist;
    direction_list[0] = dire;
    Serial.print(distance_list[0]);
    Serial.print(direction_list[0]);
  }
  char key = keypadManager.getKey();
      
  if (key == '#'){

    printCamps = !printCamps;
    
  }
  if (key == '0'){
    printStats = !printStats;
    remote_control = !remote_control;
    starting_point = myLIDAR.getDistance();
    Serial.println(remote_control);
    
  }
  if (key == 'A'){

    starting_point = myLIDAR.getDistance();
    printLidar = !printLidar;
  }

  if (key == 'B'){
    Distance_Buffer = 0;
    Serial.println("tracking");
    Track = !Track;
    flow_track_bool = false;
    printLidar = !printLidar;
    set_buffer = true;

  }
  if (key == 'C'){
    Serial.println("flowing track");
    step = 0;
    Distance_Buffer = 0;
    Track = !Track;
    printLidar = !printLidar;
    set_buffer = true;
    flow_track = true;
  }
   if (key == 'D'){
    Serial.println("Print Stats");
    remote_control = false;
    Track = false;
    printLidar = false;
    set_buffer = false;
    flow_track = false;
    printStats = true;
  }
   if (key == '1'){
    Serial.println("Area");
    remote_control = false;
    Track = false;
    printLidar = false;
    set_buffer = false;
    flow_track = false;
    printStats = false;
    calculate_area = !calculate_area;
  }
  if (key == '2'){
      go = true;
  }
  if (key == '3'){
      calibrate_bool = true;
  }
   if (key == '4'){
      save_bool = true;
  }
  
  if  (key == '*'){
    
    key = keypadManager.getKey();
   
    int i = 0;
    while (i < 20){
      int cursor = 14;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("add your next move.");
      lcd.setCursor(0, 1);
      lcd.print("# = next , * = exit.");
      lcd.setCursor(0, 2);
      lcd.print("set direction: ");
      do{

        key = keypadManager.getKey();
        lcd.setCursor(16, 2);
        if (isAlpha(key) && key != '#' && key != '*' ){
          lcd.print(key);
          direction_list[i] = String(key);
        }

        if (key == '#' || key == '*' ){
        break;
        }
      }while (true);

      lcd.setCursor(0, 3);
      lcd.print("set distance: ");
      int k = 10;
      do{
       lcd.setCursor(cursor, 3);
       key = keypadManager.getKey();
       
       if (isDigit(key) && key != '#' && key != '*' ){

         lcd.print(key);
         distance_list[i] = (key - 48) + distance_list[i]*k ;
         Serial.print("\n"); 
        
          cursor ++;
        }
      
      if (key == '#' || key == '*' ){
    
        break;
      }

      }while (true);
      i++;
      if (key == '*'){
        break;
      } 
    }
  }
  
    
    for (int i = 0; i < 20; i++){
     
      
      if (distance_list[i] == 0){
        break;
      }else{
      
      printCamps = true;
      int desired_impulses = (distance_list[i] * impulse_per_cm ) ;
              
     
      rotate(direction_list[i]);
      Direct_Hold = readAngle(CMPS14_address);
      delay(1000);
      int rest_theValue = Channel_R_I_counter;
      int PWM_P = 25;
      while (((Channel_R_I_counter - rest_theValue) < desired_impulses)  ) {
          if (((Channel_R_I_counter - rest_theValue) / desired_impulses) <= 0.8){
            if (PWM_P < 250){
              PWM_P += 5;
            } 
            move (PWM_P, 1, Direct_Hold);

          }else{
            move (40, 1, Direct_Hold);

          }
        }
      }
      printCamps = false;
    }
    
 
}

void track (bool flow_track_bool){
    if (flow_track_bool){
      Distance_Buffer = buffer_var;

    }else{
         buffer();
    }
    Direct_Hold = readAngle(CMPS14_address);
    while ( myLIDAR.getDistance() > Distance_Buffer && ((myLIDAR.getDistance() - Distance_Buffer) != 65535)){
      if ((myLIDAR.getDistance() - Distance_Buffer) > 5){
        
        move(140, 1,Direct_Hold);

      }else{

      move(30, 1,Direct_Hold);
      }
    }
    delay(150);

    while ( myLIDAR.getDistance() < Distance_Buffer){

      if ((myLIDAR.getDistance()- Distance_Buffer) < -5){
        move(140, 0,Direct_Hold);
 
      }else{
      move(30, 0,Direct_Hold);

      }
    }
    delay(200);
    if (flow_track){
      if (step == 0){
        rotate("C");
        step ++;
      }else{
        flow_track = false;
        Track = false;
        set_buffer = false;
      }
    }


}

void surface_calculator (){
  char key;
  bool area = true;
  bool volume = false;
  int x_y[2] = {0, 0};
  int z = 0;
  int index = 0;
  int Area = 0;
  int Volume = 0;
  String  rotation ;
  
while(true){
  int x_y[2] = {0, 0};
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("surface calculator");
  if (area){
   
    for (int i = 1; i < 4 ;i++){
      if (i == 2){
        index = 0;
        rotation = "C";
      }else if (i > 2){
        index = 1;
        rotation = "B";
      }else{
        index = 0;
        rotation = "B";

      }
        
      delay(500); 
      x_y[index] += myLIDAR.getDistance();
      rotate(rotation);
    
    }
      x_y[index] += myLIDAR.getDistance();
      x_y[0] -= 40;
      x_y[1] -= 40;
    
      Area = x_y[0] * x_y[1];
      if (Area < 0){
        Area = 0;
      }
      area = false;
    }
      if (volume){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("volume calculator");
        lcd.setCursor(1,0);
        lcd.print("point to the celling and '#'");
        do{
            key = keypadManager.getKey();
        } while (key != '#');
        z =  myLIDAR.getDistance() - 20;
        Volume = z * Area;
        volume = false;
      }

    
    lcd.setCursor(0,1);
    lcd.print("# Area: ");
    lcd.print(Area);
    lcd.print(" cm2.");
    lcd.setCursor(0,2);
    lcd.print("* Volume: ");
    lcd.print(Volume);
    lcd.print(" cm3");
    lcd.setCursor(0,3);
    lcd.print("0 exit:");
    delay(100);
    key = keypadManager.getKey();
    if (key == '*'){
      volume = true;
    }else if(key == '#'){
      area = true;
    }else if(key == '0'){
      break;
    }
  }
}


void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  if (myLIDAR.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    while(1);
  }
  Serial.println("LIDAR acknowledged!");
   impulse_per_cm_lidar = readSavedValue(0);
   Serial.println(impulse_per_cm_lidar);
// pin setup.
  pinMode(R_Motor_P_port, OUTPUT);
  pinMode(L_Motor_P_port, OUTPUT);  
  pinMode(R_Motor_D_port, OUTPUT);
  pinMode(L_Motor_D_port, OUTPUT);

  pinMode(X_port, INPUT);
  pinMode(Y_port, INPUT);

  pinMode(Channel_A, INPUT);
  pinMode(Channel_B, INPUT);

// interrupt setup.
  attachInterrupt(digitalPinToInterrupt(Channel_A), Channel_A_Encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(Channel_B), Channel_B_Encoder, RISING);
  
  lcd.begin(20, 4);
  
  
}

void loop() {

  if (Serial.available() > 0 && message.length()  == 0){

    message = Serial.readString();
    Serial.println("IP Address: " + message);

  }
  angle = readAngle(CMPS14_address);
   
  if (printCamps){
    print_compss();
  }else if (printLidar){
    lcd.clear();
    print_Lidar(travled_Dist);
    travled_Dist = calculate_dist(starting_point);
  }else if(printStats){
    print_stats();
  }
  if (remote_control){
    lcd.clear();
    Remote_Control();
    unsigned long currentMillis = millis();
    unsigned long  previousMillis;
    if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
      previousMillis = currentMillis;
      sendDataToESP( myLIDAR.getDistance(),angle);  
    }
  }
  if (Track){
    track(flow_track_bool);
  }
  if (calculate_area){
    surface_calculator();
    calculate_area = !calculate_area;
    printStats = true;
  }
  if (go){
          flowtrack_1();
  }

  if (calibrate_bool){
      calibrate();
  }
  if (save_bool){
    writeSavedValue( calibrate(), 0);
    save_bool = false;
  }

  X_reading = analogRead(X_port);
  Y_reading = analogRead(Y_port);
  X_postion =  map(X_reading,0,1023,100,-100);
  Y_postion =  map(Y_reading,0,1023,0,100);
 
  move_to();
 
  if (X_postion < 0){
   
    move (0, 0, readAngle(CMPS14_address));
  }else{

    move (0, 1, readAngle(CMPS14_address));

  }

  time_passed = millis();

  if (time_passed >= interval){

    R_RPM = (((Channel_R_I_counter - previous_R_postion)* 60 / impulse_per_rotaion));
    previous_R_postion = Channel_R_I_counter;
    L_RPM = (((Channel_L_I_counter - previous_L_postion)* 60 / impulse_per_rotaion));
    previous_L_postion = Channel_L_I_counter;
    
    Average_speed = ( (R_RPM +L_RPM) / 2 ) * 23.88;
    time_passed = 0;

  }
  
  total_Distance = distance_counter /impulse_per_cm;
  
}

void Channel_A_Encoder(){

  Channel_L_I_counter ++;
}
void Channel_B_Encoder(){
  Channel_R_I_counter ++;
  distance_counter++;

}


void move (int speed , byte direction ,int direct_hold ){
//speed 0% -> 100%.
// direction forwrad  1 ** backward  0.
   
    if (direction == 1){

      digitalWrite(R_Motor_D_port,backward);  
      digitalWrite(L_Motor_D_port,forwrad);
    
    }else{

      digitalWrite(R_Motor_D_port, forwrad);  
      digitalWrite(L_Motor_D_port, backward);
    }

    if (speed == 0 && direction == 0){
      speed = -X_postion;

    }else if (speed == 0){

      speed = X_postion;
    }

    X_coefficient = (speed *240)/100;
    if (Y_postion < 46){

      Y_postion *= 2;
      L_Motor_P = (X_coefficient * Y_postion) / 100;
      R_Motor_P = X_coefficient;

    }else if(Y_postion > 54){

      Y_postion = 200 - (Y_postion * 2);
      R_Motor_P = (X_coefficient * Y_postion) / 100;
      L_Motor_P = X_coefficient;

    }else{
      L_Motor_P = X_coefficient;
      R_Motor_P = X_coefficient;

    }
    if (( readAngle(CMPS14_address) - direct_hold) < -2){
      L_Motor_P = L_Motor_P *0.9;
    }else if (( readAngle(CMPS14_address) - direct_hold) > 2){
      R_Motor_P = R_Motor_P *0.9;
    }
    
    analogWrite(R_Motor_P_port,R_Motor_P);
    analogWrite(L_Motor_P_port,L_Motor_P);  

}

void rotate (String desired_direction){             

  byte R_direction;
  byte L_direction;
  int desired_angle;
  int PWM; 
  bool turn = true;
  freaz_camps  = angle;
  int rotaion_angle =  90;
  
  if (!( isAlpha(desired_direction[0]))){
    int Buffer_var = desired_direction.toInt();
    if (Buffer_var > 0 ){
      desired_direction = "C";
      rotaion_angle = Buffer_var;
    }else if (Buffer_var < 0){
      desired_direction = "D";
      rotaion_angle =  - Buffer_var;
    }else{
      desired_direction = "A";
    }
  }

  if (desired_direction == "C") {
    desired_angle = freaz_camps + rotaion_angle;
    R_direction = backward;
    L_direction = backward;
  } else if (desired_direction == "D") {
    R_direction = forwrad;
    L_direction = forwrad;
    desired_angle = freaz_camps - rotaion_angle;
  } else if (desired_direction == "B") {
    R_direction = forwrad;
    L_direction = forwrad; 
    desired_angle = freaz_camps + 180;
  }else  {
   turn = false;
  }
   
    
  if (desired_angle > 360){
      desired_angle -= 360;
  }else if(desired_angle < 0 ) {
      desired_angle += 360;
  }
  
  while ( turn && abs(angle - desired_angle) > 15 ){
    
    angle = readAngle(CMPS14_address);
    PWM  = 150;
    
    digitalWrite(R_Motor_D_port, R_direction);  
    digitalWrite(L_Motor_D_port, L_direction);
    analogWrite(R_Motor_P_port,PWM);
    analogWrite(L_Motor_P_port,PWM);  
  
  }
  analogWrite(R_Motor_P_port,0);
  analogWrite(L_Motor_P_port,0);
  delay(300);
  angle = readAngle(CMPS14_address);
  
  while (turn && abs(angle - desired_angle) > 1 ){
    
      angle = readAngle(CMPS14_address);
      if (angle > desired_angle){
        
        R_direction = forwrad;
        L_direction = forwrad;
        PWM = 100;

      }else{
        R_direction = backward;
        L_direction = backward;
        PWM = 100;
      }
      digitalWrite(R_Motor_D_port, R_direction);  
      digitalWrite(L_Motor_D_port, L_direction);
      analogWrite(R_Motor_P_port,PWM);
      analogWrite(L_Motor_P_port,PWM);

    }
    analogWrite(R_Motor_P_port,0);
    analogWrite(L_Motor_P_port,0);
}

void print_stats() {

  lcd.print("Group: lamouri.");
  lcd.setCursor(0, 1);
  lcd.print("RPM: ");
  lcd.print(Average_speed);
  lcd.print(" cm/m");
  lcd.setCursor(0, 2);
  lcd.print("Distance: ");
  lcd.print(total_Distance);
  lcd.print(" CM.");
  delay(100);
  lcd.clear();

}
void print_Lidar(int traveled_Distance) {
  
  lcd.print("Lidar Info:");
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print( Dist);
  lcd.print(" Cm.");
  lcd.setCursor(0, 2);
  lcd.print("Buffer : ");
  lcd.print(Distance_Buffer);
  lcd.print(" Cm.");
  lcd.setCursor(0, 3);
  lcd.print("traveled: ");
  lcd.print(traveled_Distance);
  lcd.print(" Cm.");
  Dist = myLIDAR.getDistance();
  delay(70);
  
}

void print_compss()
{

  char direction[] = {'N','E','S','W',' '}; 
  int index_1;
  int index_2 = 4;
  int cursor [] = {0, 0};
  
  byte customChar[] = {
     B11111,
     B11111,
     B11111,
     B11111,
     B11111,
     B11111,
     B11111,
     B11111
   };
  
  
  
  if (angle == 0 || angle == 360 ) {
    
    index_1 = 0;
    cursor [0] = 11;
    cursor [1] = 0;
     
  }else if (angle == 90){

    index_1 = 1; 
    cursor [0] = 15;
    cursor [1] = 2;

  }else if (angle == 180){
    
    index_1 = 2;
    cursor [0] = 11;
    cursor [1] = 3;
    
  }else if (angle == 270){

    index_1 = 3;
    cursor [0] = 8;
    cursor [1] = 2;

  }else if (angle > 0 && angle < 90){
   
    index_1 = 0;
    index_2 = 1; 
    cursor [0] = 13;
    cursor [1] = 1;

  }else if (angle > 90 && angle < 180){

    index_1 = 2;
    index_2 = 1;
    cursor [0] = 12;
    cursor [1] = 3;

  }else if (angle > 180 && angle < 270){

    index_1 = 2;
    index_2 = 3;
    cursor [0] = 8;
    cursor [1] = 4;
    
  }else if (angle > 270){

    index_1 = 0;
    index_2 = 3;
    cursor [0] = 7;
    cursor [1] = 1;
  }

  lcd.setCursor(0,0);
  lcd.print("Camps: ");
  lcd.setCursor(10,2);
  lcd.print(angle);
  lcd.print(" ");
  lcd.print(direction[index_1]);
  lcd.print(direction[index_2]);
  lcd.setCursor(cursor [0], cursor [1]);
  lcd.write(byte(0));
  delay(100);
  lcd.clear();
 

}

int readAngle(int CMPS_address) {
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
    Wire.requestFrom(CMPS14_address, 1, true);

    if (Wire.available() >= 1) {
        int raw = Wire.read();
        int angle = map(raw, 0, 255, 0, 360);
        return angle;
    }

    // Return an invalid angle if data is not available
    return -1;
}
int calculate_dist(int starting_point) {
  int distance = starting_point - myLIDAR.getDistance();

  if (distance < 0) {
    distance = -distance;
  }

  return distance;
}


void Remote_Control (){
  if (Serial.available() > 0 && remote_control){
    bool openMenu = false;
    String dire = " ";
    String Buffer_var = Serial.readStringUntil("x");
    

    String Buffer_var_name = Buffer_var.substring(0, 4); 
    
    if (Buffer_var_name == "dist"){
      
      Buffer_var = Buffer_var.substring(4);
      int dist = Buffer_var.toInt();
      Serial.print("Buffer_var: ");
      Serial.println(Buffer_var);
      if (dist < 0){
        dire = "B";
        dist = -dist;
        
      }else{
        dire = "A";
        
      }
      
      move_to(dist , dire);

    }else if(Buffer_var_name == "dire"){
      Buffer_var = Buffer_var.substring(4);
      rotate(Buffer_var);
      
    }
  }
}
void buffer () {
  if (set_buffer){
      int cursor = 14;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("set buffer: ");
      int k = 10;
      do{
       lcd.setCursor(cursor, 1);
       key = keypadManager.getKey();
       
       if (isDigit(key) && key != '#' && key != '*' ){

         lcd.print(key);
         Distance_Buffer = (key - 48) + Distance_Buffer*k ;
         Serial.print("\n"); 
          cursor ++;
        }
      
      if (key == '#' || key == '*' ){

        set_buffer = !set_buffer;
        break;
      }

      }while (true);
  }
}



void flowtrack_1(){
    int distance_array[4]= {10,20,10,20};
    String direction_array[4]={"D","D","D","D"};
    flow_track_bool = true; 
    for (int i = 0; i < 4; i++){
        Serial.println(direction_array[i]);
        buffer_var = distance_array[i];
        track(true);
        delay(500);
        rotate(direction_array[i]);
        delay(500);

    }
  go = false;


}

int calibrate(){
    distance_counter = 0;
    Serial.println(distance_counter);

    int startingPoint = myLIDAR.getDistance();
    if (startingPoint - 20 > 5){
      int endPoint = startingPoint - 20;
      Direct_Hold = readAngle(CMPS14_address);
      while ( myLIDAR.getDistance() > endPoint && ((myLIDAR.getDistance() - endPoint) != 65535)){
        if ((myLIDAR.getDistance() - endPoint) > 5){
          
          move(140, 1,Direct_Hold);

        }else{

        move(30, 1,Direct_Hold);
        }
      }
    }
    Serial.println(distance_counter);
    impulse_per_cm_lidar = distance_counter / 20;
    Serial.println(impulse_per_cm_lidar);
    calibrate_bool = false;
    return impulse_per_cm_lidar;

}

// Function to write a value to the EEPROM at the specified address
// Uses EEPROM.update() to only write if the value is different,
// which helps in reducing wear on the EEPROM
void writeSavedValue(byte value, int EEPROM_ADDRESS) {
  EEPROM.update(EEPROM_ADDRESS, value);
}

// Function to read a value from the EEPROM at the specified address
byte readSavedValue(int EEPROM_ADDRESS) {
  return EEPROM.read(EEPROM_ADDRESS);
}

void sendDataToESP(int myInt1, int myInt2) {
  // Convert the String to a char array before sending.
  String num1 = String(myInt1);
  String num2 = String(myInt2);
  String data = num1 + "," + num2;
  char buf[data.length() + 1]; // +1 to accommodate the null terminator.
  data.toCharArray(buf, sizeof(buf));

  // Use Serial.write to send the data as a byte array.
  Serial.write(buf, sizeof(buf) - 1); // -1 to exclude the null terminator from being sent.

  // Explicitly send a Line Feed (LF) character to signify the end of the message.
  Serial.write('\n');
}

