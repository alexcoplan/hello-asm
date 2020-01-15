void _start(void)
{
  asm("mov $60,%rax;"
      "mov $1,%rdi;"
      "syscall");
}
