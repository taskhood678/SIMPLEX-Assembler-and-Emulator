; Fail Test 2: Emulator Runtime Protections
; This will assemble fine, but will be caught by the emulator's safety checks.

start:
    ldc 0
    a2sp             ; Sets the Stack Pointer to 0
    adj -5           ; SP is now -5 (An illegal negative memory address!)
    
    ldc 99
    stl 0            ; Tries to write to memory[-5]. SEGFAULT ERROR!

; --- Note for your Viva ---
; If you delete the code above and run the code below instead, 
; it will test your C++ Shift Protection logic:

    ldc 5
    ldl 0            ; Pushes 5 into B
    ldc 35           ; A = 35 (You cannot shift a 32-bit number by 35!)
    shl              ; SHIFT OUT OF BOUNDS ERROR!
    
    HALT