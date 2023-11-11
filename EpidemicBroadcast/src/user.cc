#include "user.h"

Define_Module(User);

User::User() : cSimpleModule() {
    //pointers
    msgToRelay = nullptr;

    //status
    msgRelayed = false;
    collision = false;

    //counters
    elapsedTimeSlots = 0;
}

void User::initialize() {
    //retrieve parameters
    try{
        cModule* floor = getParentModule();
    }
    catch(e) {
        EV_ERROR<<"USER-INITIALIZE: Couldn't retrieve floor module\n";
    }

    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait");
    maxMsgCopies = floor->par("maxMsgCopies");
    nSlot2Wait = floor->par("nSlot2Wait");
    producerIndex = floor->par("producerIndex");

    int msgCopies[maxMsgCopies];    //array to count message copies and collisions
    std::fill(msgCopies, msgCopies + maxMsgCopies, 0);  //initialize to 0

    //if module is producer send message
    try{
        if(this->getIndex() == producerIndex){
            msgToRelay = new cMessage("Hello!");
            broadCast();
            msgRelayed = true;
        }
    }
    catch(e) {
        EV_ERROR<<"USER-INITIALIZE: Couldn't retrieve module index in initialize method\n";
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
        handleSlotMsg(msg);
    }

    //handle message from another user
    else {
        msgCopies[elapsedTimeSlots] += 1;
        msgToRelay = msg->dup();
    }
}

void User::handleSlotMsg(cMessage* msg) {

    if(elapsedTimeSlots < nSlot2Wait) {
        if(msgCopies[elapsedTimeSlots] > 1) {
            collision = true;
        }
    }
    else if(elapsedTimeSlots == nSlot2Wait){
        int totCopies = 0, indexNoCollision = -1;
        for(int i = 0; i < maxMsgCopies; i++) {
            totCopies += msgCopies[i];
            if(msgCopies[i] == 1) indexNoCollision = i;
        }
        if(totCopies < maxMsgCopies && indexNoCollision != -1) {
            broadcast();
            msgRelayed = true;
        }
        else {
            toManyMessage = true;
        }
    }

    elapsedTimeSlots += 1;
    scheduleAt(simTime()+slotDuration, timeMsg);
}

//relay message to neighbors
void User::broadcast() {
    int nGates = this->gateCount()/2;

    for(int i = 0; i < nGates; i++) {
        cMessage* copy = msgToRelay->dup();
        send(copy, "gate$o",i);
    }

    delete msg;
}
