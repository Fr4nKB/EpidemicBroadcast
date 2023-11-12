package epidemicbroadcast;

simple User
{
    parameters:
        double coordX = default(0);
        double coordY = default(0);
    
    gates:
        //two way connections
        inout gate[];
}
