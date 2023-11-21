package epidemicbroadcast.simulations;

import epidemicbroadcast.User;
import epidemicbroadcast.Supervisor;

network Floor
{
    parameters:
        int userNumber = default(100);
        
        //floor dimensions
        double floorX = default(150);
		double floorY = default(150);
        
        //slot duration
        double slotDuration @unit(s) = default(1s);
        
        //number of time slots to wait before msg relay
        int nSlot2Wait = default(5);
        
        //number of maximum msg copies
        int maxMsgCopies = default(3);
        
        //user radius
        double radius = default(20);
        
        //choose a random user as producer
        int producerIndex = default(intuniform(0, userNumber-1));
        
        @statistic[simTimeEndStat](source="simTimeEndSig"; record=last;);  
        @statistic[coverageStat](source="covered"; record=sum;);
        
    submodules:
        sup: Supervisor;
        user[userNumber]: User {
        	parameters:
            	coordX = default(uniform(0, parent.floorX));
            	coordY = default(uniform(0, parent.floorY));
            	@display("p=$coordX, $coordY");
                @display("r=$radius");
        }
            
}
