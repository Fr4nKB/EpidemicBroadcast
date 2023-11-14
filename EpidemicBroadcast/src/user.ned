package epidemicbroadcast;

simple User
{
    parameters:
        double coordX = default(0);
        double coordY = default(0);
        
        @signal[collision](type="string");
        
    gates:
        //two way connections
        input gateIN[];
        output gateOUT[];
        output gateSupOut;
}
