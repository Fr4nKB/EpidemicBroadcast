#ifndef __EPIDEMICBROADCAST_USER_H_
#define __EPIDEMICBROADCAST_USER_H_

#include <string>
#include <vector>
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
        double radiusSquared = 0.0;

        //signals
        cTimestampedValue TimeStampedCollision;
        simsignal_t covered;
        simsignal_t collisionCounter;

        //pointers
        cMessage* timeMsg = nullptr;
        cMessage* msgToRelay = nullptr;
        cModule* supervisor = nullptr;
        std::vector<cModule*> users;

        //status
        double coverageValue = 0.0;
        bool finished = false;
        int msgRcvAtSlot = -1;
        unsigned int base = 0;

        //counters
        unsigned int elapsedTimeSlots = 0;
        unsigned int msgRcvInCurrSlot = 0;
        unsigned int totCopies = 0;

        //methods
        void handleTimeSlot(cMessage*);
        void broadcast();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

};

#endif
