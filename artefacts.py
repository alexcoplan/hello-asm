def describe(env):
  env.Program('minimal', ['minimal.c', 'arm/ajc_linux.s'], ldflags='-nostdlib')
