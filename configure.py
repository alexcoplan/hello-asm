#!/usr/bin/env python3

import argparse
import os
import platform
import sys
from enum import Enum
from collections import namedtuple
from typing import Dict, List, TextIO, Tuple

import artefacts # user's build definitions

base_flags  = "-g -Wall -Wextra -Wpedantic -Werror -fdiagnostics-color"
base_flags += " -Wno-error=unused-parameter -Wno-error=unused-function"
base_flags += " -Wno-gnu-zero-variadic-macro-arguments"

cflags = "$baseflags -std=c11"
cxxflags = "$baseflags -std=c++11"

ninja_vars = {
  "builddir" : "build",
  "cc" : "gcc",
  "cxx" : "g++",
  "baseflags" : base_flags,
  "cflags" : cflags,
  "cxxflags" : cxxflags,
  "ldflags" : "-L$builddir",
}

def cc_rule(name : str) -> str:
  flagvar = {
    "cc" : "cflags",
    "cxx" : "cxxflags",
  }[name]
  out = f"rule {name}\n"
  out += f"  command = ${name} -MMD -MT $out -MF $out.d ${flagvar} -c $in -o $out\n"
  out += f"  depfile = $out.d\n"
  out += f"  deps = gcc"
  return out

as_rule = """rule as
  command = as -o $out $in
"""

compile_rules = list(map(cc_rule, ["cc", "cxx"])) + [as_rule]

ninjafile_base = "\n" + "\n\n".join(compile_rules)
ninjafile_base += """
rule link
  command = $cxx $ldflags -o $out $in
  description = LINK $out

"""

class Sanitizer(Enum):
  Address = 1
  Undefined = 2
  Fuzzer = 4

san_map = {
  "address" : Sanitizer.Address,
  "undefined" : Sanitizer.Undefined,
  "fuzzer" : Sanitizer.Fuzzer,
}

class SanitizerInfo:
  def __init__(self, arg : str):
    self.mask : int = 0
    self.cflags : str = ""
    self.ldflags : str = ""

    if arg is None:
      return

    sans = arg.split(',')
    for san in sans:
      assert san in san_map, f"sanitizer {san} not supported"
      ty = san_map[san]
      self.mask |= ty.value
      en_flag = f" -fsanitize={san}"
      self.cflags += en_flag
      if ty == Sanitizer.Fuzzer:
        self.ldflags += " -fsanitize=fuzzer-no-link"
      else:
        self.ldflags += en_flag
      if ty == Sanitizer.Undefined:
        no_recover = " -fno-sanitize-recover=undefined"
        self.cflags += no_recover
        self.ldflags += no_recover

class Program:
  def __init__(self, name : str, objects : List[str], ldflags : str = "") -> None:
    self.name = name
    self.objects = objects
    self.ldflags = ldflags

class ObjectFile:
  def __init__(self, src_file : str):
    self.name, self.ext = src_file.split(".")

  def is_cpp(self) -> bool:
    return self.ext == "cpp"

  def compile_rule(self) -> str:
    if self.ext == "c":
      return "cc"
    elif self.ext == "s":
      return "as"
    elif self.is_cpp():
      return "cxx"
    else:
      assert False, f"bad ext {self.ext}"

  def ninja_line(self) -> str:
    return f"{self.name}.o: {self.compile_rule()} {self.name}.{self.ext}\n"

class BuildEnv:
  def __init__(self, args : argparse.Namespace):
    self.san_info = SanitizerInfo(args.sanitizers)
    self.vars = ninja_vars
    self.vars["baseflags"] += self.san_info.cflags
    self.vars["ldflags"] += self.san_info.ldflags

    if args.config == "release":
      self.vars["baseflags"] += ' -O3'
    else:
      assert args.config == "debug", f"invalid config {args.config}"

    self.progs : List[Program] = []

    # maps object file names -> ObjectFile objects
    self.obj_map : Dict[str, ObjectFile] = {}

  def IsWindows(self) -> bool:
    return os.name == 'nt'

  def IsLinux(self) -> bool:
    return platform.system() == 'Linux'

  def Program(self, name : str, src : List[str], ldflags : str = "") -> None:
    objects = []
    for f in src:
      obj = ObjectFile(f)
      objects.append(obj)
      if obj.name not in self.obj_map:
        self.obj_map[obj.name] = obj

    need_libcpp = any(map(lambda x: x.is_cpp(), objects))
    if need_libcpp:
      spacer = " " if len(ldflags) > 0 else ""
      maybe_std = "std" if self.IsLinux() else ""
      ldflags += f"{spacer}-l{maybe_std}c++" # XXX: win32?

    obj_names = list(map(lambda x: x.name, objects))
    self.progs.append(Program(name, obj_names, ldflags=ldflags))

  def Test(self, name : str, src : List[str]) -> None:
    return self.Program("test/%s" % name, src + ["test/test.c"])

  def FuzzTarget(self, name : str, src : List[str]) -> None:
    if self.san_info.mask & Sanitizer.Fuzzer.value:
      return self.Program(f"fuzz/{name}", src, ldflags="-fsanitize=fuzzer")

  def write_ninja(self, fp : TextIO) -> None:
    fp.write("# auto-generated by configure.py\n")

    for k,v in self.vars.items():
      fp.write("%s = %s\n" % (k,v))

    fp.write(ninjafile_base)

    reconf_cmd = "./configure.py"
    if len(sys.argv) > 1:
      conf_args = " ".join(sys.argv[1:])
      reconf_cmd += f" {confg_args}"

    fp.write("rule reconf\n")
    fp.write(f"  command = {reconf_cmd}\n")
    fp.write(f"  generator = 1\n")

    fp.write("\n# re-configure if necessary\n")
    fp.write("build build.ninja : reconf configure.py artefacts.py\n")

    fp.write("\n# objects\n")
    for obj in self.obj_map.values():
      fp.write(f"build $builddir/{obj.ninja_line()}")

    fp.write("\n# executables\n")
    for prog in self.progs:
      obj_line = " ".join(map(lambda x: "$builddir/%s.o" % x, prog.objects))
      ext = ".exe" if self.IsWindows() else ""
      fp.write("build $builddir/%s%s: link %s\n" % (prog.name, ext, obj_line))
      if len(prog.ldflags) > 0:
        fp.write(f"  ldflags = $ldflags {prog.ldflags}\n")

def configure_build(args : argparse.Namespace) -> None:
  env = BuildEnv(args)
  artefacts.describe(env)
  with open("build.ninja", "w") as f:
    env.write_ninja(f)

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('--sanitizers', '--san', dest='sanitizers', default=None)
  parser.add_argument('--config', default='debug')
  args = parser.parse_args()
  configure_build(args)
