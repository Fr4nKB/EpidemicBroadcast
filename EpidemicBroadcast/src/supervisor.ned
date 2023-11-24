package epidemicbroadcast;

simple Supervisor
{   
    parameters:
        @signal[simTimeEndSig](type="double");
        
    gates:
        input dirInGate @directIn;
}
