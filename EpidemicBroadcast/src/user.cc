#include "user.h"
#include <string>

Define_Module(User);

void User::initialize() {
    //retrieve parameters
    cModule* floor;
    try {
         floor = getParentModule();
    }
    catch(...) {
        EV_ERROR<<"initialize "+std::to_string(this->getIndex())+": Couldn't retrieve floor module\n";
        return;
    }

    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait");
    maxMsgCopies = floor->par("maxMsgCopies");
    producerIndex = floor->par("producerIndex");

    //if module is producer send message
    if(this->getIndex() == producerIndex){
        msgToRelay = new cMessage("Hello!");
        broadcast();
        msgRelayed = true;
        emit(covered, 1);
        return;
    }

    covered = registerSignal("covered");
    collisionCounter = registerSignal("collision");

    //empty message to slot time
    timeMsg = new cMessage();
    scheduleAt(simTime(), timeMsg);
}

void User::handleMessage(cMessage *msg) {
    //user has already done its job
    if(msgRelayed) {
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
    }
}


void User::handleTimeSlot(cMessage* msg) {

    if(msgRcvAtSlot == -1) {
        //message correctly received in current slot
        if(msgRcvInCurrSlot == 1) {
            //wait "nSlot2Wait" to broadcast message from the current slot (excluded, hence +1)
            msgRcvAtSlot = elapsedTimeSlots + 1;
            emit(covered, 1);
            EV<<"handleCustomMsg (USER "+std::to_string(this->getIndex())+"): message correctly received\n";
        }

        //otherwise save collision
        else if(msgRcvInCurrSlot > 1){
            emit(collisionCounter, msgRcvInCurrSlot);
            EV<<"handleCustomMsg (USER "+std::to_string(this->getIndex())+"): collision\n";
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
                msgRelayed = true;
                EV<<"handleCustomMsg (USER "+std::to_string(this->getIndex())+"): message broadcasted\n";
            }

            else {
                EV<<"handleCustomMsg (USER "+std::to_string(this->getIndex())+"): to many copies of the messages\n";
                delete msgToRelay;
                msgToRelay = nullptr;
            }

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

    int nGates = this->gateCount()/2;

    for(int i = 0; i < nGates; i++) {
        cMessage* copy = msgToRelay->dup();
        send(copy, "gateOUT", i);
    }

    delete msgToRelay;
    msgToRelay = nullptr;
}
