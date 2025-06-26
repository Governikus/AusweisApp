#!/usr/bin/env python

import hglib
from hglib.util import b

import argparse
import os
import glob
import sys

from pathlib import Path


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        super().__init__()
        self.add_argument(
            '--clean', help='Clean up patches', action='store_true'
        )
        self.add_argument(
            '--clean-only', help='Clean up only', action='store_true'
        )
        self.add_argument(
            '--clean-dir-only',
            help='Clean up working directory only',
            action='store_true',
        )
        self.add_argument(
            '--split', help='Apply patches split', action='store_true'
        )
        self.add_argument(
            '--repatch', help='Repatch previous series', action='store_true'
        )
        self.add_argument(
            '--patch', help='Patch file', default='../patch.diff'
        )
        self.args = self.parse_args()

        for key, value in vars(self.args).items():
            setattr(self, key, value)


class PatchProcessor:
    def __init__(self, file, repatch):
        self.file = file.decode('utf-8')
        self.dir = os.path.dirname(os.path.realpath(self.file))
        self.patch_series = sorted(
            glob.glob(self.dir + os.sep + '*.patch'),
            key=lambda x: int(Path(x).stem),
        )
        self.applied = self.dir + os.sep + 'applied'
        self.applied_patches = []

        if os.path.isfile(self.applied):
            with open(self.applied, 'r') as f:
                self.applied_patches = list(filter(None, f.read().split('\n')))
                if repatch and len(self.applied_patches) > 0:
                    self.applied_patches.pop()

    def clean(self):
        if os.path.isfile(self.applied):
            os.remove(self.applied)
        for patch in self.patch_series:
            os.remove(patch)

        self.patch_series = []
        self.applied_patches = []

    def split(self):
        def nextFilename():
            self.patch_series.append(
                self.dir + os.sep + str(len(self.patch_series)) + '.patch'
            )
            return open(self.patch_series[-1], 'wb')

        with open(self.file, 'rb') as f:
            file = nextFilename()
            for line in f:
                if line == b'# HG changeset patch\n' and file.tell() > 0:
                    file.close()
                    file = nextFilename()
                file.write(line)
            file.close()

    def isSplit(self):
        return len(self.patch_series) > 0

    def apply(self, client, split):
        if self.patch_series == self.applied_patches or not split:
            print('Apply all patches: %s' % len(self.patch_series))
            client.import_(
                [s.encode('utf-8') for s in self.patch_series], nocommit=True
            )
            return

        with open(self.applied, 'w') as f:
            for patch in self.patch_series:
                pending = patch not in self.applied_patches
                print('Apply patch: {} (pending: {})'.format(patch, pending))
                client.import_(
                    [patch.encode('utf-8')],
                    user='CI',
                    date='now',
                    message=patch,
                    nocommit=pending,
                )

                f.write(patch)
                f.write('\n')
                if pending:
                    print('Pending patch: {}'.format(patch))
                    break


def main():
    parser = CommandLineParser()

    patch = b(parser.patch)
    if not os.path.isfile(patch):
        print('Patch file "{}" does not exists'.format(patch))
        return -1

    if 'CI' not in os.environ:
        print('Warning: This script should be called on CI only')
        return -1

    cfg = [
        'extensions.hgext.purge=',
        'extensions.hgext.strip=',
        'patch.eol=auto',
        'phases.new-commit=draft',
    ]
    client = hglib.open(configs=cfg)

    rev = client.identify(id=True).replace(b('+'), b('')).rstrip()
    print('Revert workspace to: ' + str(rev))
    client.update(rev=rev, clean=True)

    print('Purge workspace...')
    client.rawcommand([b('purge'), b('--all')])

    if parser.clean_dir_only:
        return 0

    revs = len(client.log(revrange='secret() or draft()'))
    print('Found secret/draft changesets: {}'.format(revs))

    if revs > 0:
        print('Strip secret and draft changesets...')
        client.rawcommand(
            [
                b('strip'),
                b('-r'),
                b('secret() or draft()'),
                b('--no-backup'),
                b('--force'),
            ]
        )

    processor = PatchProcessor(patch, parser.repatch)
    if parser.clean or parser.clean_only:
        processor.clean()

    if parser.clean_only:
        return 0

    if not processor.isSplit():
        print('Split patch: {}'.format(patch))
        processor.split()
        print('Wrote {} patch(es)'.format(len(processor.patch_series)))

    processor.apply(client, parser.split)
    return 0


if __name__ == '__main__':
    sys.exit(main())
