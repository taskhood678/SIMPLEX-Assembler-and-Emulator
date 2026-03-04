; Fail Test 1: Assembler Error Catching
; This will fail assembly and generate a log full of errors.

1badlabel: ldc 0     ; Error: Bogus Label Name (Starts with a number)
bad_char:  ldc 0     ; Error: Bogus Label Name (Contains underscore)

duplicate: ldc 1
duplicate: ldc 2     ; Error: Duplicate Label Definition

fibble 5             ; Error: Unknown Instruction
ldc                  ; Error: Missing Operand Value
add 5                ; Error: Unexpected extra operand
ldc 08xyz            ; Error: Invalid Numeric Operand
br nowhere           ; Error: Undefined label

SET 10               ; Warning: SET instruction used without label
badset1: SET 08xy    ; Error: Invalid SET operand
badset2: SET         ; Error: SET missing operand

unused: data 0       ; Warning: Unused label (Declared but never called/loaded)