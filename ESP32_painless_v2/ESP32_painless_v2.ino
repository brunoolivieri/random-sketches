#include "painlessMesh.h"
//#include "namedMesh.h"
#include <ArduinoJson.h>

#define   MESH_PREFIX     "gradysnetwork"
#define   MESH_PASSWORD   "yetanothersecret"
#define   MESH_PORT       8888
#define   myChipNameSIZE    24
#define   LED             5    // GPIO number of connected LED (2 no esp do Breno)
#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  100  // milliseconds LED is on for


//// - begin wifi
Scheduler userScheduler; // to control tasks
painlessMesh  mesh;
char myChipName[myChipNameSIZE]; // Chip name (to store MAC Address
//String nodeName(myChipName); // Name needs to be unique and uses the attached .h file
bool calc_delay = false;
SimpleList<uint32_t> nodes;
//// - end wifi

//// - Serial
String  string_test;
String  string2convertions;
int seq = 0;
int teste = 0;
DynamicJsonDocument json_receive(1024);
StaticJsonDocument<300> json_send;
StaticJsonDocument<300> json_echo_send;
void sendJson();
void readJson();
//// - Serial


// User stub
void sendWifiMessage() ; // Prototype so PlatformIO doesn't complain
void sendSerialJson() ; // Prototype so PlatformIO doesn't complain

Task tasksendWifiMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendWifiMessage );
Task tasksendSerialJson( TASK_SECOND * 1 , TASK_FOREVER, &sendSerialJson ); //2Breno


// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;


void sendWifiMessage() {
  String msg = "Msg generated on ";
  msg += mesh.getNodeId();
  msg += " - myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast( msg );
  tasksendWifiMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  tasksendSerialJson.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 )); //2Breno

  // blink
  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  tasksendWifiMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
  tasksendSerialJson.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds //2Breno

}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("%s: Received from %u msg=%s\n", myChipName, from, msg.c_str());
}

//void receivedCallback_str( String &from, String &msg ) {
//  Serial.printf("%s: Received from %s msg=%s\n", myChipName, from.c_str(), msg.c_str());
//}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("%s: New Connection, nodeId = %u\n", myChipName, nodeId);
}

void changedConnectionCallback() {
  Serial.printf("%s: Changed connections\n", myChipName);

  //blink
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num of other known nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;

}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("%s: Adjusted time %u. Offset = %d\n", myChipName, mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

  // getting ESP32 details:
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(myChipName, myChipNameSIZE, "SENSOR-BAT-%04X%08X", chip, (uint32_t)chipid);
  Serial.print("Hello! I am ");
  Serial.println(myChipName);
  ////////////////

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );

 // mesh.setName(nodeName); // This needs to be an unique name! 
  mesh.onReceive(&receivedCallback);
  //mesh.onReceive(&receivedCallback_str); //extra for names
  
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( tasksendWifiMessage );
  tasksendWifiMessage.enable();
  userScheduler.addTask( tasksendSerialJson );//2Breno
  tasksendSerialJson.enable();//2Breno
  
  ///  Blink stuff
  pinMode(LED, OUTPUT);
  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
    // If on, switch off, else switch on
    if (onFlag)
      onFlag = false;
    else
      onFlag = true;
    blinkNoNodes.delay(BLINK_DURATION);

    if (blinkNoNodes.isLastIteration()) {
      // Finished blinking. Reset task for next run 
      // blink number of nodes (including this node) times
      blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
      // Calculate delay based on current mesh time and BLINK_PERIOD
      // This results in blinks between nodes being synced
      blinkNoNodes.enableDelayed(BLINK_PERIOD - 
          (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
    }
});
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();
  randomSeed(analogRead(A0));
 
}


void sendJson() {
  json_send["id"] = myChipName;
  json_send["type"] = 50;  // type for GPS stuff?
  json_send["count"] = seq++;
  json_send["lat"] = -15.840074;
  json_send["lon"] = -47.926646;
  json_send["alt"] = 11.1;
  json_send["gpsfixtype"] = 0;
  serializeJson(json_send, Serial);
  Serial.print("\n");
}

void receiveJson() {
  while(Serial.available()){
    string_test = Serial.readString();
    deserializeJson(json_receive, string_test);

    if (json_receive["type"] == 1) {     // it is the echo request
      json_echo_send["id"] = myChipName; // my esp32 chip name
      json_echo_send["type"] = 2;        // echo reply
      json_echo_send["count"] = seq++;   // seq from the sender
      string2convertions = "echo reply from seq ";
      string2convertions += String(seq);
      json_echo_send["payload"] = string2convertions;
      serializeJson(json_echo_send, Serial);
      Serial.print("\n");
    } else {
       Serial.printf("%s: Unknown command type, msg: %s", myChipName, string_test);
    }

  }
}


void loop() {
  // Breno
  receiveJson();
 
  // It will run the user scheduler as well
  mesh.update();

  // Blink the led with the number of connected nodes.
  digitalWrite(LED, !onFlag);
  
}
