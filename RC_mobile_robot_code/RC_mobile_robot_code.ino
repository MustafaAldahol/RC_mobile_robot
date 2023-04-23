  //define motor's pins//
#define EN_R1       3
#define IN1_R1      2
#define IN2_R1      4

#define EN_R2       5
#define IN1_R2      7
#define IN2_R2      6

#define EN_L1       9
#define IN1_L1      10
#define IN2_L1      8

#define EN_L2       11  
#define IN1_L2      13
#define IN2_L2      12
  // direction of motor
#define FORWARD             'f'
#define BACKWARD            'b'
  //direction of motion's variable
#define FRONT               'F' 
#define BACK                'B'
#define LEFT                'L'
#define RIGHT               'R'
#define FORWARD_LEFT        'G'
#define FORWARD_RIGHT       'I'
#define BACK_LEFT           'H'
#define BACK_RIGHT          'J'
#define STOP                'S'  //for both motor and motion suatus

  //lights's variables
#define FRONT_LIGHT_ON      'W'
#define FRONT_LIGHT_OFF     'w'
#define BACK_LIGHT_ON       'U'
#define BACK_LIGHT_OFF      'u'
#define BLINK_LIGHT_ON      'X'
#define BLINK_LIGHT_OFF     'x'

  //define led's pins
#define led_front   A2 
#define led_back    A3
#define led_reverse A4
#define led_blink   A5

  //define Ultrasonic's pins
#define TRIG        A0
#define ECHO        A1

const int SOUND_SPEED = 343; // 343 [m/s]
float  distance = 0 ,pre_distance =0;
 
byte speed;
byte pre_speed;
char direction;
// LEDs varialble to set LED's status ON or OFF
bool front_led;
bool back_led;
bool reverse_led;
bool blink_led;
unsigned long blink_timer = 0;


  // motor_name[3]         = {EN_pin , IN1_pin , IN2_pin}
byte motor_upper_right[3]  =  {EN_R1 , IN1_R1 , IN2_R1};
byte motor_down_right [3]  =  {EN_R2 , IN1_R2 , IN2_R2};
byte motor_upper_left [3]  =  {EN_L1 , IN1_L1 , IN2_L1};
byte motor_down_left  [3]  =  {EN_L2 , IN1_L2 , IN2_L2};


void set_motor(byte Motor_name[] ,char Direction ,byte S);
void set_motion(char data ,byte Speed);
void set_leds();
float get_dis();
void data_parsing(char data);


void setup() {
  Serial.begin(57600);
  for(byte i = 2 ;i <= 13 ;i++){
    pinMode(i ,OUTPUT);
  }
  pinMode(led_front   ,OUTPUT);
  pinMode(led_back    ,OUTPUT);
  pinMode(led_reverse ,OUTPUT);
  pinMode(led_blink   ,OUTPUT);
  pinMode(TRIG        ,OUTPUT);
  digitalWrite(TRIG ,LOW);
  speed = 20; //init speed becase the App doesn't send an initial value to car
  pre_speed = 20;
  }


//////////////////////////////////////////////
void loop() {
  
  char Data = Serial.read();
  data_parsing(Data); 
//  speed = map(speed,0,100,0,255);       // WARRNING: this two line make
//  speed =constrain(speed , 0, 255);     // a big problem, don't use them to rerange speed

 pre_distance = get_dis();  
 if (pre_distance != 0) {   // In some cases Ultrasonic return '0' if it doesn't detect an 
  distance = pre_distance;  // object,that's happen when distance > 300,we don't need this value
 }

  if(distance < 5 && distance != 0) {  
    speed = 0;
  }
  else{
    speed = pre_speed;
    }
  set_motion(Data ,speed);
  set_leds();

}
/////////////////////////////////////////////


 
  //----driving motor functions----//
void set_motor(byte Motor_name[] ,char Direction ,byte S){

  if(Direction == FORWARD){
    digitalWrite(Motor_name[1] ,HIGH);
    digitalWrite(Motor_name[2] ,LOW);
  }
  else if(Direction == BACKWARD){
    digitalWrite(Motor_name[1] ,LOW);
    digitalWrite(Motor_name[2] ,HIGH);    
  }
  else {
    digitalWrite(Motor_name[1] ,LOW);
    digitalWrite(Motor_name[2] ,LOW);     
  }
  analogWrite(Motor_name[0],S);
}

void set_motion(char data ,byte Speed ){
  // need to adjust speed for every motor one by one in real world
  // becase motors dosn't come with the same speed..
  switch(data)
  {
  case FRONT : {
    set_motor(motor_upper_right ,FORWARD ,Speed);
    set_motor(motor_down_right  ,FORWARD ,Speed);
    set_motor(motor_upper_left  ,FORWARD ,Speed);
    set_motor(motor_down_left   ,FORWARD ,Speed);
  } break;
  
  case BACK : {
    set_motor(motor_upper_right ,BACKWARD ,Speed);
    set_motor(motor_down_right  ,BACKWARD ,Speed);
    set_motor(motor_upper_left  ,BACKWARD ,Speed);
    set_motor(motor_down_left   ,BACKWARD ,Speed);
  } break;
  
  case LEFT : {
    set_motor(motor_upper_right ,FORWARD  ,Speed);
    set_motor(motor_down_right  ,FORWARD  ,Speed);
    set_motor(motor_upper_left  ,BACKWARD ,Speed);
    set_motor(motor_down_left   ,BACKWARD ,Speed);
  } break;
  
  case RIGHT : {
    set_motor(motor_upper_right ,BACKWARD ,Speed);
    set_motor(motor_down_right  ,BACKWARD ,Speed);
    set_motor(motor_upper_left  ,FORWARD  ,Speed);
    set_motor(motor_down_left   ,FORWARD  ,Speed);
  } break;
  
  case FORWARD_LEFT : {
    set_motor(motor_upper_right ,FORWARD ,Speed);
    set_motor(motor_down_right  ,FORWARD ,Speed);
    set_motor(motor_upper_left  ,FORWARD ,Speed/4); // dividing by numer that need to adjust in real world
    set_motor(motor_down_left   ,FORWARD ,Speed/4);
  } break;
  
  case FORWARD_RIGHT : { 
    set_motor(motor_upper_right ,FORWARD ,Speed/4);
    set_motor(motor_down_right ,FORWARD ,Speed/4);
    set_motor(motor_upper_left ,FORWARD ,Speed);
    set_motor(motor_down_left ,FORWARD ,Speed);
  } break;
  
  case BACK_LEFT : {
    set_motor(motor_upper_right ,BACKWARD ,Speed);
    set_motor(motor_down_right  ,BACKWARD ,Speed);
    set_motor(motor_upper_left  ,BACKWARD ,Speed/4);
    set_motor(motor_down_left   ,BACKWARD ,Speed/4);
  } break;
  
  case BACK_RIGHT : {
    set_motor(motor_upper_right ,BACKWARD ,Speed/4);
    set_motor(motor_down_right  ,BACKWARD ,Speed/4); 
    set_motor(motor_upper_left  ,BACKWARD ,Speed);
    set_motor(motor_down_left   ,BACKWARD ,Speed);
  } break;
  
  case STOP : {
    Speed = Speed; // it's same speed comes from app 
    set_motor(motor_upper_right ,STOP ,Speed);
    set_motor(motor_down_right ,STOP ,Speed);
    set_motor(motor_upper_left ,STOP ,Speed);
    set_motor(motor_down_left ,STOP ,Speed);
  }   
  }
}

  //get_dis func reads distance and Decide to stop motor or Not
float get_dis(){
  // make sure this func placed before set_motion func, to give speed's value to it
 double duration = 0;
  digitalWrite(TRIG,LOW);
  delayMicroseconds(1);
  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  // 1000 refere to wait for 1 [ms] after that time there's no wall
  //this number is very important, if was too small, distance will never be token
  //1 [ms] is enough to detect 34 [cm]
  duration = pulseIn(ECHO,HIGH,1000); 
 
  pre_distance =(SOUND_SPEED * duration) /(10000.00)/2.0; // distance in cm
  
//   Serial.println(distance);

return(pre_distance);
}

  //----driving LEDs functions----//
void set_leds(){

//---set front led---//
    if(front_led == true){
      digitalWrite(led_front ,HIGH);
    }
    else{
      digitalWrite(led_front ,LOW);
    }
//---set back led---//
    if(back_led == true){
      digitalWrite(led_back ,HIGH);
    }
    else{
      digitalWrite(led_back ,LOW);
    }
//---set reverse led---//
    if(reverse_led == true){
      digitalWrite(led_reverse ,HIGH);
    }
    else{
      digitalWrite(led_reverse ,LOW); 
    }
//---set blinking led---//    
    if(blink_led == true){
      // blink led with freqancy equal to 1/100
      if( (millis() - blink_timer) < 100 ){
        digitalWrite(led_blink ,HIGH);        
      }
      else if( (millis() - blink_timer >= 100) && (millis() - blink_timer <= 200)){
      digitalWrite(led_blink ,LOW);    
      }
      else blink_timer = millis();
    }
    
    if(blink_led == false){
      digitalWrite(led_blink ,LOW); 
    }
    
}

 //---parsing data come from App to do it's work---//
void data_parsing(char data){
  switch(data){
    case FRONT :          back_led = false ; reverse_led = false;break;
    case BACK :           back_led = false ; reverse_led = true ;break;
    case LEFT :           back_led = false ; reverse_led = false;break;
    case RIGHT :          back_led = false ; reverse_led = false;break;
    case FORWARD_LEFT :   back_led = false ; reverse_led = false;break;
    case FORWARD_RIGHT :  back_led = false ; reverse_led = false;break;
    case BACK_LEFT :      back_led = false ; reverse_led = true ;break;
    case BACK_RIGHT :     back_led = false ; reverse_led = true ;break;
    case STOP :           back_led = true  ; reverse_led = false;break;
    
    case FRONT_LIGHT_ON   : front_led = true  ;break;
    case FRONT_LIGHT_OFF  : front_led = false ;break;
    case BACK_LIGHT_ON    : back_led  = true  ;break;
    case BACK_LIGHT_OFF   : back_led  = false ;break;
    case BLINK_LIGHT_OFF  : blink_led = false ;break;
    case BLINK_LIGHT_ON   : blink_led = true; blink_timer = millis();  ;break;
      
    case '0' : pre_speed = 0   ;break;
    case '1' : pre_speed = 25  ;break;
    case '2' : pre_speed = 50  ;break;
    case '3' : pre_speed = 100 ;break;
    case '4' : pre_speed = 125 ;break;
    case '5' : pre_speed = 150 ;break;
    case '6' : pre_speed = 175 ;break;
    case '7' : pre_speed = 200 ;break;
    case '8' : pre_speed = 225 ;break;
    case '9' : pre_speed = 250 ;break;
    case 'q' : pre_speed = 255 ;break;
    
  } 
}

