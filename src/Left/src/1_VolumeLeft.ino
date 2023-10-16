#include <Wire.h>
#include <SigmaDSP.h>
#include <painlessMesh.h>
#include "SigmaDSP_parameters.h"
#include <../lib/AudioBuffer.h>


#define   MESH_PREFIX     "LivingRoom"
#define   MESH_PASSWORD   "PassworD1234"
#define   MESH_PORT       5555

SigmaDSP* dsp;


Scheduler userScheduler; 
painlessMesh mesh;

bool ackReceived = false;
int muteState = 0;
unsigned long lastSent = millis() - 2000;
const unsigned long interval = 2000;
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 60000;
unsigned long lastHeartbeatReceived = 0;
const unsigned long HEARTBEAT_TIMEOUT = 1000;
long heartbeatTimeoutCounter = 0;
float currentVolume = -10;
float currentSubVolume = 0;
int currentSource = 2;
uint32_t serverId = 0;
uint32_t myNodeId = 0;


void handleCallback(uint32_t, String &);
void setVolume(float);
void setSubVolume(float);
void mute(int);
String getValue(const String& data, char separator, int index);
void sendHeartbeat();
void sendMessage();
void receivedCallback(uint32_t, String &);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void changeSource(int);

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void setup() {
  Serial.begin(115200);
  Wire.begin();  // Ensure Wire is initialized
  dsp = new SigmaDSP(Wire, DSP_I2C_ADDRESS, 48000.00f);

  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();

  myNodeId = mesh.getNodeId();

  Serial.println("Mesh Node ID:");
  Serial.println(myNodeId);

  Serial.println(F("SigmaDSP 1_Volume example\n"));

  Wire.begin();
  dsp->begin();
  //ee.begin();

  delay(2000);

  Serial.println(F("Pinging i2c lines...\n0 -> device is present\n2 -> device is not present"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp->ping());
  //Serial.print(F("EEPROM ping: "));
  //Serial.println(ee.ping());

  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(*dsp);
  Serial.println("Done!\n");


  // Comment out the three code lines above and use this step instead if EEPROM is present
  // The last parameter in writeFirmware is the FW version, and prevents the MCU from overwriting on every reboot
  //ee.writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 0);
  //dsp.reset();
  //delay(2000); // Wait for the FW to load from the EEPROM

  setVolume(currentVolume);
  setSubVolume(currentVolume);

  mute(muteState);
}

void loop() {

  mesh.update();

  if (!ackReceived && (millis() - lastSent >= interval)) {
    mesh.sendBroadcast(String("Left:" + String(mesh.getNodeId())));  // Send ID to server
    Serial.println("Broadcasting ack to server...");
    Serial.println(String(ackReceived));
    Serial.println("Left:" + String(mesh.getNodeId()));
    lastSent = millis();
  }

  if (ackReceived && millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    Serial.println("Sending heartbeat to server...");
    lastHeartbeat = millis();
  }

  if (lastHeartbeatReceived > HEARTBEAT_TIMEOUT) {
    lastHeartbeatReceived = millis();
    heartbeatTimeoutCounter++;
    ackReceived = false;
    Serial.println("Heartbeat failed... Retrying Ack");
  }

  if (heartbeatTimeoutCounter >= 1 ) {
    mute(1);
    delay(1000);
    mute(0);
    delay(750);
    mute(1);
    delay(1000);
    mute(0);
    delay(750);
    mute(1);
    Serial.println("2 heartbeat timeouts in a row.. Resetting server ack...");
    ackReceived = false;
  }
}

void sendHeartbeat() {
  // Create the heartbeat message with current volume, sub levels, and mute state
  String heartbeatMessage = "HeartbeatLeft:" + String(currentVolume) + ":" + String(currentSubVolume) + ":" + String(muteState) + ":" + String(currentSource);

  // Send the heartbeat message to the server
  mesh.sendSingle(serverId, heartbeatMessage);

  // Log the heartbeat message
  Serial.println("Sent heartbeat message: " + heartbeatMessage);
}


void handleCallback(uint32_t from, String &msg) {
  String command = getValue(msg, ':', 0);
  String value1 = getValue(msg, ':', 1);
  String value2 = getValue(msg, ':', 2);
  String value3 = getValue(msg, ':', 3);
  String value4 = getValue(msg, ':', 4);
  Serial.println("COMMAND VALUES " + command + " " + value1 + " " + value2 + " " + value3 + " " + value4);
  if (from == serverId || command.equals("Ack")) {
    if (command.equals("Volume")) {
      setVolume(value1.toFloat());
      mesh.sendSingle(from, "VolumeACK:" + String(currentVolume));
      Serial.println("Volume changed to " + String(currentVolume));
    } else if (command.equals("Sub")) {
      setSubVolume(value1.toFloat());  // Using value2
      mesh.sendSingle(from, "SubACK:" + String(currentSubVolume));
      Serial.print("Sub Volume Changed to " + String(currentSubVolume));
    } else if (command.equals("Mute")) {
      mute(value1.toInt());
      mesh.sendSingle(from, "Mute:" + String(muteState));
      Serial.println("Mute set to " + String(muteState));
    } else if (command.equals("Ack")) {
      ackReceived = true;
      serverId = from;
      setVolume(value1.toFloat());
      setSubVolume(value2.toFloat());
      mute(value3.toInt());
      changeSource(value4.toInt());
      Serial.println("Server ack complete.");
      Serial.println("Volume: " + String(value1));
      Serial.println("Sub Volume: " + String(value2));
    } else if (command.equals("heartBeatGOOD")) {
        lastHeartbeat = millis();
        heartbeatTimeoutCounter = 0;
        Serial.println("Heartbeat GOOD");
    } else if (command.equals("heartBeatBAD")) {
        setVolume(value1.toFloat());
        setSubVolume(value2.toFloat());  // Assuming you've extracted value3 as well
        mute(value3.toInt());
        changeSource(value4.toInt());
        Serial.println("Heartbeat BAD!");
        Serial.println("Volume set to " + String(currentVolume));
        Serial.println("Sub Volume set to " + String(currentSubVolume));
        Serial.println("Mute set to " + String(muteState));
        Serial.println("Source set to " + String(currentSource));
    } else if (command.equals("Source")){
        changeSource(value1.toInt());
        mesh.sendSingle(from, "SourceACK:" + String(currentSource));
        Serial.println("Source changed to " + String(currentSource));
    }else{
        Serial.println("Unknown Command...");
    }
  }
}


String getValue(const String& data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  uint maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void setVolume(float newVolume) {
  dsp->volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, newVolume, 12);
  currentVolume = newVolume;
}

void mute(int newMuteState) {  // Toggle mute state
  if (newMuteState == 0){
    dsp->mute(MOD_MUTE1_MUTENOSLEWALG1MUTE_ADDR, false);
  }else if (newMuteState == 1){
    dsp->mute(MOD_MUTE1_MUTENOSLEWALG1MUTE_ADDR, true);
  }
  muteState = newMuteState;
}

void setSubVolume(float newSubVolume) {
  dsp->volume_slew(MOD_SWVOL2_ALG0_TARGET_ADDR, newSubVolume, 12);
  currentSubVolume = newSubVolume;
}

void changeSource(int newSource) {
    dsp->mux(MOD_NX2_1_STEREOSWSLEW_ADDR, newSource-1);
    currentSource = newSource;
}

void sendMessage() {
    String msg = "Left:";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    handleCallback(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
    ackReceived = false;
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}