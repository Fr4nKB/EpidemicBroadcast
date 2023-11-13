package epidemicbroadcast;

simple User
{
    parameters:
        double coordX = default(0);
        double coordY = default(0);
        
        @signal[collision](type="unsigned long");    
    gates:
        //two way connections
        input gateIN[];
        output gateOUT[];
}
