////
//// Created by Tanner on 12/24/2023.
////
//
//#include "audioOut.h"
//#include "Communication/ESPNowStream.h"
//#include "Communication/UDPStream.h"
//#include "AudioLibs/AudioA2DP.h"
//
//using namespace audio_tools;
//
//IPAddress udpAddress(192, 168, 1, 255);
//const int udpPort = 7000;
//
//A2DPStream in;
//WiFiClient espClient;
//MeasuringStream clientTimed(espClient);
//StreamCopy copier(clientTimed, in);
//
//
//void setup() {
//    Serial.begin(115200);
//
//    // start the bluetooth audio receiver
//    Serial.println("starting A2DP...");
//    auto cfg = in.defaultConfig(RX_MODE);
//    cfg.name = "MyReceiver";
//    in.begin(cfg);
//}
//
//// Arduino loop
//void loop() {
//    copier.copy();
//}
