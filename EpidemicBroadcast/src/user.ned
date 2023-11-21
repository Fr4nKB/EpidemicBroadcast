package epidemicbroadcast;

simple User
{
    parameters:
        double coordX = default(0);
        double coordY = default(0);
        
        @signal[covered](type="double");
        @signal[collision](type="unsigned int");
        @statistic[collisionStat](source="collision"; record=vector;);
        
    gates:
        input dirInGate @directIn;
}
