#!/usr/bin/env python

import sys

if sys.version_info < (3, 1):
    print('Python 3.1 or greater is required')
    sys.exit(1)

import os
import tempfile
import urllib.request
import subprocess
import shutil
import time


if len(sys.argv) != 2:
    print('Please provide address of jenkins server')
    print('Example: localhost:8090')
    sys.exit(1)

if not shutil.which('java'):
    print('Cannot find java')
    sys.exit(1)


filename = 'jenkins-cli.jar'
jenkinsJar = tempfile.gettempdir() + '/' + filename

if not os.path.isfile(jenkinsJar):
    try:
        url = 'http://' + sys.argv[1] + '/jnlpJars/' + filename
        print('Download ' + url)
        jenkinsJar, headers = urllib.request.urlretrieve(url, jenkinsJar)
    except Exception as e:
        print('Cannot download ' + filename + ': ' + str(e))
        sys.exit(1)
else:
    print('Use existing: ' + jenkinsJar)


def systemCall(cmd, content=None):
    print('Calling: ' + cmd)
    try:
        result = str(
            subprocess.check_output(cmd, stdin=content, shell=True), 'UTF8'
        ).strip()
    except subprocess.CalledProcessError as e:
        print('Got error: ' + str(e.output, 'UTF8'))
        return 'code: ' + str(e.returncode)

    print('Got: ' + result)
    return result


def callJenkins(cmd, content=None):
    return systemCall(
        'java -jar '
        + jenkinsJar
        + ' -noKeyAuth -s http://'
        + sys.argv[1]
        + ' '
        + cmd,
        content,
    )


def installPlugin(plugin):
    return callJenkins('install-plugin ' + ' '.join(plugin) + ' -restart')


initialSessionId = callJenkins('session-id')
alreadyInstalledPlugins = callJenkins('list-plugins')

plugins = []
plugins.append('build-timeout')
plugins.append('categorized-view')
plugins.append('coverage')
plugins.append('copyartifact')
plugins.append('cppcheck')
plugins.append('depgraph-view')
plugins.append('build-name-setter')
plugins.append('description-setter')
plugins.append('envinject')
plugins.append('extra-columns')
plugins.append('heavy-job')
plugins.append('htmlpublisher')
plugins.append('jenkins-multijob-plugin')
plugins.append('jobConfigHistory')
plugins.append('job-dsl')
plugins.append('mercurial')
plugins.append('naginator')
plugins.append('nodelabelparameter')
plugins.append('parameterized-trigger')
plugins.append('pipeline-stage-view')
plugins.append('pipeline-utility-steps')
plugins.append('postbuildscript')
plugins.append('show-build-parameters')
plugins.append('sloccount')
plugins.append('sidebar-link')
plugins.append('swarm')
plugins.append('text-finder')
plugins.append('timestamper')
plugins.append('uno-choice')
plugins.append('warnings-ng')
plugins.append('workflow-aggregator')
plugins.append('ws-cleanup')
plugins.append('xunit')

installPlugins = []

for plugin in plugins:
    if plugin not in alreadyInstalledPlugins:
        print('Try to install plugin: ' + plugin)
        installPlugins.append(plugin)


if installPlugins:
    print(
        'Try to install plugins... show progress at http://'
        + sys.argv[1]
        + '/updateCenter/'
    )
    if 'code: ' in installPlugin(installPlugins):
        print('Cannot install plugins!')
        print(
            'Maybe you need to configure a proxy '
            'in your jenkins to get update server?'
        )
        sys.exit(1)

    print('Wait until jenkins is restarted...')
    while 1:
        time.sleep(5)
        result = callJenkins('session-id')
        if result != initialSessionId and 'code: ' not in result:
            break
        print('Wait...')

    print('Jenkins restarted...')


content = open('config.xml', 'r')
if 'code: ' not in callJenkins('create-job _Seeder_', content):
    callJenkins('build _Seeder_')
