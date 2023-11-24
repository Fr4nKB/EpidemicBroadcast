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

        //signals
        simsignal_t simTimeEndSig;

        //pointers
        cMessage* waitMsg = nullptr;

        //status
        double endSimTime = 0.0;

        //counters
        int activeUsers = 0;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

#endif
