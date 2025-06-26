#!/usr/bin/env python

import argparse
import hashlib
import json
import logging
import os
import subprocess
import sys

import gitlab
import requests

log = logging.getLogger('gitlab')
logging.basicConfig(format='%(levelname)s: %(message)s', level='INFO')
parser = None


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        self.regex = '.*'
        self.api_path = '/api/v4/projects/'

        super().__init__()
        self.add_argument(
            '--release', help='Create release pipeline', action='store_true'
        )
        self.add_argument('-r', '--rev', help='Use revision for the pipeline')
        self.add_argument(
            '-p',
            '--patch',
            help='Use patches for the pipeline',
            nargs='?',
            const='::. and draft()',
        )
        self.add_argument(
            '-s',
            '--squash',
            help='Squash multiple changesets into a single diff',
            action='store_true',
        )
        self.add_argument(
            '-a',
            '--all',
            help='Use all jobs or a regex to select jobs for the pipeline.',
            nargs='?',
            const=self.regex,
        )
        self.add_argument(
            '-e',
            '--env',
            help='Add environment variable to the pipeline.',
            action='append',
        )
        self.add_argument(
            '--dry-run',
            help='Dry-run instead of creating a pipeline',
            action='store_true',
        )
        self.add_argument(
            '--files',
            help='Show list of secure files and exit',
            action='store_true',
        )
        self.add_argument(
            '--token',
            help='Use token or provide CI_PIPELINE_TOKEN environment variable',
        )
        self.add_argument(
            '--gitlab', help='URL of gitlab', default='https://gitlab.govkg.de'
        )
        self.add_argument(
            '--project', help='ID of gitlab project', default='786'
        )
        self.add_argument(
            '--ref', help='Git ref of gitlab project', default='main'
        )
        self.add_argument(
            '-v', '--verbose', help='Verbose output', action='store_true'
        )
        self.args = self.parse_args()

        if self.args.files and (self.args.rev or self.args.patch):
            self.error('Cannot combine --files')
        if self.args.release and self.args.patch:
            self.error('Cannot combine --release and --patch')
        if self.args.release and not self.args.rev:
            self.error('Provide revision for the release')
        if (
            not self.args.release
            and not self.args.patch
            and not self.args.rev
            and not self.args.files
        ):
            self.error('Provide revision for the pipeline')
        if not self.args.token:
            self.args.token = os.environ.get('CI_PIPELINE_TOKEN')
            if not self.args.token:
                self.error('Token is required')

        for key, value in vars(self.args).items():
            setattr(self, key, value)

    def headers(self):
        return {'PRIVATE-TOKEN': self.token}


def run(cmd):
    try:
        return subprocess.run(cmd, capture_output=True, text=True, check=True)
    except subprocess.CalledProcessError as e:
        log.error(e.stderr.strip())
        return None


def export():
    if parser.patch == 'w':
        return run(['hg', 'diff', '--nodates', '-g'])
    else:
        return run(['hg', 'export', '--nodates', '-g', '-r', parser.patch])


def patch():
    result = export()

    if not result or not result.stdout.strip():
        log.error('Patch is empty or cannot be generated')
        return None

    urlInfo = upload(result.stdout)
    if urlInfo is not None and parser.rev is None:
        branch = run(['hg', 'branch'])
        if branch is None:
            return None
        parser.rev = branch.stdout.strip()

    return urlInfo


def upload(content):
    file = hashlib.md5(content.encode('utf-8')).hexdigest()
    url = (
        parser.gitlab
        + parser.api_path
        + f'{parser.project}/packages/generic/patches/drafts/{file}.patch'
    )
    if parser.dry_run:
        log.info(content)
        log.info(f'Upload patch: {url}')
    else:
        try:
            response = requests.head(url, headers=parser.headers())

            if response.status_code == 404:
                response = requests.put(
                    url, headers=parser.headers(), data=content
                )
                if response.status_code != 201:
                    log.error(
                        'Upload patch file failed: '
                        f'{response.status_code} {response.text}'
                    )
                    return None
        except Exception as e:
            log.error(f'Cannot upload patch file: {e}')
            return None

    identifier = f'{parser.patch} / {file}'
    return [
        {'key': 'PATCH_URL', 'value': url},
        {'key': 'PATCH_IDENTIFIER', 'value': identifier},
    ]


def pipeline(variables, inputs):
    try:
        gl = gitlab.Gitlab(parser.gitlab, private_token=parser.token)
        if parser.verbose:
            gl.enable_debug()
        project = gl.projects.get(parser.project, lazy=True)
        pipeline = project.pipelines.create(
            {'ref': parser.ref, 'variables': variables, 'inputs': inputs}
        )
        log.info(f'Pipeline started: {pipeline.web_url}')
    except Exception as e:
        log.error(f'Cannot start pipeline: {e}')


def secure_files():
    url = parser.gitlab + parser.api_path + f'{parser.project}/secure_files/'
    try:
        response = requests.get(url, headers=parser.headers())
        log.info(json.dumps(response.json(), indent=2))
    except Exception as e:
        log.error(f'Cannot fetch secure files: {e}')
        return None


def main():
    global parser
    parser = CommandLineParser()

    if parser.files:
        secure_files()
        return 0

    variables = []
    inputs = {}
    regex = f'/{parser.regex}/i'

    if parser.patch:
        v = patch()
        if not v:
            return 1
        variables += v
        regex = ''

    inputs['revision'] = parser.rev
    if parser.release:
        inputs['release'] = True

    regex = f'/{parser.all}/i' if parser.all else regex
    variables += [{'key': 'SELECT_JOB_REGEX', 'value': regex}]

    if parser.squash:
        variables += [{'key': 'SPLIT', 'value': 'OFF'}]

    if parser.env:
        for entry in parser.env:
            if '=' in entry:
                key, value = entry.split('=', 1)
            else:
                key, value = entry, '1'
            variables += [{'key': key, 'value': value}]

    if parser.dry_run:
        log.info(variables)
        log.info(inputs)
    else:
        pipeline(variables, inputs)


if __name__ == '__main__':
    sys.exit(main())
