#!/usr/bin/env python

import subprocess
import sys

req_version = (3, 5)
cur_version = sys.version_info

if cur_version < req_version:
    print('python version >=3.5 required')
    sys.exit(1)

args = str(sys.argv)

# Alpine
alpine = [
    'alpine:swarm',
    'alpine:trigger',
    'alpine:sync',
    'alpine:common',
    'alpine:docs',
    'alpine:linux',
]

# Ubuntu
ubuntu = ['ubuntu:swarm', 'ubuntu:android', 'ubuntu:vanilla']

images = alpine + ubuntu

if 'build' in args:
    for image in images:
        dir = image.replace(':', '-')
        print('Building %s in directory %s' % (image, dir))
        proc_args = [
            'docker',
            'build',
            '-t',
            'dev-docker.governikus.de/ausweisapp2/%s' % (image),
            dir,
        ]
        subprocess.run(proc_args).check_returncode()


if 'push' in args:
    for image in images:
        print('Push %s' % image)
        proc_args = [
            'docker',
            'push',
            'dev-docker.governikus.de/ausweisapp2/%s' % (image),
        ]
        subprocess.run(proc_args).check_returncode()
