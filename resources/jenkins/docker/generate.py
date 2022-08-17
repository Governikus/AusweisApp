#!/usr/bin/env python

import subprocess
import sys

req_version = (3, 5)
cur_version = sys.version_info

if cur_version < req_version:
    print("python version >=3.5 required")
    sys.exit(1)

# Alpine
images = ["swarm", "trigger", "common", "docs", "linux"]
for image in images:
    print("Building %s ..." % (image))
    proc_args = ["docker", "build", "-t", "alpine:%s" % (image), image]
    subprocess.run(proc_args).check_returncode()
    proc_args = ["docker", "tag", "alpine:%s" % (image),
                 "dev-docker.govkg.de/ausweisapp2/alpine:%s" % image]
    subprocess.run(proc_args).check_returncode()
