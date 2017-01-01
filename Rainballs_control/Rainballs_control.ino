#include<SD.h>


const int selectPin = 53;
File myFile;
uint16_t unit;
uint16_t maxLength;
uint16_t maxWidth;
uint16_t maxHeight;
uint16_t baseSpeed;
uint16_t frameNumber;
uint16_t **eachFrame;
uint16_t **eachStatus;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  pinMode(selectPin,OUTPUT);
  if(!SD.begin(selectPin)){
    Serial.println("Card failed...");
    return ;
  }
  Serial.println("Card initialized.");
  readFile();
  printFrame();
}

void loop() {  
  //Serial.println("Press the button to start~~");
  

}

char fileName = "test.rbl";
void readFile(){
  Serial.println("Starting to readfile...");
  myFile = SD.open("test.rbl", FILE_READ);
  if(!myFile){
    Serial.println("Fail to open file.");
    return ;
  }

  //
  myFile.read(&maxLength, sizeof(maxLength));
  myFile.read(&maxWidth, sizeof(maxWidth));
  myFile.read(&maxHeight, sizeof(maxHeight));
  myFile.read(&frameNumber, sizeof(frameNumber));

  Serial.println(maxLength);
  Serial.println(maxWidth);
  eachFrame = malloc(maxLength * frameNumber * sizeof(uint16_t *));
  for(int i=0 ; i < maxLength * frameNumber ; i++){
    eachFrame[i] = malloc( maxWidth * sizeof(uint16_t) );
  }
  eachStatus = malloc( frameNumber * sizeof(uint16_t) );
  for(int i=0 ; i < frameNumber ; i++ ){
      eachStatus[i] = malloc( 2 * sizeof(uint16_t) );
  }

  for(int i=0 ; i < frameNumber ; i++){
    myFile.read(&eachStatus[i][0], sizeof(uint16_t));
    myFile.read(&eachStatus[i][1], sizeof(uint16_t));
    for(int len= i * maxLength ; len <  (i + 1) * maxLength ;  len++){
      for(int width= 0; width < maxWidth ; width++){
        myFile.read(&eachFrame[len][width], sizeof(uint16_t)); 
      }
    }
  }

  myFile.close();
  Serial.println("Closing File...");
}


void printFrame(){
  for(int i=0 ; i < frameNumber ; i++){
    Serial.println((String)"Speed:" + eachStatus[i][0]);
    Serial.println((String)"Duration" + eachStatus[i][1]);
    for(int len= i * maxLength ; len <  (i + 1) * maxLength ;  len++){
      for(int width= 0; width < maxWidth ; width++){
        Serial.print((String)eachFrame[len][width] + " ");
      }
      Serial.println();
    }
    Serial.println();
  }
  Serial.println("Please check if there is error.");
}

