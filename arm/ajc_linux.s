.text
.global ajc_exit
ajc_exit:
  mov %r7, $1 // exit is syscall #1
  swi $0
