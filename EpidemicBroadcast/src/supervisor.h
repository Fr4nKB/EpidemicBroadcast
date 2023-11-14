#ifndef __EPIDEMICBROADCAST_SUPERVISOR_H_
#define __EPIDEMICBROADCAST_SUPERVISOR_H_

#include <omnetpp.h>

using namespace omnetpp;

class Supervisor : public cSimpleModule
{
    private:
        //parameters
        double slotDuration = 1.0;
        int nSlot2Wait = 0;

        //status
        bool ready = false;

        //pointers
        cMessage* syncMsg = nullptr;
        cMessage* waitMsg = nullptr;
        cMessage* endMsg = nullptr;

        //counters
        int activeNodes = 0;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

#endif
