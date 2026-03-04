; Success Test 1: Countdown Loop and Addition
; Calculates 5+4+3+2+1 and stores the sum (15)

start:
    ldc 0
    stl 0       ; sum = 0
    ldc 5
    stl 1       ; counter = 5

loop:
    ldl 1
    brz done    ; if counter == 0, jump to done
    
    ldl 0
    ldl 1
    add
    stl 0       ; sum = sum + counter
    
    ldl 1
    adc -1
    stl 1       ; counter--
    br loop     ; jump back to start of loop

done:
    HALT