def describe(env):
  env.FuzzTarget('fuzz_atoi', ['fuzz_atoi.c', 'ajc_c.c'])
  env.Test('test_atoi', ['test/test_atoi.c', 'ajc_c.c'])

  if env.TargetIsArm():
    env.Program('minimal', ['minimal.c', 'arm/ajc_linux.s'], ldflags='-nostdlib')
    env.Program('big', ['big.c', 'arm/ajc_lib.s'])
