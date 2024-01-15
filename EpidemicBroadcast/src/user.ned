package epidemicbroadcast;

simple User
{
    parameters:
        @display("b=5,5,rect,white");
        double coordX = default(0);
        double coordY = default(0);
        int relayDelay = default(1);
        
        @signal[covered](type="double");
        @signal[coveredStatus](type="unsigned short");
        @signal[collision](type="unsigned int");
        
        @statistic[coverageVectorStat](source="coveredStatus"; record=vector;);
        @statistic[collisionStat](source="collision"; record=vector;);
        
    gates:
        input dirInGate @directIn;
}
