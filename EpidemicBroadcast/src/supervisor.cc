#include "supervisor.h"

Define_Module(Supervisor);

void Supervisor::initialize()
{
    cModule* floor = getParentModule();
    slotDuration = floor->par("slotDuration").doubleValue();
    nSlot2Wait = floor->par("nSlot2Wait");

    ready = false;
    activeNodes = 0;
    syncMsg = new cMessage("sync");
    waitMsg = new cMessage("wait");
    scheduleAt(simTime()+(slotDuration*(nSlot2Wait+1)), syncMsg);
}

void Supervisor::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        if(!ready) ready = true;
        else if(msg->isName("wait") && activeNodes == 0) endSimulation();
    }

    else {
        if(msg->isName("inc")) {
            activeNodes += 1;
            EV<<"SUPERVISOR: inc->"<<activeNodes<<"\n";
        }

        else if(msg->isName("dec")) {
            activeNodes -= 1;
            EV<<"SUPERVISOR: dec->"<<activeNodes<<"\n";
        }
    }

    if(ready && activeNodes == 0) {
        scheduleAt(simTime()+(slotDuration*(nSlot2Wait+1)), waitMsg);
    }
}

