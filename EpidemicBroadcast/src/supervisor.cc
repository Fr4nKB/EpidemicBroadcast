#include "supervisor.h"
using namespace std;

Define_Module(Supervisor);

void Supervisor::initialize() {
    //retrieve parent
    cModule* floor = getParentModule();

    //retrieve parameters
    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait");

    //register signals
    simTimeEndSig = registerSignal("simTimeEndSig");

    //wait a little (2 slots) before starting the supervisor to avoid ending simulation earlier
    activeUsers = 0;
    waitMsg = new cMessage();
    scheduleAt(simTime()+(slotDuration*2), waitMsg);
}

void Supervisor::handleMessage(cMessage *msg) {
    //check again if no user is active as the number might have changed since the scheduling of waitMsg
    if(msg->isSelfMessage() && activeUsers == 0) {
        emit(simTimeEndSig, endSimTime);
        delete waitMsg;
        endSimulation();
    }

    else {
        if(msg->isName("inc")) {
            delete msg;
            activeUsers += 1;
            EV<<"SUPERVISOR: active nodes incremented to "<<activeUsers<<"\n";
        }
        else if(msg->isName("dec")) {
            delete msg;
            activeUsers -= 1;
            EV<<"SUPERVISOR: active nodes decremented to "<<activeUsers<<"\n";
        }

        //supposedly no user can relay message
        if(activeUsers == 0) {
            endSimTime = (simTime()+slotDuration).dbl();
            //wait the time window before ending simulation as some user might still have to communicate their status
            cancelEvent(waitMsg);
            scheduleAt(simTime()+(slotDuration*(nSlot2Wait+1)), waitMsg);
        }
    }
}

