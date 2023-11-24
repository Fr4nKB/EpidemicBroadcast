#include "supervisor.h"

Define_Module(Supervisor);

void Supervisor::initialize()
{
    cModule* floor = getParentModule();
    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait");

    ready = false;
    activeNodes = 0;

    simTimeEndSig = registerSignal("simTimeEndSig");

    syncMsg = new cMessage("sync");
    waitMsg = new cMessage("wait");
    scheduleAt(simTime()+(slotDuration*(nSlot2Wait+1)), syncMsg);
}

void Supervisor::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        if(!ready) ready = true;
        else if(msg->isName("wait") && activeNodes == 0) {
            emit(simTimeEndSig, endSimTime);
            delete syncMsg;
            delete waitMsg;
            endSimulation();
        }
    }

    else {
        if(msg->isName("inc")) {
            delete msg;
            activeNodes += 1;
            EV<<"SUPERVISOR: active nodes incremented to "<<activeNodes<<"\n";
        }

        else if(msg->isName("dec")) {
            delete msg;
            activeNodes -= 1;
            EV<<"SUPERVISOR: active nodes decremented to "<<activeNodes<<"\n";
        }
    }

    if(ready && activeNodes == 0) {
        endSimTime = (simTime()+slotDuration).dbl();
        scheduleAt(simTime()+(slotDuration*(nSlot2Wait+1)), waitMsg);
    }
}

