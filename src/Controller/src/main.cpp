#include <IRrecv.h>
#include <painlessMesh.h>
#include <BluetoothA2DPSink.h>
#include <../lib/AudioBuffer.h>

#define MESH_PREFIX "LivingRoom"
#define MESH_PASSWORD "PassworD1234"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;
BluetoothA2DPSink a2dp_sink;
AudioBuffer audioBuffer(4096);

// Assuming you have an IR receiver connected to pin 2
const int IR_PIN = 26;
IRrecv irrecv(IR_PIN);
decode_results results;

const uint32_t volumeUpCode = 0xF7B847;
const uint32_t volumeDownCode = 0xF758A7;
const uint32_t muteCode = 0xF7C03F;
const uint32_t subUpCode = 0xF738C7;
const uint32_t subDownCode = 0xF77887;
const uint32_t sourceCode = 0xF728D7;
const uint64_t repeatCode = 0xFFFFFFFFFFFFFF;


// Constants
const float MAX_LINEAR_VOLUME = 30;
const float MIN_LINEAR_VOLUME = 1;
const float MAX_DB_VOLUME = 0.0;
const float MIN_DB_VOLUME = -30.0;


// Variables
float currentLinearVolume = 20;
float currentVolumeDb = -10.34;
float currentLinearSubVolume = 30;
float currentSubVolumeDb = 0;
long lastOperation = 0;
unsigned long lastIRReceivedTime = 0;
const unsigned long debounceDelay = 200;
int muteState = 0;
int currentSource = 2;

unsigned long heartbeatLastReceivedLeft = 0;
unsigned long heartbeatLastReceivedRight = 0;

unsigned long lastSendTimeLeft = 0;
unsigned long lastSendTimeRight = 0;
int retryCountLeft = 0;
int retryCountRight = 0;
String lastMessageLeft = "";
String lastMessageRight = "";

bool leftAck = false;
uint32_t leftId = 0;
bool rightAck = false;
uint32_t rightId = 0;
bool leftInit = false;
bool rightInit = false;
unsigned long lastLeftInitMes = 0;
unsigned long lastRightInitMes = 0;
const unsigned long initDebounce = 3000;
uint32_t myNodeId = 0;
bool leftTime = false;
bool rightTime = false;

// Prototypes so PlatformIO doesn't complain
void handleIRCode(uint32_t);

void handleCallback(uint32_t, String &);

void volume(float);

void sub(float);

void mute(int);

String getValue(const String &data, char separator, int index);

void sendCommandToClients(const String &command, const String &value);

float mapLinearToDb(float);

void sendMessage();

void receivedCallback(uint32_t, String &);

void newConnectionCallback(uint32_t);

void changedConnectionCallback();

void nodeTimeAdjustedCallback(int32_t);

void source(int);

void parseIncomingMessage(String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
                          String &value5, String &value6, String &value7);

void handleVolumeAck(uint32_t from, const String &value1);

void handleSubVolumeAck(uint32_t from, const String &value1);

void
handleHeartbeat(uint32_t from, const String &value1, const String &value2, const String &value3, const String &value4);

void handleInit(uint32_t from, const String &command);

void handleMute(uint32_t from, const String &value1);

void handleSource(uint32_t from, const String &value1);

void handleTime(uint32_t from, const String &value1);

void sendToMesh(const String &data);

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

Task taskBroadcastTime(TASK_SECOND * 10, TASK_FOREVER, []() {
    // This lambda function is your task.
    uint64_t currentTime = mesh.getNodeTime();
    String timeMessage = "Time:" + String(currentTime);
    mesh.sendBroadcast(timeMessage);
});

void read_data_stream(const uint8_t *data, uint32_t length) {
    // Add incoming audio data to the buffer
    if (!audioBuffer.addData(data, length)) {
        // Handle buffer overflow, if you need to
    }

    // Check if the buffer is full and ready for packetization
    if (audioBuffer.isFull()) {
        AudioPacket packet = audioBuffer.packetize();
        if (packet.isValid) {  // make sure it's a valid packet
            // Convert packet data to String and send
            String audioDataToSend = String((char *) packet.data, packet.size);
            sendToMesh(audioDataToSend);
        }
        audioBuffer.clear();  // Clear the buffer for the next set of data
    }
}


void setup() {
    Serial.begin(115200);

    mesh.setDebugMsgTypes(
            ERROR | STARTUP | CONNECTION | DEBUG);  // set before init() so that you can see startup messages
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    userScheduler.addTask(taskBroadcastTime);
    taskBroadcastTime.enable();

    myNodeId = mesh.getNodeId();

    Serial.println("Mesh Node ID:");
    Serial.println(myNodeId);

    irrecv.enableIRIn();  // Start the IR receiver


    currentVolumeDb = mapLinearToDb(20);
    currentSubVolumeDb = mapLinearToDb(25);
    muteState = 0;
    a2dp_sink.start("MyMusic");
    a2dp_sink.set_stream_reader(read_data_stream);
}

void loop() {

    mesh.update();

    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }

    if (irrecv.decode(&results)) {
        unsigned long currentIRReceivedTime = millis();

        // If it's a repeat code, replay the last operation
        if (results.value == repeatCode && lastOperation != 0) {
            if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                handleIRCode(lastOperation);
            }
        } else {
            // If it's not a repeat code and enough time has passed (debouncing)
            if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                lastOperation = (long) results.value;
                handleIRCode(results.value);
            }
        }
        lastIRReceivedTime = currentIRReceivedTime;
        irrecv.resume();
    }
//Check for command ack. Retry sending the command if no ack is received within 500ms
    if (!leftAck && millis() - lastSendTimeLeft > 500 && retryCountLeft < 5) {
        mesh.sendSingle(leftId, lastMessageLeft);
        lastSendTimeLeft = millis();
        retryCountLeft++;
    }

    if (!rightAck && millis() - lastSendTimeRight > 500 && retryCountRight < 5) {
        mesh.sendSingle(rightId, lastMessageRight);
        lastSendTimeRight = millis();
        retryCountRight++;
    }

    if (rightAck) {
        retryCountRight = 0;
    }

    if (leftAck) {
        retryCountLeft = 0;
    }

//init debug Serial print
    if (!leftInit && millis() - lastLeftInitMes > initDebounce) {
        Serial.println("Waiting for Left Init...");
        lastLeftInitMes = millis();
    }

    if (!rightInit && millis() - lastRightInitMes > initDebounce) {
        Serial.println("Waiting for Right Init...");
        lastRightInitMes = millis();
    }
}

float mapLinearToDb(float linearVolume) {
    return (linearVolume - MIN_LINEAR_VOLUME) * (MAX_DB_VOLUME - MIN_DB_VOLUME) /
           (MAX_LINEAR_VOLUME - MIN_LINEAR_VOLUME) + MIN_DB_VOLUME;
}


void handleIRCode(uint32_t code) {
    Serial.println(code, HEX);
    switch (code) {
        case volumeUpCode:
            if (currentLinearVolume < MAX_LINEAR_VOLUME) {
                currentLinearVolume++;
                volume(currentLinearVolume);
                Serial.println("Volume Up: " + String(currentLinearVolume));
            } else {
                volume(currentLinearVolume);
                Serial.println("Volume already at max.");
            }
            break;
        case volumeDownCode:
            if (currentLinearVolume > MIN_LINEAR_VOLUME) {
                currentLinearVolume--;
                volume(currentLinearVolume);
                Serial.println("Volume Down: " + String(currentLinearVolume));
            } else {
                volume(currentLinearVolume);
                Serial.println("Volume already at min");
            }
            break;
        case muteCode:
            if (muteState == 0) {
                mute(1);
                muteState = 1;
                Serial.println("Mute ON");
            } else {
                mute(0);
                muteState = 0;
                Serial.println("Mute OFF");
            }
            break;
        case subUpCode:
            if (currentLinearSubVolume < MAX_LINEAR_VOLUME) {
                currentLinearSubVolume++;
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume Up: " + String(currentLinearSubVolume));
            } else {
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume already at max");
            }
            break;
        case subDownCode:
            if (currentLinearSubVolume > MIN_LINEAR_VOLUME) {
                currentLinearSubVolume--;
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume Down: " + String(currentLinearSubVolume));
            } else {
                sub(currentLinearSubVolume);
                Serial.println("Sub volume already at min");
            }
            break;
        case sourceCode:
            if (currentSource == 1) {
                source(2);
                Serial.println("Changed to source 2 (Line in)");
            } else if (currentSource == 2) {
                source(1);
                Serial.println("Changed to source 1 (Optical)");
            }
            break;
        default:
            Serial.println("Unknown Code");
            break;
    }
}

void handleCallback(uint32_t from, String &msg) {

    String command, value1, value2, value3, value4, value5, value6, value7;
    parseIncomingMessage(msg, command, value1, value2, value3, value4, value5, value6, value7);
    Serial.println("COMMAND VALUES " + command + " " + value1 + " " + value2 + " " + value3 + " " + value4);
    if (command.equals("VolumeACK")) handleVolumeAck(from, value1);
    else if (command.equals("SubACK")) handleSubVolumeAck(from, value1);
    else if (command.equals("heartBeatLeft") || command.equals("heartBeatRight"))
        handleHeartbeat(from, value1, value2, value3, value4);
    else if (command.equals("Left") || command.equals("Right")) handleInit(from, command);
    else if (command.equals("Mute")) handleMute(from, value1);
    else if (command.equals("Source")) handleSource(from, value1);
    else if (command.equals("Time")) handleTime(from, value1);
    else Serial.println("Unknown Command...");
}

void handleTime(uint32_t from, const String &value1) {
    if (from == leftId) {
        if (value1.equals(String(mesh.getNodeTime()))) {
            leftTime = true;
        } else {
            Serial.println("Left time not equal");
            leftTime = false;
            mesh.sendSingle(leftId, "Time:" + String(mesh.getNodeTime()));
        }

    } else if (from == rightId) {
        if (value1.equals(String(mesh.getNodeTime()))) {
            rightTime = true;
        } else {
            Serial.println("Right time not equal");
            rightTime = false;
            mesh.sendSingle(rightId, "Time:" + String(mesh.getNodeTime()));
        }
    }

}

void handleSource(uint32_t from, const String &value1) {
    if (from == leftId) {
        if (value1.equals(String(currentSource))) {
            leftAck = true;
        }
    } else if (from == rightId) {
        if (value1.equals(String(currentSource))) {
            rightAck = true;
        }
    }


}

void handleMute(uint32_t from, const String &value1) {
    if (from == leftId) {
        if (value1.equals(String(muteState))) {
            leftAck = true;
        }
    } else if (from == rightId) {
        if (value1.equals(String(muteState))) {
            rightAck = true;
        }
    }


}

void handleInit(uint32_t from, const String &command) {
    if (command.equals("Left") && !leftInit) {
        leftId = from;
        leftInit = true;
        mesh.sendSingle(leftId,
                        "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                        ":" + String(currentSource));
        Serial.println("Left Init complete!");
    } else if (command.equals("Right") && !rightInit) {
        rightId = from;
        rightInit = true;
        mesh.sendSingle(rightId,
                        "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                        ":" + String(currentSource));
        Serial.println("Right Init complete!");
    }
}

void
handleHeartbeat(uint32_t from, const String &value1, const String &value2, const String &value3, const String &value4) {
    if (from == leftId) {
        heartbeatLastReceivedLeft = millis();
        if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
            value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
            mesh.sendSingle(leftId, "heartBeatGOOD");
            Serial.println("Left Heartbeat GOOD");
        } else {
            mesh.sendSingle(leftId, "heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                                    String(muteState) + ":" + String(currentSource));
            Serial.println("Left Heartbeat BAD");
            Serial.println(String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) + ":" +
                           String(currentSource));
        }
    } else if (from == rightId) {
        heartbeatLastReceivedRight = millis();
        if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
            value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
            mesh.sendSingle(rightId, "heartBeatGOOD");
            Serial.println("Right Heartbeat GOOD");
        } else {
            mesh.sendSingle(rightId,
                            "heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                            String(muteState) + ":" + String(currentSource));
            Serial.println("Right Heartbeat BAD");
            Serial.println(String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) + ":" +
                           String(currentSource));
        }
    }

}

void handleSubVolumeAck(uint32_t from, const String &value1) {
    if (from == leftId) {
        if (value1.equals(String(currentSubVolumeDb))) {
            leftAck = true;
        }
    } else if (from == rightId) {
        if (value1.equals(String(currentSubVolumeDb))) {
            rightAck = true;
        }
    }

}

void handleVolumeAck(uint32_t from, const String &value1) {
    if (from == leftId) {
        if (value1.equals(String(currentVolumeDb))) {
            leftAck = true;
        }
    } else if (from == rightId) {
        if (value1.equals(String(currentVolumeDb))) {
            rightAck = true;
        }
    }

}

void parseIncomingMessage(String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
                          String &value5, String &value6, String &value7) {
    command = getValue(msg, ':', 0);
    value1 = getValue(msg, ':', 1);
    value2 = getValue(msg, ':', 2);
    value3 = getValue(msg, ':', 3);
    value4 = getValue(msg, ':', 4);
    value5 = getValue(msg, ':', 5);
    value6 = getValue(msg, ':', 6);
    value7 = getValue(msg, ':', 7);
}

void volume(float newLinearVolume) {
    currentVolumeDb = mapLinearToDb(newLinearVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClients("Volume", String(currentVolumeDb));
    currentLinearVolume = newLinearVolume;
    Serial.print("Volume UP: ");
    Serial.println(String(currentVolumeDb) + "Db");
}

void sub(float newLinearSubVolume) {
    currentSubVolumeDb = mapLinearToDb(newLinearSubVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClients("Sub", String(currentSubVolumeDb));
    currentLinearSubVolume = newLinearSubVolume;
}

void mute(int newMuteState) {
    leftAck = false;
    rightAck = false;
    sendCommandToClients("Mute", String(newMuteState));
    muteState = newMuteState;
}

void source(int newSource) {
    leftAck = false;
    rightAck = false;
    sendCommandToClients("Source", String(newSource));
    currentSource = newSource;
}

String getValue(const String &data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
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

void sendCommandToClients(const String &command, const String &value) {
    String message = command + ":" + value;

    // Broadcast the message initially
    mesh.sendSingle(leftId, message);
    mesh.sendSingle(rightId, message);
    lastMessageLeft = message;
    lastMessageRight = message;
    lastSendTimeLeft = millis();
    lastSendTimeRight = millis();
    retryCountLeft = 0;
    retryCountRight = 0;
}

void sendToMesh(const String &data) {
    mesh.sendBroadcast(data);
}

void sendMessage() {
    String msg = "Server:";
    msg += mesh.getNodeId();
    mesh.sendBroadcast(msg);
    taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

void receivedCallback(uint32_t from, String &msg) {
    if (from == rightId) {
        Serial.print("Right:");
    } else if (from == leftId) {
        Serial.print("Left: ");
    }
    Serial.println(String(from) + ", " + String(msg));
    handleCallback(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
    leftAck = false;
    leftId = 0;
    rightAck = false;
    rightId = 0;
    leftInit = false;
    rightInit = false;
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

