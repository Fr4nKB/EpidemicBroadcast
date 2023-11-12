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

/**
 * TODO - Generated class
 */
class User : public cSimpleModule
{
    private:
        //parameters
        double slotDuration = 1.0;
        int nSlot2Wait = 0;
        int maxMsgCopies = 0;
        int producerIndex = 0;

        //pointers
        cMessage* timeMsg = nullptr;
        cMessage* msgToRelay = nullptr;

        //status
        bool msgRelayed = false;
        bool toManyMsg = false;
        std::map<int, int> msgCopies;

        //counters
        int elapsedTimeSlots = 0;

        //methods
        void handleCustomMsg(cMessage*);
        void broadcast();


    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    public:
        User();
};

#endif
