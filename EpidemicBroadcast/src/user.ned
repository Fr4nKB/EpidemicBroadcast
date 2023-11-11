package epidemicbroadcast;

simple User
{
    parameters:
        double coordX;
        double coordY;
    
    gates:
        //two way connections
        inout gate[];
}
