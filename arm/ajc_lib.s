.text
.global ajc_add
.global ajc_strlen
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
