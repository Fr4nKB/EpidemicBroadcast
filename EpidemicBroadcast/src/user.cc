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
        EV_ERROR<<"initialize: Couldn't retrieve floor module\n";
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
        EV_ERROR<<"initialize: Couldn't retrieve module index in initialize method\n";
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
        if(msgToRelay == nullptr) msgToRelay = msg->dup();
    }
}

void User::handleSlotMsg(cMessage* msg) {

    //check for collisions
    if(elapsedTimeSlots < nSlot2Wait) {
        if(msgCopies[elapsedTimeSlots] > 1) {
            collision = true;
            EV_WARNING<<"handleSlotMsg: collision\n";
        }
    }
    //maximum time slots to wait reached
    else if(elapsedTimeSlots == nSlot2Wait){

        int totCopies = 0, indexNoCollision = -1;
        for(int i = 0; i < maxMsgCopies; i++) {
            totCopies += msgCopies[i];
            if(msgCopies[i] == 1) indexNoCollision = i; //one slot without collision is sufficient
        }

        //message is sent only if there's at least one slot with no collisions and if number of message copies are below maximum
        if(totCopies < maxMsgCopies) {

            if(indexNoCollision != -1) {
                broadcast();
                msgRelayed = true;
            }

            //reset counters, user can still relay the message
            else {
                EV_WARNING<<"handleSlotMsg: max copies not reached but too many collisions\n";
                collision = false;
                elapsedTimeSlots = -1;  //set to -1 so at end of the method it's set to 0
                std::fill(msgCopies, msgCopies + maxMsgCopies, 0);
            }
        }

        else {
            toManyMessage = true;
            EV_WARNING<<"handleSlotMsg: to many copies of the messages\n";
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

    delete msgToRelay;
}
