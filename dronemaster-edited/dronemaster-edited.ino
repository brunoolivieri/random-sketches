//#include <esp_now.h>
//#include <WiFi.h>
#include <mavlink.h>
#include <SoftwareSerial.h>
#define GPS_INFO "GLOBAL_POSITION_INT_COV"
#define ESP32_SYSID 2
#define ESP32_COMPID 1

static uint8_t broadcastAddress[] = {0xFF, 0xFF,0xFF,0xFF,0xFF,0xFF};

SoftwareSerial SerialMav(17, 16);

int heartbeats = 0;


void setup() {
  Serial.begin(115200); //Main serial port for console output
  Serial.println("Serial do ftdi ok");
  
  SerialMav.begin(57600); //RXTX from Pixhawk (Port 19,18 Arduino Mega)
  Serial.println("Serial do ardupilot ok");
  
  Mav_Request_Data();
}


void _MavLink_request_GPS() {
  mavlink_message_t msg;
  mavlink_status_t stats;
  uint8_t len;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  Serial.println("Requesting GPS data");
  
  mavlink_msg_param_request_read_pack(ESP32_SYSID, ESP32_COMPID, &msg, 1, 1, GPS_INFO, -1);
  SerialMav.write(buf, len);
}

void Mav_Request_Data()
{
  mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // STREAMS that can be requested
  /*
   * Definitions are in common.h: enum MAV_DATA_STREAM
   *   
   * MAV_DATA_STREAM_ALL=0, // Enable all data streams
   * MAV_DATA_STREAM_RAW_SENSORS=1, /* Enable IMU_RAW, GPS_RAW, GPS_STATUS packets.
   * MAV_DATA_STREAM_EXTENDED_STATUS=2, /* Enable GPS_STATUS, CONTROL_STATUS, AUX_STATUS
   * MAV_DATA_STREAM_RC_CHANNELS=3, /* Enable RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW
   * MAV_DATA_STREAM_RAW_CONTROLLER=4, /* Enable ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT.
   * MAV_DATA_STREAM_POSITION=6, /* Enable LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages.
   * MAV_DATA_STREAM_EXTRA1=10, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA2=11, /* Dependent on the autopilot
   * MAV_DATA_STREAM_EXTRA3=12, /* Dependent on the autopilot
   * MAV_DATA_STREAM_ENUM_END=13,
   * 
   * Data in PixHawk available in:
   *  - Battery, amperage and voltage (SYS_STATUS) in MAV_DATA_STREAM_EXTENDED_STATUS
   *  - Gyro info (IMU_SCALED) in MAV_DATA_STREAM_EXTRA1
   */

  // To be setup according to the needed information to be requested from the Pixhawk
  const int  maxStreams = 1;
  const uint8_t MAVStreams[maxStreams] = {MAV_DATA_STREAM_ALL};
  const uint16_t MAVRates[maxStreams] = {0x02};
    
  for (int i=0; i < maxStreams; i++) {
    /*
     * mavlink_msg_request_data_stream_pack(system_id, component_id, 
     *    &msg, 
     *    target_system, target_component, 
     *    MAV_DATA_STREAM_POSITION, 10000000, 1);
     *    
     * mavlink_msg_request_data_stream_pack(uint8_t system_id, uint8_t component_id, 
     *    mavlink_message_t* msg,
     *    uint8_t target_system, uint8_t target_component, uint8_t req_stream_id, 
     *    uint16_t req_message_rate, uint8_t start_stop)
     * 
     */
     
    mavlink_msg_request_data_stream_pack(ESP32_SYSID, ESP32_COMPID, &msg, 1, 1, MAVStreams[i], MAVRates[i], 1);
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);    
    SerialMav.write(buf, len);
  }
}

void _MavLink_receive() {
  mavlink_message_t msg;
  mavlink_status_t stats;

  Serial.println("Start Mav Read");
  while(SerialMav.available()) {
    uint8_t c = SerialMav.read();
    if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &stats)) {
      Serial.print("Message id (msgid): ");
      Serial.println(msg.msgid);
      Serial.print("Size of message is: "); Serial.println(sizeof(mavlink_message_t));

      switch(msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
          Serial.println("case MAVLINK_MSG_ID_HEARTBEAT");
          mavlink_heartbeat_t hb;
          mavlink_msg_heartbeat_decode(&msg, &hb);

          Serial.println("HeartBeat");
          Serial.print("system_status: ");
          Serial.println(hb.system_status);

          heartbeats++;
          
          break;
        }
        case MAVLINK_MSG_ID_GPS_STATUS:
        {
          Serial.println("case MAVLINK_MSG_ID_GPS_STATUS");
          Serial.println("GPS_STATUS");
          __mavlink_gps_status_t gpsStatus;
          mavlink_msg_gps_status_decode(&msg, &gpsStatus);

          Serial.print("Number of satellites: ");
          Serial.println(gpsStatus.satellites_visible);
          break;
        }
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
          Serial.println("case MAVLINK_MSG_ID_GLOBAL_POSITION_INT");
          Serial.println("GPS Position!");
          __mavlink_global_position_int_t gpsPos;
          mavlink_msg_global_position_int_decode(&msg, &gpsPos);

          Serial.print("Relative Altitude: ");
          Serial.println(gpsPos.relative_alt);
          Serial.print("Latitude: ");
          Serial.print(gpsPos.lat);
          Serial.print("  -  Longitude: ");
          Serial.println(gpsPos.lon);
          Serial.print("Altitude above MSL: ");
          Serial.println(gpsPos.alt);
        }
        case MAVLINK_MSG_ID_GPS_RAW_INT:
        {
          Serial.println("case MAVLINK_MSG_ID_GPS_RAW_INT");
          Serial.println("GPS Position!");
          __mavlink_gps_raw_int_t gpsPos;
          mavlink_msg_gps_raw_int_decode(&msg, &gpsPos);

          Serial.print("Number of satellites: ");
          Serial.println(gpsPos.satellites_visible);

          Serial.print("Latitude: ");
          Serial.print(gpsPos.lat);
          Serial.print("  -  Longitude: ");
          Serial.println(gpsPos.lon);
          Serial.print("Altitude above MSL: ");
          Serial.println(gpsPos.alt);
          
        }
       default:
       {
          Serial.print("switch case default: ");
          Serial.println("switch case default: ");
          delay(1000);
       }
      }
    } else {
      Serial.println("Não funcionou o if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &stats))");
    }
  }
  Serial.println("End Mav Read");
  delay(1000);
}

void loop() {
  _MavLink_receive();
  if( heartbeats > 60 ) {
    heartbeats = 0;
    Mav_Request_Data();
  }
  delay(1000);
}
