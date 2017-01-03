#include<SD.h>
#include <AccelStepper.h>
#define HALFSTEP 8
#define MAXSPEED 1000

//not sure but keep it smaller is a good way...
#define oringinSpeed 500
#define unitDistance 1000
#define unitDuration 1000

//SD card setting & frame info
const int selectPin = 53;
File myFile;
uint16_t maxRow;
uint16_t maxColumn;
uint16_t maxHeight;
uint16_t frameNumber;
uint16_t **eachFrame;
uint16_t **eachStatus;

//Stepper Motor Control
AccelStepper stepperArr[3][5];
int stepperPinMap[16][4];

int totalDistanceToGo = 0;
uint16_t currentFrame = 0;
uint16_t currentStatus = 0;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  //Serial.println("Initializing SD card...");
  pinMode(selectPin,OUTPUT);
  if(!SD.begin(selectPin)){
    return ;
  }
  readFile();
  
  //Stepper initialize
  createStep(maxRow, maxColumn);
  initStepper();
  
}


void loop() {  
  //update the stepper motor's frame height info
  for(int row = 0 ; row < maxRow; row++){
    for(int col = 0 ; col < maxColumn ; col++){
      stepperArr[row][col].moveTo( eachFrame[ row + currentFrame ][col] * unitDistance );
      stepperArr[row][col].setSpeed( eachStatus[currentStatus][0] * oringinSpeed );
    }
  }
  currentFrame += maxRow;
  //run frame until totalDistanceToGo = 0
  while(1){
    totalDistanceToGo = 0;
    for(int row = 0 ; row < maxRow ; row++){
      for(int col = 0 ; col < maxColumn ; col++){
        stepperArr[row][col].runSpeedToPosition();
        totalDistanceToGo += stepperArr[row][col].distanceToGo();
      }
    }
    if(totalDistanceToGo == 0){
      //which means all stepper motor have reach the target postion in this frame
      break;
    }
  }

  //stop for duration in status
  delay(eachStatus[currentStatus][1] * unitDuration);
  currentStatus++;
  if(currentStatus == frameNumber){
    while(1){
      //do nothing after finish all task.
    }
  }
}

char fileName = "test.rbl";
void readFile(){
  myFile = SD.open("test.rbl", FILE_READ);
  if(!myFile){
    return ;
  }

  myFile.read(&maxRow, sizeof(maxRow));
  myFile.read(&maxColumn, sizeof(maxColumn));
  myFile.read(&maxHeight, sizeof(maxHeight));
  myFile.read(&frameNumber, sizeof(frameNumber));
  
  eachFrame = malloc(maxRow * frameNumber * sizeof(uint16_t *));
  for(int i=0 ; i < maxRow * frameNumber ; i++){
    eachFrame[i] = malloc( maxColumn * sizeof(uint16_t) );
  }
  eachStatus = malloc( frameNumber * sizeof(uint16_t) );
  for(int i=0 ; i < frameNumber ; i++ ){
      eachStatus[i] = malloc( 2 * sizeof(uint16_t) );
  }

  for(int i=0 ; i < frameNumber ; i++){
    myFile.read(&eachStatus[i][0], sizeof(uint16_t));
    myFile.read(&eachStatus[i][1], sizeof(uint16_t));
    for(int len= i * maxRow ; len <  (i + 1) * maxRow ;  len++){
      for(int width= 0; width < maxColumn ; width++){
        myFile.read(&eachFrame[len][width], sizeof(uint16_t)); 
      }
    }
  }

  myFile.close();
}


void printFrame(){
  for(int i=0 ; i < frameNumber ; i++){
    Serial.println((String)"Speed:" + eachStatus[i][0]);
    Serial.println((String)"Duration" + eachStatus[i][1]);
    for(int row= i * maxRow ; row <  (i + 1) * maxRow ;  row++){
      for(int col= 0; col < maxColumn ; col++){
        Serial.print((String)eachFrame[row][col] + " ");
      }
      Serial.println();
    }
    Serial.println();
  }
  Serial.println("Please check if there is error.");
}

//Stepper Control section
void createStep(int rowMax, int colMax){
  int digitalPinMapStep=0;
  int analogPinMapStep=54;

  for(int row = 0 ; row <  rowMax ; row++){
    for(int col =0 ; col < colMax ; col++){
      if(digitalPinMapStep <=44){
        stepperArr[row][col] = AccelStepper(HALFSTEP,digitalPinMapStep, digitalPinMapStep+2, digitalPinMapStep+1, digitalPinMapStep+3); 
        digitalPinMapStep+=4;
      }
      else if(analogPinMapStep <= 66){
        stepperArr[row][col] = AccelStepper(HALFSTEP, analogPinMapStep, analogPinMapStep+2, analogPinMapStep+1, analogPinMapStep+3); 
        analogPinMapStep+=4;
      }
    }
  }
}

void initStepper(){
  for(int i = 0 ; i < maxRow ; i++ ){
    for(int j = 0 ; j < maxColumn ; j++){
      stepperArr[i][j].setMaxSpeed(MAXSPEED);
    }
  }
}

