#include "user.h"
#include <string>

Define_Module(User);

User::User():cSimpleModule() {
    //pointers
    msgToRelay = nullptr;

    //status
    msgRelayed = false;

    //counters
    elapsedTimeSlots = 0;
}

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
    msgCopies[0] = 0;

    //if module is producer send message
    if(this->getIndex() == producerIndex){
        msgToRelay = new cMessage("Hello!");
        broadcast();
        msgRelayed = true;
        return;
    }

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
        handleCustomMsg(msg);
    }

    //handle message from another user
    else {
        msgCopies[elapsedTimeSlots] += 1;
        if(msgToRelay == nullptr) msgToRelay = msg->dup();
    }
}

void User::handleCustomMsg(cMessage* msg) {

    //check for collisions
    if(elapsedTimeSlots < nSlot2Wait) {
        if(msgCopies[elapsedTimeSlots] > 1) {
            EV<<"handleSlotMsg "+std::to_string(this->getIndex())+": collision\n";
        }

        elapsedTimeSlots += 1;
        msgCopies[elapsedTimeSlots] = 0;    //create new element
        scheduleAt(simTime()+slotDuration, timeMsg);
    }

    //maximum time slots to wait reached
    else if(elapsedTimeSlots == nSlot2Wait){

        int totCopies = 0;
        bool indexNoCollision = false;

        for(auto i: msgCopies) {
            totCopies += i.second;
            if(i.second == 1) indexNoCollision = true; //one slot without collision is sufficient
        }

        //message is sent only if there's at least one slot with no collisions and if number of message copies are below maximum
        if(totCopies < maxMsgCopies) {

            if(indexNoCollision) {
                broadcast();
                msgRelayed = true;
            }

            //reset counters, user can still relay the message
            else {
                EV<<"handleSlotMsg "+std::to_string(this->getIndex())+": max copies not reached but too many collisions\n";
                elapsedTimeSlots = -1;  //set to -1 so at end of the method it's set to 0
                msgCopies.clear();
                msgCopies[0] = 0;
            }

        }

        else {
            toManyMsg = true;
            EV<<"handleSlotMsg: to many copies of the messages\n";
        }

        if(msgToRelay != nullptr) {
            delete msgToRelay;
            msgToRelay = nullptr;
        }
    }
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
