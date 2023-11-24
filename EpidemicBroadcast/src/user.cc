#include "user.h"

using namespace std;

Define_Module(User);

void User::initialize() {
    //retrieve parameters
    cModule* floor = getParentModule();
    int nUsers = (floor->par("userNumber").intValue());

    //retrieve supervisor
    supervisor = floor->getSubmodule("sup");

    //retrieve all the other parameters
    coverageValue = 1.0/nUsers;
    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait").intValue();
    maxMsgCopies = floor->par("maxMsgCopies").intValue();
    producerIndex = floor->par("producerIndex").intValue();
    radiusSquared = floor->par("radius").doubleValue();
    radiusSquared *= radiusSquared;

    //retrieve all the users
    double coordX = this->par("coordX").doubleValue();
    double coordY = this->par("coordY").doubleValue();
    double diffX = 0.0, diffY = 0.0;
    users.clear();
    cModule* tmpUser;
    for(int i = 0; i < nUsers; i++) {
        if(i == this->getIndex()) continue;     //skip current user

        tmpUser = floor->getSubmodule("user", i);
        diffX = coordX - tmpUser->par("coordX").doubleValue();
        diffY = coordY - tmpUser->par("coordY").doubleValue();
        if((diffX*diffX + diffY*diffY) <= radiusSquared) users.push_back(tmpUser);
    }

    //register signals
    covered = registerSignal("covered");
    collisionCounter = registerSignal("collision");
    TimeStampedCollision = cTimestampedValue();

    //if module is producer send message
    if(this->getIndex() == producerIndex){
        msgToRelay = new cMessage("Hello!");
        broadcast();
        finished = true;
        emit(covered, coverageValue);
        return;
    }

    //empty message to slot time
    timeMsg = new cMessage();
    scheduleAt(simTime(), timeMsg);
}

void User::handleMessage(cMessage *msg) {
    //user has already done its job
    if(finished) {
        return;
    }

    //slot time message, update counters
    if(msg->isSelfMessage()) {
        handleTimeSlot(msg);
    }

    //handle message from another user
    else {
        msgRcvInCurrSlot += 1;
        if(msgToRelay == nullptr) msgToRelay = msg->dup();
        delete msg;
    }
}

void User::handleTimeSlot(cMessage* msg) {

    if(msgRcvAtSlot == -1) {
        //message correctly received in current slot
        if(msgRcvInCurrSlot == 1) {
            //wait "nSlot2Wait" to broadcast message from the current slot (excluded, hence +1)
            msgRcvAtSlot = elapsedTimeSlots + 1;
            emit(covered, coverageValue);
            sendDirect(new cMessage("inc"), supervisor, supervisor->gateBaseId("dirInGate"));
            EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): message correctly received\n";
        }

        //otherwise save collision
        else if(msgRcvInCurrSlot > 1){
            delete msgToRelay;
            msgToRelay = nullptr;
            TimeStampedCollision.set(simtime_t(elapsedTimeSlots), uintval_t(msgRcvInCurrSlot));
            emit(collisionCounter, &TimeStampedCollision);
            EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): collision\n";
        }
    }

    //message already received, waiting for nSlot2Wait before relaying
    else {
        totCopies += msgRcvInCurrSlot;

        //maximum time slots to wait reached
        if(elapsedTimeSlots == msgRcvAtSlot + nSlot2Wait) {

            //message is sent only if number of message copies are below maximum
            if(totCopies < maxMsgCopies) {
                broadcast();
                EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): message broadcasted\n";
            }

            else {
                EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): to many copies of the messages\n";
                delete msgToRelay;
                msgToRelay = nullptr;
            }

            sendDirect(new cMessage("dec"), supervisor, supervisor->gateBaseId("dirInGate"));
            finished = true;
            return;
        }
    }

    msgRcvInCurrSlot = 0;
    elapsedTimeSlots += 1;
    scheduleAt(simTime()+slotDuration, timeMsg);
}

//relay message to neighbors
void User::broadcast() {
    if(msgToRelay == nullptr) return;

    int size = users.size();
    for(int i = 0; i < size; i++) {
        sendDirect(msgToRelay->dup(), users[i], users[i]->gateBaseId("dirInGate"));
    }

    delete msgToRelay;
    msgToRelay = nullptr;
}
