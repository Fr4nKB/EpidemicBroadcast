#ifndef __EPIDEMICBROADCAST_USER_H_
#define __EPIDEMICBROADCAST_USER_H_

#include <omnetpp.h>

using namespace omnetpp;

class User : public cSimpleModule
{
    private:
        //parameters
        double slotDuration = 1.0;
        int nSlot2Wait = 0;
        int maxMsgCopies = 0;
        int producerIndex = 0;

        //signals
        simsignal_t covered;
        simsignal_t collisionCounter;

        //pointers
        cMessage* timeMsg = nullptr;
        cMessage* msgToRelay = nullptr;

        //status
        bool finished = false;
        int msgRcvAtSlot = -1;

        //counters
        int elapsedTimeSlots = 0;
        int msgRcvInCurrSlot = 0;
        int totCopies = 0;

        //methods
        void handleTimeSlot(cMessage*);
        void broadcast();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

};

#endif
