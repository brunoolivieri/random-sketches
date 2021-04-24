// http://www.iotsharing.com/2017/05/how-to-use-arduino-esp32-to-store-data-to-sdcard.html

#include <mySD.h>

File myFile;
char *testID = "T0";
char logFileName[] = "default.txt";
uint32_t msgsReceived = 0;
long randNumber;


void openSDcard(){
  Serial.println("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {  // 26=CS; 14=MOSI; 13=MISO; 27=SCK
    Serial.println("SD Card - init failed!");
    return;
  }
  Serial.println("SD Card - init OK.");    
}

int countFilesInSDcard(){
  int accFiles = 0;
  myFile = SD.open("/");
  if (myFile) {    
    while(true) {
     File entry =  myFile.openNextFile();
     if (!entry) {
       break;
     }
     accFiles++;
     entry.close();
    }
    myFile.close();
  } else {
    Serial.println("error couting files");
  }  
  return accFiles; 
}

void doLog2SDCard(String logline){
  Serial.printf("I should log: %s in %s\n", logline, logFileName);

  /* open "test.txt" for writing */
  myFile = SD.open(logFileName, FILE_WRITE);
  /* if open succesfully -> myFile != NULL 
    then write string "Hello world!" to it
  */
  if (myFile) {
    myFile.println(logline);
    myFile.flush();
    myFile.close();
  } else {
    Serial.println("ERROR logging");
  }   
}


void setup()
{
  Serial.begin(115200);
  openSDcard();
  int acc_files = countFilesInSDcard();
  Serial.printf("\nFiles found: %s\n", String(acc_files));
  acc_files++;

  strcpy(logFileName,testID);
  strcat(logFileName,"-");
  char temp[12]; 
  itoa(acc_files, temp, 10);
  strcat(logFileName,temp);
  strcat(logFileName,".txt");
  
  Serial.printf("\n\nNew logfile name: \\%s\\\n\n",logFileName );
  doLog2SDCard("bootstrap");
  randomSeed(analogRead(0));
  Serial.println("Finish init");
}

void loop()
{
  randNumber = random(10, 20);
  String thisString = String(randNumber);
  doLog2SDCard(thisString);
  delay(2000);
}
