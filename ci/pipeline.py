#!/usr/bin/env python

import argparse
import json
import logging
import os
import subprocess
import sys
import zlib

import gitlab
import requests

log = logging.getLogger('gitlab')
logging.basicConfig(format='%(levelname)s: %(message)s', level='INFO')

stdout_handler = logging.StreamHandler(sys.stdout)
stdout_handler.setFormatter(logging.Formatter('%(message)s'))
log_stdout = logging.getLogger('gitlab.stdout')
log_stdout.setLevel(logging.INFO)
log_stdout.addHandler(stdout_handler)
log_stdout.propagate = False

parser = None


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        self.regex = '.*'
        self.api_path = '/api/v4/projects/'

        prog = os.path.basename(sys.argv[0])
        description = f"""\
Trigger a GitLab CI/CD pipeline with optional patches, revisions
and environment variables. It supports submitting Mercurial patches,
triggering release pipelines and selecting specific jobs via regex.


# Typical workflows

- Specific jobs for current draft changesets:
  {prog} -p -a Android

- Pipeline with current draft changesets as single patch:
  {prog} -p -s

- Run only selected jobs using a regex:
  {prog} -a SonarQube -r default

- Pipeline using a branch as revision:
  {prog} -r default

- Pipeline with a specific patch changeset on specific branch:
  {prog} -p tip -r default

- Release pipeline (requires --rev):
  {prog} --release -r 1.0


# Helper functionality (does not trigger a pipeline)

- Show secure files of the GitLab project:
  {prog} --files
"""

        epilog = """\
notes:
 - The token can be provided via the CI_PIPELINE_TOKEN environment variable.
"""

        super().__init__(
            description=description,
            epilog=epilog,
            formatter_class=argparse.RawDescriptionHelpFormatter,
        )
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
            '--apply',
            help='Apply regex to select jobs for the pipeline',
        )
        self.add_argument(
            '-m',
            '--manual',
            help='Enable "manual" builds of non-matching jobs',
            action='store_true',
        )
        self.add_argument(
            '-e',
            '--env',
            help='Add environment variable to the pipeline',
            action='append',
        )
        self.add_argument(
            '--dry-run',
            help='Dry-run instead of creating a pipeline',
            action='store_true',
        )
        self.add_argument('--repository', help='Use local repository')
        self.add_argument(
            '--files',
            help='Show list or download of secure files and exit',
            nargs='?',
            const=' ',
        )
        self.add_argument(
            '--packages',
            help='Show list of packages and exit',
            nargs='?',
            const=' ',
        )
        self.add_argument(
            '--download-logs',
            help='Download all logs from pipeline (CI_PIPELINE_ID)',
            action='store_true',
        )
        self.add_argument(
            '--show',
            help='Show list of pipelines and exit',
            nargs='?',
            const='running',
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
        if self.args.show and (self.args.rev or self.args.patch):
            self.error('Cannot combine --show')
        if self.args.files and self.args.show:
            self.error('Cannot combine --files and --show')
        if self.args.release and self.args.patch:
            self.error('Cannot combine --release and --patch')
        if self.args.release and not self.args.rev:
            self.error('Provide revision for the release')
        if (
            not self.args.release
            and not self.args.patch
            and not self.args.rev
            and not self.args.files
            and not self.args.show
            and not self.args.packages
            and not self.args.download_logs
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


def get_url():
    return parser.gitlab + parser.api_path + parser.project


def load_hgrc():
    hgrc = {}
    if output := run(['hg', 'showconfig']):
        for line in output.stdout.strip().splitlines():
            line = line.split('=', 1)
            if len(line) == 2:
                key, value = line
            else:
                key = line[0]
                value = ''
            hgrc[key] = value.strip()
    return hgrc


def run(cmd):
    try:
        return subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=True,
            cwd=parser.repository,
            encoding='utf-8',
        )
    except subprocess.CalledProcessError as e:
        log.error(e.stderr.strip())
        return None


def export():
    if parser.patch == 'w':
        return (run(['hg', 'diff', '--nodates', '-g']), ['wdir'])
    else:
        nodes = run(['hg', 'log', '-T', '{node|short} ', '-r', parser.patch])
        return (
            run(['hg', 'export', '--nodates', '-g', '-r', parser.patch]),
            nodes.stdout.strip().split() if nodes else ['unknown'],
        )


def patch():
    if not parser.repository:
        parser.repository = load_hgrc().get(
            'bundle.mainreporoot'
        ) or os.path.dirname(os.path.realpath(__file__))
    log.debug('Use working directory: %s', parser.repository)
    result, nodes = export()

    if not result or not result.stdout.strip():
        log.error('Patch is empty or cannot be generated')
        return None

    urlInfo = upload(result.stdout, nodes)
    if urlInfo is not None and parser.rev is None:
        branch = run(['hg', 'branch'])
        if branch is None:
            return None
        parser.rev = branch.stdout.strip()

    return urlInfo


def upload(content, nodes):
    crc = zlib.crc32(content.encode('utf-8'))
    url = (
        f'{get_url()}/packages/generic/patch/{crc:08x}/{"_".join(nodes)}.patch'
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

    return [
        {'key': 'PATCH_URL', 'value': url},
        {'key': 'PATCH_IDENTIFIER', 'value': ' '.join(nodes)},
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
        return False
    return True


def api(url, stdout=True):
    log.debug(url)
    try:
        response = requests.get(url, headers=parser.headers())
        log.debug(f'HTTP Code: {response.status_code}')
        log.debug(json.dumps(dict(response.headers), indent=2))
        if 'application/json' in response.headers.get('Content-Type', ''):
            content = json.dumps(response.json(), indent=2)
        else:
            content = response.text
        if stdout:
            log_stdout.info(content)

        if response.status_code >= 400:
            raise requests.HTTPError(f'HTTP Error {response.status_code}')

        return (response.headers, content)
    except Exception as e:
        log.error(f'Cannot fetch api {url}: {e}')
        return (None, None)


def secure_files():
    url = get_url() + '/secure_files/'
    if parser.files.strip():
        url += parser.files + '/download'
    headers, _ = api(url)
    return headers is not None


def show():
    url = get_url()
    if parser.show.isdigit():
        url += f'/pipelines/{parser.show}/'
    else:
        url += f'/pipelines?status={parser.show}'
    headers, _ = api(url)
    return headers is not None


def packages():
    from datetime import datetime, timedelta, timezone

    now = datetime.now(timezone.utc)
    page = '1'
    while page:
        url = get_url() + f'/packages?page={page}'
        (headers, content) = api(url, False)
        if not headers:
            return 1
        page = headers['X-Next-Page']

        if content:
            for entry in json.loads(content):
                tsp = datetime.fromisoformat(
                    entry.get('last_downloaded_at') or entry.get('created_at')
                )
                diff = now - tsp
                if diff > timedelta(days=7):
                    log.info(
                        f'Package obsolete: {entry["id"]} '
                        f'| {entry["version"]} '
                        f'| {entry["name"]}'
                    )
                    if parser.packages == 'delete':
                        log.info('Delete outdated package')
                        response = requests.delete(
                            entry['_links']['delete_api_path'],
                            headers=parser.headers(),
                        )
                        if response.status_code != 204:
                            log.error(
                                'Deletion of package failed: '
                                f'{response.status_code} {response.text}'
                            )
                            return False
                else:
                    log.info(
                        f'Package relevant: {entry["id"]} '
                        f'| {entry["version"]} '
                        f'| {entry["name"]}'
                    )
    return True


def download_logs():
    pipeline_id = os.environ.get('CI_PIPELINE_ID')
    if not pipeline_id:
        log.error(
            'CI_PIPELINE_ID environment variable not set. This must run within a GitLab CI job.'
        )
        return 1

    log_dir = os.path.join(os.getcwd(), 'logs')
    os.makedirs(log_dir, exist_ok=True)
    log.info(f'Saving logs to: {log_dir}')

    all_jobs = []
    page = 1
    per_page = 100  # Max allowed by GitLab API

    while True:
        url = (
            get_url()
            + f'/pipelines/{pipeline_id}/jobs?page={page}&per_page={per_page}'
        )
        log.debug(f'Fetching page {page}...')

        try:
            response = requests.get(url, headers=parser.headers())
            if response.status_code != 200:
                log.error(
                    f'Failed to fetch pipeline jobs: {response.status_code} {response.text}'
                )
                return 1

            jobs = response.json()
            if not jobs:
                break

            all_jobs.extend(jobs)
            log.debug(f'Page {page}: {len(jobs)} jobs')

            next_page = response.headers.get('X-Next-Page')
            if not next_page:
                break

            page += 1

        except Exception as e:
            log.error(f'Error fetching pipeline jobs (page {page}): {e}')
            return 1

    log.info(f'Found {len(all_jobs)} jobs in pipeline {pipeline_id}')

    for job in all_jobs:
        job_id = job['id']
        job_name = job['name']
        job_status = job['status']

        # Sanitize job name for filename
        safe_name = ''.join(
            c if c.isalnum() or c in ('-', '_') else '_' for c in job_name
        )
        log_file = os.path.join(log_dir, f'{safe_name}_{job_id}.log')

        log.info(
            f'Downloading log: {job_name} (ID: {job_id}, Status: {job_status})'
        )

        log_url = get_url() + f'/jobs/{job_id}/trace'
        try:
            log_response = requests.get(log_url, headers=parser.headers())
            if log_response.status_code == 200:
                with open(log_file, 'w', encoding='utf-8') as f:
                    f.write(log_response.text)
                log.info(f'   Saved to: {log_file}')
            else:
                log.warning(
                    f'   Could not download log: {log_response.status_code}'
                )
        except Exception as e:
            log.error(f'   Error downloading log for {job_name}: {e}')

    log.info(f'Successfully downloaded {len(all_jobs)} logs to {log_dir}')
    return 0


def main():
    global parser
    parser = CommandLineParser()

    if parser.verbose:
        log.setLevel('DEBUG')

    if parser.files:
        return secure_files()

    if parser.show:
        return show()

    if parser.packages:
        return packages()

    if parser.download_logs:
        return download_logs()

    variables = []
    inputs = {}

    if parser.patch:
        v = patch()
        if not v:
            return 1
        variables += v

    inputs['revision'] = parser.rev
    if parser.release:
        inputs['release'] = True

    if parser.apply:
        variables += [
            {'key': 'SELECT_JOB_REGEX', 'value': f'/{parser.apply}/i'}
        ]
    if parser.squash:
        variables += [{'key': 'SPLIT', 'value': 'OFF'}]

    if parser.manual:
        variables += [{'key': 'SELECT_JOB_MANUAL', 'value': ''}]

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
        return pipeline(variables, inputs)


if __name__ == '__main__':
    sys.exit(main())
