#include "user.h"
using namespace std;

Define_Module(User);

void User::initialize() {
    //retrieve parent
    cModule* floor = getParentModule();

    //retrieve supervisor
    supervisor = floor->getSubmodule("sup");

    //retrieve all the other parameters
    int nUsers = (floor->par("userNumber").intValue());
    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait").intValue();
    maxMsgCopies = floor->par("maxMsgCopies").intValue();
    producerIndex = floor->par("producerIndex").intValue();
    radiusSquared = floor->par("radius").doubleValue();
    radiusSquared *= radiusSquared;

    //build neighbors' list
    double coordX = this->par("coordX").doubleValue();
    double coordY = this->par("coordY").doubleValue();
    double diffX = 0.0, diffY = 0.0;

    neighbors.clear();
    cModule* tmpUser;
    for(int i = 0; i < nUsers; i++) {
        if(i == this->getIndex()) continue;     //skip current user

        tmpUser = floor->getSubmodule("user", i);
        diffX = coordX - tmpUser->par("coordX").doubleValue();
        diffY = coordY - tmpUser->par("coordY").doubleValue();
        if((diffX*diffX + diffY*diffY) <= radiusSquared) neighbors.push_back(tmpUser);
    }

    //register signals
    covered = registerSignal("covered");
    coveredStatus = registerSignal("coveredStatus");
    collisionCounter = registerSignal("collision");
    TimeStampedCollision = cTimestampedValue();

    //if module is producer send message
    if(this->getIndex() == producerIndex){
        msgToRelay = new cMessage("Hello!");
        broadcast();
        emit(covered, 1);
        color = "blue";
        finished = true;
    }

    //otherwise time slotting
    else {
        timeMsg = new cMessage();
        scheduleAt(simTime(), timeMsg);
    }
}

void User::handleMessage(cMessage *msg) {

    if(!finished) {
        //slot time message
        if(msg->isSelfMessage()) handleTimeSlot(msg);

        //message from another user, update counter
        else {
            msgRcvInCurrSlot += 1;
            if(msgToRelay == nullptr) msgToRelay = msg->dup();  //save message only once
            delete msg;
        }
    }

    //eliminate messages received after having finished
    else if(!(msg->isSelfMessage())) delete msg;
}

void User::refreshDisplay()const {
    this->getDisplayString().setTagArg("b", 3, color.c_str());      //change color of user
}

void User::finish() {
    cancelAndDelete(timeMsg);   //remove slot message
}

//advances time slots, checks if user can broadcast
void User::handleTimeSlot(cMessage* msg) {

    if(msgRcvAtSlot == -1) {
        //message correctly received in current slot
        if(msgRcvInCurrSlot == 1) {
            msgRcvAtSlot = elapsedTimeSlots + 1;    //wait "nSlot2Wait" to broadcast message from the current slot excluded, hence +1

            //signal emission
            TimeStampedCollision.set(simtime_t(elapsedTimeSlots), uintval_t(0));
            emit(coveredStatus, &TimeStampedCollision);
            emit(covered, 1);

            //user signals to supervisor the ability to relay message
            sendDirect(new cMessage("inc"), supervisor, supervisor->gateBaseId("dirInGate"));

            //GUI and debug
            color = "green";
            EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): message correctly received\n";
        }

        //otherwise save collision
        else if(msgRcvInCurrSlot > 1){
            delete msgToRelay;
            msgToRelay = nullptr;

            TimeStampedCollision.set(simtime_t(elapsedTimeSlots), uintval_t(msgRcvInCurrSlot));
            emit(collisionCounter, &TimeStampedCollision);

            color = "white";
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
                TimeStampedCollision.set(simtime_t(elapsedTimeSlots), uintval_t(1));
                emit(coveredStatus, &TimeStampedCollision);
                EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): message broadcasted\n";
            }

            else {
                EV<<"handleCustomMsg (USER "+to_string(this->getIndex())+"): to many copies of the messages\n";
                delete msgToRelay;
                msgToRelay = nullptr;
                TimeStampedCollision.set(simtime_t(elapsedTimeSlots), uintval_t(2));
                emit(coveredStatus, &TimeStampedCollision);
            }

            //user in idle from now on
            sendDirect(new cMessage("dec"), supervisor, supervisor->gateBaseId("dirInGate"));   //user cannot relay message anymore
            color="orange";
            finished = true;
            return;
        }
    }

    //reset message counter, increment time slot
    msgRcvInCurrSlot = 0;
    elapsedTimeSlots += 1;
    scheduleAt(simTime()+slotDuration, timeMsg);
}

//relay message to neighbors
void User::broadcast() {
    if(msgToRelay == nullptr) return;

    int size = neighbors.size();
    for(int i = 0; i < size; i++) {
        sendDirect(msgToRelay->dup(), neighbors[i], neighbors[i]->gateBaseId("dirInGate"));
    }

    delete msgToRelay;
    msgToRelay = nullptr;
}
