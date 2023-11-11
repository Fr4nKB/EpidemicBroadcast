package epidemicbroadcast.simulations;

import epidemicbroadcast.User;

network Floor
{
    parameters:
        int userNumber = default(100);
        
        //floor dimensions
        double floorX @unit(m) = default(150m);
		double floorY @unit(m) = default(150m);
        
        //slot duration
        double slotDuration @unit(s) = default(1s);
        
        //number of time slots to wait before msg relay
        int nSlot2Wait = default(5);
        
        //number of maximum msg copies
        int maxMsgCopies = default(3);
        
        //user radius
        double radius @unit(m) = default(20m);
        
        //choose a random user as producer
        int producerIndex = default(intuniform(0, userCount-1));
        
    submodules:
        user[nodeNumber]: User {
        	parameters:
            	coordX = uniform(0, floorX);
            	coordY = uniform(0, floorX);
            	@display("p=$coordX, $coordY");
                @display("r=$radius");
        }
        
    connections:
        for i = 0..userNumber-1, for j= 0..userNumber-1{
            user[i].gate++ <--> user[j].gate++ if i!=j && ((user[i].coordX - user[j].coordX)^2 + (user[i].coordY - user[j].coordY)^2) <= R^2;
        }
            
}
