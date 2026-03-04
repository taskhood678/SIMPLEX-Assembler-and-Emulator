; Success Test 2: Subroutines and Stack Pointer
; Sets up a stack, calls a function to multiply by 4 using shifts

main:
    ldc 0x1000
    a2sp        ; Initialize SP to 4096
    
    ldc 5       ; Load 5 into Accumulator
    call times4 ; Call the subroutine
    
    HALT        ; Emulator should stop here with A = 20

times4:
    ldl 0       ; Push A into B temporarily
    ldc 2       ; A = 2
    shl         ; Shift B left by A (5 << 2 = 20)
    return      ; Return to main