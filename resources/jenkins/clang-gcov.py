#!/usr/bin/env python

import subprocess
import sys

cmd = "llvm-cov gcov " + " ".join(sys.argv[1:])
subprocess.call(cmd, shell=True)
