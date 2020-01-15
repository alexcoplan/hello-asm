.text
.global ajc_add
.global ajc_strlen
.global ajc_atoi
ajc_add:
  add %r0, %r0, %r1
  bx lr

// r0 is const char *str
// r1 is our accumulator
// r2 is a temporary for the current byte
ajc_strlen:
  mov %r1, #0
loop:
  ldrb %r2, [%r0,%r1] // read a byte
  cmp %r2, #0
  beq done
  add %r1, #1
  b loop
done:
  mov %r0, %r1
  bx lr

// r0 is const char *str
// r1 is int *result
// r2, r3: temporaries
// r4 stores negation
// r5 is accumulator
// r6 used as temp for multiplication
ajc_atoi:
  ldrb %r2, [%r0]
  mov %r4, #0
  cmp %r2, $'-
  addeq %r4, #1
  addeq %r0, #1
  ldrb %r2, [%r0]
  cmp %r2, #0
  beq atoi_fail // "" and "-" are invalid
  ldrb %r3, [%r0,#1]
  mov %r5, #0
  cmp %r2, $'0
  bne atoi_loop
  cmp %r3, #0
  bne atoi_fail // leading zero only valid if it's the only digit
atoi_loop:
  ldrb %r2, [%r0]
  cmp %r2, #0
  beq atoi_done
  add %r0, #1
  cmp %r2, $'0
  blt atoi_fail
  cmp %r2, $'9
  bgt atoi_fail
  ldr %r3, .imax // INT_MAX / 10
  cmp %r5, %r3
  bgt atoi_fail // would overflow
  mov %r3, #10
  mov %r6, %r5
  mul %r5, %r6, %r3
  sub %r3, %r2, $'0
  add %r5, %r3
  b atoi_loop
atoi_fail:
  mov %r0, #0
  bx lr
atoi_done:
  cmp %r4, #0
  negne %r5, %r5
  mov %r0, #1
  str %r5, [%r1]
  bx lr

.imax:
.word 214748364 // INT_MAX / 10
