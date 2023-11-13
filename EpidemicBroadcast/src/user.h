//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

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
        bool msgRelayed = false;
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
