// http://www.iotsharing.com/2017/05/how-to-use-arduino-esp32-to-store-data-to-sdcard.html

#include <mySD.h>

File myFile;
String testID = "testeZERO";
uint32_t msgsReceived = 0;


void openSDcard(){
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {  // 26=CS; 14=MOSI; 13=MISO; 27=SCK
    Serial.println("SD Card - init failed!");
    return;
  }
  Serial.println("SD Card - init OK.");    
}

void listSDcard(){
  /* Begin at the myFile "/" */
  myFile = SD.open("/");
  if (myFile) {    
    printDirectory(myFile, 0);
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }  
}

void writeTestSDcard(){

  /* open "test.txt" for writing */
  myFile = SD.open("test.txt", FILE_WRITE);
  /* if open succesfully -> myFile != NULL 
    then write string "Hello world!" to it
  */
  if (myFile) {
    myFile.println("Hello world!");
    myFile.flush();
   /* close the file */
    myFile.close();
  } else {
    /* if the file open error, print an error */
    Serial.println("error opening test.txt");
  }  
}

void readTestSDcard(){
  /* after writing then reopen the file and read it */
  myFile = SD.open("test.txt");
  if (myFile) {    
    /* read from the file until there's nothing else in it */
    while (myFile.available()) {
      /* read the file and print to Terminal */
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
}

void setup()
{
  Serial.begin(115200);
  openSDcard();
  listSDcard();
  writeTestSDcard();
  delay(1000);
  readTestSDcard();
  Serial.println("done!");
}

void loop()
{
}

void printDirectory(File dir, int numTabs) {
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');   // we'll have a nice indentation
     }
     // Print the name
     Serial.print(entry.name());
     /* Recurse for directories, otherwise print the file size */
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       Serial.print("\t\t");
       Serial.println(entry.size());
     }
     entry.close();
   }
}
