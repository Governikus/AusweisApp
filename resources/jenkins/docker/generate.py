#!/usr/bin/env python

import glob
import subprocess
import sys

req_version = (3, 5)
cur_version = sys.version_info

if cur_version < req_version:
    print("python version >=3.5 required")
    sys.exit(1)

# Alpine
alpine_images = glob.glob("alpine/alpine-minirootfs-*.tar.gz")
if len(alpine_images) == 0:
    print("No alpine image found in ./alpine")
    sys.exit(1)

print("Building base Alpine ...")
proc_args = ["docker", "build", "-t", "alpine:latest", "alpine"]
result = subprocess.run(proc_args).check_returncode()

images = ["swarm", "trigger", "common", "docs", "linux"]
for image in images:
    print("Building %s ..." % (image))
    proc_args = ["docker", "build", "-t", "alpine:%s" % (image), image]
    subprocess.run().check_returncode(proc_args)
