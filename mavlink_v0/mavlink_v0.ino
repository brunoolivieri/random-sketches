#include <mavlink.h>
#include <SoftwareSerial.h>
#define RXpin 17
#define TXpin 16
SoftwareSerial SerialMAV(RXpin, TXpin); // sets up serial communication on pins 3 and 2

mavlink_message_t msg;
mavlink_status_t status;
int chan = MAVLINK_COMM_0;


void setup() {
  Serial.begin(115200); //Main serial port for console output
  Serial.println("Serial do ftdi ok");
  
  SerialMAV.begin(57600); //RXTX from Pixhawk 
  Serial.println("Serial do ardupilot ok");
  request_datastream();

}

void loop() {
  MavLink_receive();
}

void MavLink_receive(){ 

 //Serial.println("Checando serial 2 - no MavLink_receive()");

  while(SerialMAV.available())
  {
    uint8_t byte = SerialMAV.read();

    //Get new message
    if(mavlink_parse_char(chan, byte, &msg, &status))
    {
     
      //Handle new message from autopilot
      switch(msg.msgid){

      case MAVLINK_MSG_ID_GPS_RAW_INT:
      {
        mavlink_gps_raw_int_t packet;
        mavlink_msg_gps_raw_int_decode(&msg, &packet);
        Serial.print("|->>> GPS RAW INT |  ");
        Serial.print("GPS Fix: ");
        Serial.print(packet.fix_type);
        Serial.print(" - Sats Visible: ");
        Serial.print(packet.satellites_visible);
        Serial.print(" - Latitude: ");
        Serial.print(packet.lat);
        Serial.print(" - Longitude: ");
        Serial.print(packet.lon);
        Serial.print(" - Speed: ");
        Serial.println(packet.vel);
        Serial.println("-------------------------------------------------");
        break;
       }
       case MAVLINK_MSG_ID_HEARTBEAT:
       {
          mavlink_heartbeat_t hb;
          mavlink_msg_heartbeat_decode(&msg, &hb);
          Serial.print("|->>> Heartbeat |  system_status: ");
          Serial.println(hb.system_status);  
          Serial.println("-------------------------------------------------");
          break;
       }
       case MAVLINK_MSG_ID_GPS_STATUS:
       {
//          Serial.print("|->>> GPS_STATUS | ");
//          mavlink_gps_status_t gpsStatus;
//          mavlink_msg_gps_status_decode(&msg, &gpsStatus);
//          Serial.print("Number of satellites: ");
//          Serial.println(gpsStatus.satellites_visible);
//          Serial.println("-------------------------------------------------");          
          break;
        }
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
          Serial.print("|->>> GPS Position INT | ");
          mavlink_global_position_int_t gpsPos;
          mavlink_msg_global_position_int_decode(&msg, &gpsPos);
          Serial.print("Relative Altitude: ");
          Serial.print(gpsPos.relative_alt);
          Serial.print(" - Latitude: ");
          Serial.print(gpsPos.lat);
          Serial.print("  -  Longitude: ");
          Serial.print(gpsPos.lon);
          Serial.print(" - Altitude above MSL: ");
          Serial.println(gpsPos.alt);
          Serial.println("-------------------------------------------------");          
          break;
        }
        case MAVLINK_MSG_ID_SYS_STATUS:
        {
          //Serial.print("|->>> Msg de SYS_STATUS | Load: ");
          mavlink_sys_status_t ss;
          mavlink_msg_sys_status_decode(&msg, &ss);
//          Serial.print(ss.load);
//          Serial.print(" - voltage_battery: ");
//          Serial.print(ss.voltage_battery);
//          Serial.print(" - battery_remaining: ");
//          Serial.print(ss.battery_remaining);
//          Serial.print(" - drop_rate_comm: ");
//          Serial.print(ss.errors_comm);          
//          Serial.print(" - errors_comm: ");
//          Serial.println(ss.drop_rate_comm);          
//          Serial.println("-------------------------------------------------");
          break;
        }case MAVLINK_MSG_ID_SYSTEM_TIME:
        {
//          Serial.print("|->>> Msg de SYSTEM_TIME , id = 2 | Unix time:  ");
//          mavlink_system_time_t st;
//          mavlink_msg_system_time_decode(&msg, &st);
//          uint32_t low = st.time_unix_usec % 0xFFFFFFFF; 
//          uint32_t high = (st.time_unix_usec >> 32) % 0xFFFFFFFF;
//          Serial.print(low); 
//          Serial.print(high); 
//          Serial.print(" - Boot time: ");
//          Serial.print(st.time_boot_ms); 
//          Serial.println("-------------------------------------------------");
          break;
        }case 22:
        {
          //Serial.println("|->>> Msg de PARAM_VALUE, id = 22");
          break;
        }
        case 27:
        {
          //Serial.println("|->>> Msg de RAW_IMU, id = 27");
          break;
        }
        case 29:
        {
          //Serial.println("|->>> Msg de SCALED_PRESSURE, id = 29");
          break;
        }case 30:
        {
          //Serial.println("|->>> Msg de ATTITUDE, id = 30");
          break;
        }
        case 35:
        {
          //Serial.println("|->>> Msg de RC_CHANNELS_RAW, id = 35");
          break;
        }case 36:
        {
          //Serial.println("|->>> Msg de SERVO_OUTPUT_RAW, id = 36");
          break;
        }
        case 42:
        {
          //Serial.println("|->>> Msg de MISSION_CURRENT, id = 42");
          break;
        }case 62:
        {
          //Serial.println("|->>> Msg de NAV_CONTROLLER_OUTPUT, id = 62");
          break;
        }
        case 74:
        {
          //Serial.println("|->>> Msg de VFR_HUD, id = 74");
          break;
        }
      default:
           //Serial.println("Mensagem fora do switch\n");
           printf("**** not predicted: Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
      }
    }
//    else {
//          Serial.print("Serial do drone não parseada com:");
//          Serial.println(byte);
//
//    }
  }
  //Serial.println("Saindo de MavLink_receive()");
}

void request_datastream() {
//Request Data from Pixhawk
  uint8_t _system_id = 200; //255; // id of computer which is sending the command (ground control software has id of 255)
  uint8_t _component_id = 2; // seems like it can be any # except the number of what Pixhawk sys_id is
  uint8_t _target_system = 1; // Id # of Pixhawk (should be 1)
  uint8_t _target_component = 0; // Target component, 0 = all (seems to work with 0 or 1
  uint8_t _req_stream_id = MAV_DATA_STREAM_ALL;
  uint16_t _req_message_rate = 0x01; //number of times per second to request the data in hex
  uint8_t _start_stop = 1; //1 = start, 0 = stop

// STREAMS that can be requested
  /*
   * Definitions are in common.h: enum MAV_DATA_STREAM and more importantly at:
     https://mavlink.io/en/messages/common.html#MAV_DATA_STREAM
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

  // Initialize the required buffers
  //mavlink_message_t msg;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];

  // Pack the message
  mavlink_msg_request_data_stream_pack(_system_id, _component_id, &msg, _target_system, _target_component, _req_stream_id, _req_message_rate, _start_stop);
  uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);  // Send the message (.write sends as bytes)

  Serial.println("Pedindo... ");
  SerialMAV.write(buf, len); //Write data to serial port
  Serial.println("Esperando... ");
}
