//
// Created by Tanner on 10/22/2023.
//
#define INCLUDE_METHOD_MAP_LINEAR_TO_DB
#include "SharedVariables.h"
#include "DEFINES.h"
#include <Handlers.h>

void initializeVariables() {

//starting variables for DSP

    currentLinearVolume = START_VOLUME;
    currentVolumeDb = mapLinearToDb(START_VOLUME);
    currentLinearSubVolume = START_SUB_VOLUME;
    currentSubVolumeDb = mapLinearToDb(START_SUB_VOLUME);
    muteState = START_MUTE;
    currentSource = START_SOURCE;



//websocket variables

    myID = 0;
    leftId = 0;
    rightId = 0;
    leftAck = false;
    rightAck = false;
    leftTime = false;
    rightTime = false;
    timeSync = false;

//init and command tracking variables

    leftInit = false;
    rightInit = false;
    lastLeftInitTime = 0;
    lastRightInitTime = 0;
    lastMessageLeft = "";
    lastMessageRight = "";
    lastSendTimeLeft = 0;
    lastSendTimeRight = 0;
    retryCountLeft = 0;
    retryCountRight = 0;
}

