void _start(void)
{
  asm("mov $60,%rax;"
      "xor %rdi,%rdi;"
      "syscall");
}
