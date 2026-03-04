Start:
    ldc 0x1000
    a2sp
    adj -4

OuterLoop:
    ldc 0
    stl 0
    ldc Array
    stl 1
    ldc 9
    stl 2

InnerLoop:
    ldl 2
    brz EndInner
    
    ldl 1
    ldnl 0
    ldl 1
    ldnl 1
    sub
    brlz NoSwap
    brz NoSwap

Swap:
    ldl 1
    ldnl 0
    stl 3
    
    ldl 1
    ldnl 1
    ldl 1
    stnl 0
    
    ldl 3
    ldl 1
    stnl 1
    
    ldc 1
    stl 0

NoSwap:
    ldl 1
    adc 1
    stl 1
    
    ldl 2
    adc -1
    stl 2
    
    br InnerLoop

EndInner:
    ldl 0
    brz Done
    br OuterLoop

Done:
    HALT

Array:
    data 5
    data 3
    data 8
    data 1
    data 9
    data 2
    data 7
    data 4
    data 6
    data 0